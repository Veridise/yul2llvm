"""cli driver to configure and run pyul"""

from operator import contains
from random import randint
import click

import subprocess
import shutil
import os
import pathlib
import re
import antlr4
from .utils.yul_parser import YulPrintListener
from .utils.yul_translator import YulTranslator
from .utils.YulAntlr import YulLexer, YulParser
from .core import inspect_json_ast


@click.group()
def run():
    """cli command dispatcher for pyul"""
    pass

@run.command()
def init():
    """setup pyul for use"""
    click.echo("checking for solc on path...")
    res = shutil.which("solc")
    if res is None:
        click.echo(f"solc found at path: {res}")
    else:
        # TODO: consider just curl'ing solidity instead of using solc-select. Would require path manipulation and permissions.
        click.echo("no solc found in path.")
        prompt = click.prompt("install solc? yes/[N]o", default=False, show_default=False)
        if prompt:
            click.echo("using solc-select to install solidity...")
            # versions of solc must be 0.8.14 or later (see Eurus README)
            proc = subprocess.run(["solc-select", "install", "0.8.15"], capture_output=True, encoding="utf-8")
            if proc.returncode != 0:
                click.echo("Error downloading solc-select and solidity.")
            else:
                click.echo(f"{proc.stdout}")

@run.command()
@click.argument("_file", metavar="FILE")
@click.option("--all-fns", is_flag=True, help="Emit all function defs")
@click.option("--contract-fns", is_flag=True, help="Emit contract functions")
@click.option("--unknown-names", is_flag=True, help="Emit unknown symbols")
def inspect_ast(_file, all_fns, contract_fns, unknown_names):
    """Extract program structure from a Yul AST JSON file. Prints all info by default."""
    (named_fns, all_defs, unknown) = inspect_json_ast(_file)
    if all_fns:
        click.echo("\nAll function definitions:\n")
        for fn in all_defs: click.echo(fn)
    elif contract_fns:
        click.echo("\nAll contract functions:\n")
        for fn in named_fns: click.echo(fn)
    elif unknown_names:
        click.echo("\nAll unknown symbols and function calls:\n")
        for fn in unknown: click.echo(fn)
    else:
        click.echo("\nAll function definitions:\n")
        for fn in all_defs: click.echo(fn)
        click.echo("\nAll contract functions:\n")
        for fn in named_fns: click.echo(fn)
        click.echo("\nAll unknown symbols and function calls:\n")
        for fn in unknown: click.echo(fn)

@run.command()
@click.argument("_file", metavar="YUL_FILE")
@click.option("--verbose", "-V", is_flag=True, help="Enable logging of results to stdout while running pyul.")
@click.option("--dry-run", "-D", is_flag=True, help="Don't write results to file.")
def parse(_file, verbose, dry_run):
    """parse a Yul file into its AST representation"""

    with open(_file, "r") as f:
        raw_yul = f.read()

    t = YulTranslator()
    yul_str = t.translate(raw_yul)

    file_prefix = os.path.splitext(_file)[0]
    tmp_file = _file + ".tmp"

    with open(tmp_file, "w") as f:
        f.write(yul_str)

    if verbose:
        click.echo(yul_str)

    input_stream = antlr4.FileStream(tmp_file)

    # step 1: make sure there's only one contract in one file (this file)
    with open(tmp_file, "r") as f:
        tmp_yul_str = f.read()
    nobj = re.findall(r"object \".*?\" \{", tmp_yul_str)
    if len(nobj) != 2:
        click.echo("Yul input invalid. Contained more than one contract object. ")

    lexer = YulLexer.YulLexer(input_stream)
    stream = antlr4.CommonTokenStream(lexer)
    parser = YulParser.YulParser(stream)
    tree = parser.start()
    printer = YulPrintListener()
    printer.clear_built_string()
    walker = antlr4.ParseTreeWalker()
    walker.walk(printer, tree)

    printer.strip_all_trailing()
    parsed_yul = printer.built_string

    if not dry_run:
        with open(file_prefix + '.json', "w") as f:
            f.write(parsed_yul + "\n")

    if verbose:
        click.echo(parsed_yul)



