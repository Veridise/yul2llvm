"""cli driver to configure and run pyul"""

import click

from subprocess import Popen, PIPE


def compile(f):
    """compile solc file to yul"""
    click.echo(f"Compiling {f} to yul...")
    res = Popen(["solc", "--ir", f], stdout=PIPE, stdin=PIPE)

    stdin, stdout = res.communicate()

    yul_obj = bytes.decode(stdin, "utf-8")
    click.echo("RESULTS")
    click.echo("==================================================")
    click.echo(yul_obj)

@click.command()
@click.argument("_file", metavar="FILE")
@click.option("--input-type", "-iT", default="sol", type=str)
@click.option("--input-type", "-iT", default="sol", type=str)
def run(_file, input_type="sol"):
    """cli command dispatcher for pyul"""
    if input_type == "sol":
        compile(_file)
    else:
        click.echo(".yul file processing not yet supported.")
