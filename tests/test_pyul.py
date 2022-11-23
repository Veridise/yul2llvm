import os
import subprocess
from pathlib import Path

THIS_DIR = Path(__file__).parent


def test_cli_version():
    subprocess.check_output(["pyul", "--version"])


def test_cli_compile_abspath():
    proc = subprocess.run(
        ["pyul", THIS_DIR / "e2e" / "SimpleAdd.sol", "--stop-after", "preprocess"],
        check=True,
    )
    assert proc.returncode == 0


def test_cli_compile_relpath():
    proc = subprocess.run(
        ["pyul", Path("e2e") / "SimpleAdd.sol", "--stop-after", "preprocess"],
        check=True,
        cwd=THIS_DIR,
    )
    assert proc.returncode == 0