@run.command()
@click.argument("_file", metavar="FILE")
@click.option("--verbose", "-V", is_flag=True, help="Enable logging of results to stdout while running pyul.")
@click.option("--output", "-o", default="compiled", type=str, help="output directory")
def compile(_file, verbose, output):
    """compile sol file to yul"""

    if verbose: click.echo(f"Compiling {_file} to yul...")

    # get the output directories setup
    cwd = os.getcwd()
    if verbose: click.echo(f"cwd: {cwd}")
    file_name = pathlib.Path(_file).stem
    if verbose: click.echo(f"file name: {file_name}")

    compile_dir = os.path.join(cwd, output)

    result_dir = os.path.join(compile_dir, file_name)
    result_yul = os.path.join(result_dir, file_name + ".yul")
    result_storage = os.path.join(result_dir, file_name + "_storage")
    result_abi = os.path.join(result_dir, file_name + "_abi")
    if verbose: click.echo(f"output dir: {compile_dir}/")
    if verbose: click.echo(f"result dir: {result_dir}/")

    if not os.path.exists(compile_dir):
        os.mkdir(compile_dir)
    if not os.path.exists(result_dir):
        os.mkdir(result_dir)

    # generate the yul
    ir_res = subprocess.run(["solc", "--ir", _file], encoding="utf-8", capture_output=True)
    # @ejmg TODO: need to figure out error handling given that solc doesn't exit with an error code in many instances that would be considered an error by users
    # ir_res.returncode gives us the mechanism to check in general, however. how to gracefully exit is another choice to make later.
    ir_output = ir_res.stdout
    # trim off the heading included by solc in the Yul output
    yul_obj = ir_output.split("\n", 2)[2]
    # grab storage layout for contract
    storage_res = subprocess.run(["solc", "--storage-layout", _file], encoding="utf-8", capture_output=True)
    storage_output = storage_res.stdout
    storage_layout = storage_output.split("\n", 3)[3]
    # grab the abi specification
    abi_res = subprocess.run(["solc", "--abi", _file], encoding="utf-8", capture_output=True)
    abi_output = abi_res.stdout
    # trim
    abi_spec = abi_output.split("\n", 3)[3]

    if verbose: click.echo("writing results to file...")
    with open(result_yul, "w") as f:
        f.writelines(yul_obj)
        if verbose: click.echo("yul successfully written")
    with open(result_storage, "w") as f:
        f.writelines(storage_layout)
        if verbose: click.echo("storage layout successfully written")
    with open(result_abi, "w") as f:
        f.writelines(abi_spec)
        if verbose: click.echo("abi specification successfully written")

    if verbose: click.echo("`pyul compile` finished running.")


@run.command()
@click.argument("_f", metavar="FILE")
@click.option("--verbose", "-V", is_flag=True, help="Enable logging of results to stdout while running pyul.")
@click.option("--dry-run", "-D", is_flag=True, help="Don't write results to file.")
@click.option("--output", "-o", default="compiled", type=str, help="output directory")
@click.pass_context
def all(ctx, _f, verbose, dry_run, output):
    """runs all pyul commands in sequence for a given .sol file. """
    # Grabbing path info we'd otherwise get from compile.
    cwd = os.getcwd()
    file_name = pathlib.Path(_f).stem
    compile_dir = os.path.join(cwd, output)
    result_dir = os.path.join(compile_dir, file_name)
    yul_file = os.path.join(result_dir, file_name + ".yul")
    json_file = os.path.join(result_dir, file_name + ".json")

    if verbose: click.echo("Running `pyul compile`")
    ctx.invoke(compile, _file=_f, verbose=verbose, output=output)

    if verbose: click.echo("Running `pyul parse`")
    ctx.invoke(parse, _file=yul_file, verbose=verbose, dry_run=dry_run)

    if verbose: click.echo("Running `pyul inspect_ast`")
    ctx.invoke(inspect_ast, _file=json_file)
