from . import ast
from .ast import ContractData, YulNode, walk_dfs, create_yul_node, create_yul_number_literal, create_yul_identifier
import functools
from typing import List, Dict, Set, Optional, Iterable
import logging
import re


def prune_dead_functions(block_node: YulNode, roots: Iterable[str]):
    '''Prune dead functions from the given block.

    This works by computing the set of functions reachable from the "roots",
    and then removing all non-reachable functions.

    :param block_node: The node containing the function definitions.
    :param roots: The (names of the) functions that reachability is computed
    with respect to.
    '''

    # build reachability graph edges
    edges: Dict[str, List[str]] = {
        n.get_fun_name(): list()
        for n in block_node.children if n.is_fun_def()
    }

    def visit_fun_body(fun_name: str, node: YulNode, parents: List[YulNode]):
        if node.is_fun_call():
            name = node.get_fun_name()
            edges[fun_name].append(name)

    for n in block_node.children:
        if n.is_fun_def():
            walk_dfs(n, functools.partial(visit_fun_body, n.get_fun_name()))

    # Compute reachable function definitions
    reachable: Set[str] = set(roots)
    worklist: Set[str] = reachable.copy()
    while worklist:
        n = worklist.pop()
        # Note: could contain edge to intrinsic, which is not in function list.
        for m in edges.get(n, []):
            if m not in reachable:
                reachable.add(m)
                worklist.add(m)

    # Remove unreachable functions
    block_node.children.underlying[:] = (
        n for n in block_node.children.underlying
        if not YulNode(n).is_fun_def()
        or YulNode(n).get_fun_name() in reachable
    )


def _prune_dead_functions_logging(blk_node: YulNode,
                                  root_nodes: Iterable[str],
                                  logger: Optional[logging.Logger]):
    """Prune dead functions, logging the change in statement count."""
    assert blk_node.type == 'yul_block'
    cnt_stmts_before = len(blk_node.children)
    prune_dead_functions(blk_node, root_nodes)
    cnt_stmts_after = len(blk_node.children)
    if logger:
        logger.debug('Stmt count after removing unreachable functions: '
                     f'{cnt_stmts_before} -> {cnt_stmts_after}')


def prune_deploy_obj(contract: ContractData,
                     logger: Optional[logging.Logger] = None):
    '''Prune unnecessary functions from the contract_body.

    This removes everything except the constructor and all functions reachable
    by the constructor.

    This also tags the main contract name in the metadata.

    :param deploy_node: The node to remove.
    '''

    deploy_node: YulNode = YulNode(contract.yul_ast['contract_body'])

    if logger:
        logger.debug('BEGIN PASS prune_deploy_obj')

    assert deploy_node.type == 'yul_code', (
        'Expected yul_code, got ' + deploy_node.type
    )

    blk_node: YulNode = deploy_node.children[0]
    assert blk_node.type == "yul_block"

    # Identify the main constructor
    ctor_calls: List[YulNode] = [
        n for n in blk_node.children
        if n.is_fun_call()
        and n.get_fun_name().startswith('constructor_')
    ]
    assert len(ctor_calls) <= 1, (
        f'expected only 1 main constructor, got {len(ctor_calls)}'
    )
    if len(ctor_calls) == 0:
        main_ctor: str = ""
    else:
        main_ctor: str = ctor_calls[0].get_fun_name()
    contract.metadata.main_ctor = main_ctor
    if logger:
        logger.debug(f'Main constructor is: {main_ctor}')

    cnt_stmts_before = len(blk_node.children)
    cnt_stmts_after = len(blk_node.children)
    if logger:
        logger.debug('Stmt count after removing unreachable functions: '
                     f'{cnt_stmts_before} -> {cnt_stmts_after}')

    blk_node.children.underlying[:] = (
        n for n in blk_node.children.underlying
        if YulNode(n).is_fun_def()
    )
    cnt_stmts_after2 = len(blk_node.children)

    if logger:
        logger.debug('Stmt count after removing non-function-def statements: '
                     f'{cnt_stmts_after} -> {cnt_stmts_after2}')
        logger.debug('END PASS prune_deploy_obj')


