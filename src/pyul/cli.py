"""cli driver to configure and run pyul"""

from random import randint, randrange
import click

from subprocess import Popen, PIPE
import shutil
import os
import pathlib
import re
import antlr4
import json
from utils.yul_parser import YulPrintListener
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
        click.echo("no solc found in path.")
        prompt = click.prompt("install solc? yes/[N]o", default=False, show_default=False)
        if prompt:
            click.echo("using solc-select to install solidity...")
            res = Popen(["solc-select", "install", "0.8.11"], stdout=PIPE, stdin=PIPE, text="utf-8")
            stdin, stdout = res.communicate()
            click.echo(f"{stdin}")

@run.command()
@click.argument("_file", metavar="FILE")
def translate(_file):
    """transforms solc generated Yul code into more easily processed form. """
    with open(_file, "r") as f:
        raw_yul = f.read()
    
    t = YulTranslator()
    yul_str = t.translate(raw_yul)

    new_file = _file + ".tmp"
    with open(new_file, "w") as f:
        f.write(yul_str)

    print(yul_str)

@run.command()
@click.argument("_file", metavar="YUL_FILE")
def parse(_file):
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

    # anyway to remove this call to eval? it's called parse don't validate sweaty
    parsed_yul = eval(printer.built_string.strip(","))

    print(parsed_yul)

    with open(_file + '.json', "w") as f:
        json.dump(parsed_yul, f, indent="  ")



@run.command()
@click.argument("_file", metavar="FILE")
@click.option("--input-type", "-iT", default="sol", type=str)
@click.option("--output", "-o", default="compiled", type=str, help="output directory")
def compile(_file, input_type, output):
    """compile solc file to yul"""
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
    click.echo(f"output dir: {compile_dir}")
    click.echo(f"result dir: {result_dir}")

    if not os.path.exists(compile_dir):
        os.mkdir(compile_dir)
    if not os.path.exists(result_dir):
        os.mkdir(result_dir)
    else:
        # wary of deleting output
        result_dir = os.path.join(result_dir, str(randint(1,100)))
        os.mkdir(result_dir)

    # generate the yul
    ir_res = Popen(["solc", "--ir", _file], stdout=PIPE, stdin=PIPE, text="utf-8")
    # @ejmg TODO: need to figure out error handling given that solc doesn't exit with an error code in many instances that would be considered an error by users
    ir_stdin, ir_stdout = ir_res.communicate()
    # trim off the heading included by solc in the Yul output
    yul_obj = ir_stdin.split("\n", 2)[2]
    storage_res = Popen(["solc", "--storage-layout", _file], stdout=PIPE, stdin=PIPE, text="utf-8")
    store_stdin, store_stdout = storage_res.communicate()
    storage_layout = store_stdin.split("\n", 3)[3]

    click.echo("writing results to file...")
    with open(result_yul, "w") as f:
      f.writelines(yul_obj)
    with open(result_storage, "w") as _fs:
        _fs.writelines(storage_layout)

    click.echo("yul object and storage data successfully written!")
