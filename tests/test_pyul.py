from pathlib import Path
import subprocess

THIS_DIR = Path(__file__).parent


# @ejmg 
# TODO: as of now, this test using the local filesystem to both access the test file and to write.
# TODO: this command naively assumes solc is available in the path without checking.
def test_cli_compile():
    proc = subprocess.run(["pyul", "compile", THIS_DIR.parent / "corpus" / "SimpleAdd.sol"], check=True)
    assert proc.returncode == 0
