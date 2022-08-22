from .ast import ContractData, YulNode, walk_dfs
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
