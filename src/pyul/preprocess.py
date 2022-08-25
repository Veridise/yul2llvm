from . import ast
from .ast import ContractData, YulNode, walk_dfs, create_yul_node
import functools
from typing import List, Dict, Set, Optional, Iterable
import logging


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

    def visit_fun_body(fun_name: str, node: YulNode):
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
    assert len(ctor_calls) == 1, (
        f'expected only 1 main constructor, got {len(ctor_calls)}'
    )
    main_ctor: str = ctor_calls[0].get_fun_name()
    contract.metadata.main_ctor = main_ctor
    if logger:
        logger.debug(f'Main constructor is: {main_ctor}')

    cnt_stmts_before = len(blk_node.children)
    prune_dead_functions(blk_node, {main_ctor})
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
    def walk_selector(node: YulNode):
        # TODO: store the selector
        if node.type == 'yul_case':
            # FIXME: flatten yul_literal->yul_number_literal->yul_hex_literal
            selector: str = node.children[0].children[0].children[0].children[0].obj
            case_body = node.children[1].children[0]
            assert case_body.is_fun_call(), f'got {node.children[0].type}'
            contract.metadata.external_fns[selector] = case_body.get_fun_name()

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
                size=type_dict['numberOfBytes'],
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
