"""cli driver to configure and run pyul"""

from random import randint
import click

import subprocess
import shutil
import os
import pathlib
import re
import antlr4
import json
from utils.yul_parser import YulPrintListener, strip_all_trailing, make_json_arr
from utils.yul_translator import YulTranslator
from utils.YulAntlr import YulLexer, YulParser


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
@click.option("--verbose", "-V", is_flag=True, default=False, help="Enable logging of results to stdout while running pyul.")
@click.option("--pipeline", "-P", is_flag=True, default=False, help="Run translate and then parse on the output automatically.", )
@click.pass_context
def translate(ctx, _file, verbose, pipeline):
    """transforms solc generated Yul code into more easily processed form. """
    with open(_file, "r") as f:
        raw_yul = f.read()

    t = YulTranslator()
    yul_str = t.translate(raw_yul)

    new_file = _file + ".tmp"

    with open(new_file, "w") as f:
        f.write(yul_str)

    if verbose:
        click.echo(yul_str)

    if pipeline:
        click.echo(f"parsing flag pass, now parsing: {new_file}")
        ctx.invoke(parse, _file=new_file)

@run.command()
@click.argument("_file", metavar="YUL_FILE")
@click.option("--verbose", "-V", is_flag=True, help="Enable logging of results to stdout while running pyul.")
@click.option("--dry-run", "-D", is_flag=True, help="Don't write results to file.")
def parse(_file, verbose, dry_run):
    """parse a Yul file into its AST representation"""
    input_stream = antlr4.FileStream(_file)

    # step 1: make sure there's only one contract in one file (this file)
    with open(_file, "r") as f:
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
        with open(_file + '.json', "w") as f:
            f.write(parsed_yul + "\n")

    if verbose:
        # TODO: this is very messy. How I pipe this data to stdout needs to be cleaned up either with
        # additional flags or by unifying output into a clean JSON format that keeps everything separate
        # or a similarly minded approach. This will apply regardless of how we ultimately extract information
        # such as the symbol table and extracted functions.
        click.echo("Parsed Yul\n")
        click.echo(parsed_yul)
        click.echo("\nKnown Symbols")
        click.echo(printer.known_symbols)
        click.echo("\nUnknown Symbols\n")
        click.echo(printer.unknown_symbols.items())
        click.echo("\nExtracted Functions\n")
        click.echo(make_json_arr(printer.extracted_fn))

@run.command()
@click.argument("_file", metavar="FILE")
@click.option("--input-type", "-iT", default="sol", type=str)
@click.option("--output", "-o", default="compiled", type=str, help="output directory")
def compile(_file, input_type, output):
    """compile sol file to yul"""
    if input_type != "sol":
        click.echo(".yul files are not supported yet.")
        return

    click.echo(f"Compiling {_file} to yul...")

    # get the output directories setup
    cwd = os.getcwd()
    click.echo(f"cwd: {cwd}")
    file_name = pathlib.Path(_file).stem
    click.echo(f"file name: {file_name}")

    compile_dir = os.path.join(cwd, output)

    result_id = randint(100, 100000)
    result_dir = os.path.join(compile_dir, file_name + "_" + str(result_id))
    result_yul = os.path.join(result_dir, file_name + ".yul")
    result_storage = os.path.join(result_dir, file_name + "_storage")
    result_abi = os.path.join(result_dir, file_name + "_abi")
    click.echo(f"output dir: {compile_dir}/")
    click.echo(f"result dir: {result_dir}/")

    if not os.path.exists(compile_dir):
        os.mkdir(compile_dir)
    if not os.path.exists(result_dir):
        os.mkdir(result_dir)
    else:
        # wary of deleting output
        result_dir = os.path.join(result_dir, str(randint(1,100)))
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

    click.echo("writing results to file...")
    with open(result_yul, "w") as f:
        f.writelines(yul_obj)
        click.echo("yul successfully written")
    with open(result_storage, "w") as f:
        f.writelines(storage_layout)
        click.echo("storage layout successfully written")
    with open(result_abi, "w") as f:
        f.writelines(abi_spec)
        click.echo("abi specification successfully written")

    click.echo("`pyul compile` finished running.")
