from pathlib import Path
import json
from pyul.ast import YulNode, walk_dfs

# TODO: need to add some setup/teardown functions for things like compiled resources
THIS_DIR = Path(__file__).parent


def test_walk_dfs():
    # File generated from Yul json using:
    #   jq '.["yul_ast"]["object_body"]["contract_body"]["children"][0]' -c
    p = THIS_DIR / 'Inputs' / 'SimpleAdd_deployedbody.json'
    assert p.exists()

    with open(p, 'r') as f:
        block_node = YulNode(json.load(f))

    fun_names = []

    def visit_defs(node: YulNode) -> bool:
        if node.is_fun_def():
            fun_names.append(node.obj['children'][0]['children'][0])
            return False
        return True

    walk_dfs(block_node, visit_defs)
    assert len(fun_names) == 17
    assert 'fun_add_21' in fun_names