def prune_deployed_code(contract: ContractData,
                        logger: Optional[logging.Logger] = None):
    '''Move the selector into a separate function, and then remove redundant
    functions.

    This also populates the "external_fun" metadata.

    '''

    if logger:
        logger.debug('BEGIN PASS prune_deployed_code')

    obj_node: YulNode = YulNode(contract.yul_ast['object_body']['contract_body'])
    blk_node: YulNode = obj_node.children[0]
    assert blk_node.type == 'yul_block'

    stmt_cnt_begin = len(blk_node.children)

    # Delete the memory guard
    mem_guard_stmt = blk_node.children[0]
    if mem_guard_stmt.is_fun_call() and mem_guard_stmt.get_fun_name() == 'mstore':
        blk_node.children.underlying.remove(mem_guard_stmt.obj)
    elif logger:
        logger.warning('Memory guard not found, skipping memory guard delete')

    # Move all free-standing stmts into a selector function
    to_move = [(i, s) for i, s in enumerate(blk_node.children)
               if not s.is_fun_def()]
    for i, s in reversed(to_move):
        blk_node.children.underlying.pop(i)

    selector_fun_name = '_pyul_selector'
    new_selector_fun = create_yul_node('yul_function_definition', [
        create_yul_node('yul_identifier', [selector_fun_name]),
        create_yul_node('yul_block', [s for _, s in to_move])
    ])
    assert new_selector_fun.is_fun_def()
    blk_node.children.underlying.insert(0, new_selector_fun.obj)

    # Populate the metadata
    def walk_selector(node: YulNode, parents: List[YulNode]):
        # TODO: store the selector
        if node.type == 'yul_case':
            # FIXME: flatten yul_literal->yul_number_literal->yul_hex_literal
            selector: str = node.children[0].children[0].children[0].children[0].obj
            case_body = node.children[1]
            case_body_fun_calls:List[YulNode] = []
            for stmt in case_body.children:
                if stmt.is_fun_call():
                    case_body_fun_calls.append(stmt)
            assert len(case_body_fun_calls) == 1, f'Incorrect number of function in selector case body'
            contract.metadata.external_fns[selector] = case_body_fun_calls[0].get_fun_name()

    walk_dfs(new_selector_fun, walk_selector)

    # Prune any dead functions
    prune_dead_functions(blk_node, [selector_fun_name])

    stmt_cnt_end = len(blk_node.children)

    if logger:
        logger.debug('Stmt count change: '
                     f'{stmt_cnt_begin} -> {stmt_cnt_end}')
        logger.debug('END PASS prune_deployed_code')


def attach_storage_layout(contract: ContractData,
                          logger: Optional[logging.Logger] = None):
    """Attaches the storage layout information to the metadata node.

    This populates the state variable list and the type information dictionary.
    """
    contract.metadata.state_vars.extend(
        ast.YulStateVar(
            name=entry['label'],
            type=entry['type'],
            offset=int(entry['offset']),
            slot=int(entry['slot']),
        )
        for entry in contract.storageLayout['storage']
    )

    def parse_type(type_dict: dict) -> ast.YulType:
        if type_dict['encoding'] == 'inplace':
            if type_dict['label'].startswith('struct '):
                return ast.YulStructType(
                    pretty_name=type_dict['label'],
                    fields={
                        td['label']: ast.YulStateVar(
                            name=td['label'],
                            type=td['type'],
                            slot=int(td['slot']),
                            offset=int(td['offset']),
                        )
                        for td in type_dict['members']
                    }
                )
            else:
                return ast.YulIntType(
                    pretty_name=type_dict['label'],
                    size=int(type_dict['numberOfBytes'])
                )
        elif type_dict['encoding'] == 'mapping':
            return ast.YulMappingType(
                pretty_name=type_dict['label'],
                key=type_dict['key'],
                value=type_dict['value'],
            )
        elif type_dict['encoding'] == 'bytes':
            return ast.YulBytesType(
                pretty_name=type_dict['label'],
                size=int(type_dict['numberOfBytes']),
            )

        if logger:
            logger.error(f'Unknown type: {type_dict}')
        raise NotImplementedError

    typs: Optional[dict] = contract.storageLayout.get('types', {})
    # This can be None for some reason, so explicitly guard against it.
    if typs is not None:
        contract.metadata.types.update({
            name: parse_type(entry)
            for name, entry in typs.items()
        })


def rewrite_storage_ops(contract: ContractData,
                        logger: Optional[logging.Logger] = None):
    '''Rewrites all sload/store operations into higher level operations.

    * The storage layout will be used to rewrite sload/store of constants into
      `pyul_sload` and `pyul_sstore` ops.
    '''

    # Create a map of storage (slots, offsets) to storage vars
    svars = {
        (v.slot, v.offset): (v.name, v.type) for v in contract.metadata.state_vars
    }

    # Regexes for rewrite rules
    # Note: we don't handle the following variants:
    # - read_from_storage_offset_dynamic
    load_re = re.compile('^read_from_storage(?P<issplit>_split)?_offset_(?P<offset>[0-9]+)_(?P<type>.*)$')
    update_re = re.compile('^update_storage_value_offset_(?P<offset>[0-9]+)?(?P<src_type>.*)_to_(?P<dest_type>)')
    dynamic_load_re = re.compile(r'^read_from_storage_split_dynamic_(?P<type>.*)')

    def rewrite_load_op(node:YulNode, match:re.Match)->bool:
        matches = match.groupdict()
        # Rewrite to function call: pyul_storage_var_load(name, ty)
        node.obj['children'][0]['children'][0] = '__pyul_storage_var_dynamic_load'
        node.obj['children'].insert(2, create_yul_number_literal(matches['offset']).obj)
        if logger:
            logger.debug('Rewrote storage load')        

    def rewrite_store_op(node:YulNode, match)->bool:
        matches = match.groupdict()
        node.obj['children'][0]['children'][0] = '__pyul_storage_var_update'
        node.obj['children'].insert(2, create_yul_number_literal(matches['offset']).obj)
        if logger:
            logger.debug('Rewrote storage update')  


    def rewrite_dyn_load_op(node:YulNode, match)->bool:
        node.obj['children'][0]['children'][0] = '__pyul_storage_var_dynamic_load'
        if logger:
            logger.debug('Rewrote storage update')    

    def rewrite_storage_ops(node: YulNode, parents: List[YulNode]) -> bool:
        if not node.is_fun_call():
            return True

        # rewrite load from storage --> pyul_storage_var_load()
        fname = node.get_fun_name()
        if (match := load_re.match(fname)):
            rewrite_load_op(node, match)

        # rewrite update to storage --> pyul_storage_var_update()
        elif (match := update_re.match(fname)):
           rewrite_store_op(node, match)

        elif (match := dynamic_load_re.match(fname)):
           rewrite_dyn_load_op(node, match)
        return True

    walk_dfs(contract.yul_ast['contract_body'], rewrite_storage_ops)
    walk_dfs(contract.yul_ast['object_body']['contract_body'], rewrite_storage_ops)

    # If all storage helpers are removed, we should be able to prune a lot of
    # other helpers.
    _prune_dead_functions_logging(
        YulNode(contract.yul_ast['object_body']['contract_body']).children[0],
        {'_pyul_selector'}, logger)
    _prune_dead_functions_logging(
        YulNode(contract.yul_ast['contract_body']).children[0],
        {contract.metadata.main_ctor}, logger)

