from pathlib import Path
import subprocess
from pyul import core

# TODO: need to add some setup/teardown functions for things like compiled resources
THIS_DIR = Path(__file__).parent


def test_inspect_json_ast_simple():
    p = THIS_DIR.parent / "compiled" / "SimpleAdd" / "SimpleAdd.json"

    (named_fns, all_defs, unknown) = core.inspect_json_ast(p)

    assert len(named_fns) == 2
    assert len(all_defs) == 18
    assert len(unknown) == 33

def test_inspect_json_ast_erc20():
    p = THIS_DIR.parent / "compiled" / "ERC20" / "ERC20.json"

    (named_fns, all_defs, unknown) = core.inspect_json_ast(p)

    assert len(named_fns) == 18
    assert len(all_defs) == 149
    assert len(unknown) == 160
    