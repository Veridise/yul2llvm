from pyul.cli import compile
from click.testing import CliRunner

import pyul

# @ejmg 
# TODO: as of now, this test using the local filesystem to both access the test file and to write.
# TODO: this command naively assumes solc is available in the path without checking.
def test_cli_compile():
    runner = CliRunner()
    result = runner.invoke(compile, ["corpus/SimpleAdd.sol"])
    assert result.exit_code == 0