def rewrite_map(contract: ContractData,
                        logger: Optional[logging.Logger] = None):
    map_index_re = r'^mapping_index_access_t_mapping(?P<types>.*)'
    def rewrite_mapping_index(node: YulNode, parents: List[YulNode]) -> bool:
        if not node.is_fun_call():
            return True
        fun_name = node.get_fun_name()
        match = re.match(map_index_re, fun_name)
        if(match):
            node.children[0].obj['children'] = ["__pyul_map_index"]

    # rewrite mapping
    walk_dfs(contract.yul_ast['contract_body'], rewrite_mapping_index)
    walk_dfs(contract.yul_ast['object_body']['contract_body'], rewrite_mapping_index)


def rewrite_shift_left(contract: ContractData,
                        logger: Optional[logging.Logger] = None):
    shift_left_re = re.compile(r'^shift_(?P<direction>.*)_(?P<amount>\d+)$')
    def _rewrite_shift_left(node:YulNode, parents: List[YulNode]):
        if(not node.is_fun_call()):
            return True
        match = shift_left_re.match(node.get_fun_name())
        if(match):
            vals = match.groupdict()
            fun_name = "sh"+vals['direction'][0]
            node.children.underlying[:] = create_yul_node(node.type, [
                create_yul_identifier(fun_name), 
                node.children[1],
                create_yul_number_literal(vals['amount'])
            ]).children.underlying
    walk_dfs(contract.yul_ast['contract_body'], _rewrite_shift_left)
    walk_dfs(contract.yul_ast['object_body']['contract_body'], _rewrite_shift_left)
    
'''
In yul, the selector is stored in memory and the pointer to that memory is 
passed as 5th argument to call. Since selector passes through memory, it is difficult to propagate constant value 
of selector in cpp component, therefore here,  strategy is to pattern match selector argument and infer rest of them 
in the cpp component. 
'''
def rewrite_call_inst(contract: ContractData,
                        logger: Optional[logging.Logger] = None):
    call_re = re.compile("^call$")

    def get_selector(node: YulNode, parents: List[YulNode]):
        parents.reverse()
        child = node
        for parent in parents:
            if(parent.is_block()):
                break
            child = parent
        statements = parent.children.underlying
        idx = statements.index(child.obj)
        selector_var = None
        while idx >= 0:
            idx -= 1
            node = YulNode(statements[idx])
            if(node.is_fun_call_and_name('mstore')):
                shl_call = node.children[2]
                if(shl_call.is_fun_call_and_name('shl')):
                    # second argument to shl is the selector var
                    selector_var = shl_call.children[1].children[0].obj
                    statements.remove(statements[idx])

        assert selector_var is not None, "Could not find selector"

        for s in statements:
            node = YulNode(s)
            if node.is_var_decl_with_name(selector_var):
                # assignment to the var declaraion is the constant value selector
                return node.children[1].get_literal_value()
        assert False, "Could not find selector"

    def _rewrite_call(node: YulNode, parents:List[YulNode]):
        if not node.is_fun_call():
            return

        if call_re.match(node.get_fun_name()):
            # 5th (idx = 4) argument to call argument is the function selector
            if node.children[4].type == 'yul_literal':
                selector = node.children[4].get_literal_value()
            else:
                selector = get_selector(node, parents)
            selector_node = create_yul_number_literal(selector)
            node.children[4] = selector_node


    walk_dfs(contract.yul_ast['contract_body'], _rewrite_call)
    walk_dfs(contract.yul_ast['object_body']['contract_body'], _rewrite_call)
