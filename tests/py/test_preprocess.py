import json
import subprocess
from pathlib import Path

from pyul.ast import YulNode, walk_dfs

THIS_DIR = Path(__file__).parent


def test_prune_deploy_obj_metadata():
    p = THIS_DIR.parent / "e2e" / "SimpleAdd.sol"
    assert p.exists()
    output = subprocess.check_output(
        ["pyul", p, "--stop-after", "preprocess"], text=True
    )
    j = json.loads(output)
    assert "metadata" in j
    assert "main_ctor" in j["metadata"]
    assert j["metadata"]["main_ctor"].startswith("constructor_AdditionContract_")


def test_prune_deployed_code():
    p = THIS_DIR.parent / "e2e" / "ERC20.sol"
    assert p.exists()
    output = subprocess.check_output(
        ["pyul", p, "--stop-after", "preprocess"], text=True
    )
    j = json.loads(output)
    blk = j["object_body"]["contract_body"]["children"][0]["children"]

    for child in blk:
        assert (
            child["type"] == "yul_function_definition"
        ), f'All children should be fun defs, but found {child["type"]}'

    assert len(j["metadata"]["external_fns"]) > 0
