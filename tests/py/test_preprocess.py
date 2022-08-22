from pathlib import Path
import json
from pyul.ast import YulNode, walk_dfs
import subprocess

THIS_DIR = Path(__file__).parent


def test_prune_deploy_obj_metadata():
    p = THIS_DIR.parent / 'e2e' / 'SimpleAdd.sol'
    assert p.exists()
    output = subprocess.check_output(['pyul', p, '--stop-after', 'preprocess'],
                                     text=True)
    j = json.loads(output)
    assert 'metadata' in j
    assert 'main_ctor' in j['metadata']
    assert j['metadata']['main_ctor'].startswith('constructor_AdditionContract_')
