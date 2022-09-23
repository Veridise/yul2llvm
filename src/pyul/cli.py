"""cli driver to configure and run pyul"""

import argparse
import subprocess
import re
import antlr4
import sys
import logging
import shutil
import shlex
import tempfile
import json
from pathlib import Path
import dataclasses
from dataclasses import dataclass
from .utils.yul_parser import YulPrintListener
from .utils.yul_translator import YulTranslator
from .utils.YulAntlr import YulLexer, YulParser
from .core import inspect_json_ast
from .ast import ContractData
from . import preprocess
from typing import Dict, List
import importlib.metadata


@dataclass
class SolcOutput(object):
    contracts: Dict[str, Dict[str, ContractData]] = dataclasses.field(default_factory=dict)


def exit_error(msg: str):
    sys.exit('error: ' + msg)


def main():
    pipeline = ['compile', 'preprocess', 'translate']
    parser = argparse.ArgumentParser()
    parser.add_argument('--version', action='version',
                        version=f'{importlib.metadata.version("pyul")}')
    parser.add_argument('--stop-after', choices=list(pipeline), default='translate')
    parser.add_argument('--project-dir', type=Path, default=Path.cwd(),
                        help='Root directory containing all sources')
    parser.add_argument('-o', '--output-dir', help='Location to place artifacts (default: do not save)',
                        type=Path, default=None)
    parser.add_argument('input_file', type=Path, help='Input .sol file')
    parser.add_argument('-d', '--disable-module-verification',
                        action='store_true', default=False, help='Disable verification of generated llvm module')
    parser.add_argument('--log-level', choices=['debug', 'info', 'warning', 'error', 'critical'],
                        default='info', help='Log level to show in console output')
    
    args = parser.parse_args()

    # Validate arguments
    if not args.input_file.exists():
        exit_error(f'file does not exist: {args.input_file}')

    if args.output_dir is not None and args.output_dir.is_file():
        exit_error(f'file is not a directory: {args.output_dir}')

    # Create artifact directory if necessary
    if args.output_dir is None:
        _tmp_dir = tempfile.TemporaryDirectory()
        args.output_dir = Path(_tmp_dir.name)
    else:
        args.output_dir.mkdir(parents=True, exist_ok=True)

    # Begin frontend pipeline
    logging.basicConfig(filename=args.output_dir / 'pyul.log', filemode='w',
                        level=logging.DEBUG)
    logger = logging.getLogger('pyul')
    stream_handler = logging.StreamHandler()
    stream_handler.setLevel(args.log_level.upper())
    logger.addHandler(stream_handler)

    solc_output = solc_compile(logger, args.input_file, args.output_dir,
                               project_dir=args.project_dir)
    if solc_output is None:
        exit_error('failed to execute solc, aborting')

    if args.stop_after == 'compile':
        return

    the_contract = None
    for fname, contracts in solc_output.contracts.items():
        for name, contract in contracts.items():
            yul_json = preprocess_ir(logger, contract, contract.out_dir)
            contract.yul_ast = yul_json
            assert the_contract is None, "TODO: handle more than 1 contract"
            the_contract = contract

    preprocess.prune_deploy_obj(the_contract, logger=logger)
    preprocess.prune_deployed_code(the_contract, logger=logger)
    preprocess.attach_storage_layout(the_contract, logger=logger)
    preprocess.rewrite_map(the_contract, logger=logger)
    preprocess.rewrite_storage_ops(the_contract, logger=logger)

    # Save the processed Yul AST
    dumped_obj = the_contract.yul_ast.copy()
    dumped_obj['metadata'] = dataclasses.asdict(the_contract.metadata)
    yul_json_path = the_contract.out_dir / 'yul.json'
    logger.info(f'Dumping processed Yul AST json to {yul_json_path}')
    with open(yul_json_path, 'w') as f:
        json.dump(dumped_obj, f)

    if args.stop_after == 'preprocess':
        json.dump(dumped_obj, sys.stdout)
        return

    name = the_contract.name
    contract = the_contract
    logger.info(f'Summary of contract {name}:')
    logger.info('')
    (named_fns, all_defs, unknown) = inspect_json_ast(contract.out_dir / 'yul.json')

    named_fns = set(named_fns)
    all_defs = set(all_defs)
    unknown = set(unknown)
    logger.info('Functions defined in the Solidity source code:')
    for n in named_fns:
        logger.info('  ' + n)
    logger.info('Functions defined in the Yul IR:')
    for n in all_defs:
        logger.info('  ' + n)
    logger.info('Unknown function symbols:')
    for n in unknown:
        logger.info('  ' + n)
    logger.info('Dead symbols:')
    for n in all_defs.difference(unknown).difference(named_fns):
        logger.info('  ' + n)

    # TODO: move this into a translate() function
    yul2llvm_cpp_bin = shutil.which('yul2llvm_cpp')
    if yul2llvm_cpp_bin is None:
        logger.error('yul2llvm_cpp not found on PATH')
        sys.exit(1)

    yul2llvm_cpp_cmd = [yul2llvm_cpp_bin, str(contract.out_dir / 'yul.json')]
    if(args.disable_module_verification):
        yul2llvm_cpp_cmd.append('-d')
    logger.info(f'Running: {shlex.join(yul2llvm_cpp_cmd)}')
    proc = subprocess.run(yul2llvm_cpp_cmd,
                          capture_output=True, text=True)
    if proc.returncode != 0:
        logger.error('Failed to run yul2llvm_cpp:')
        for line in proc.stderr.splitlines():
            logger.error(line)
        logger.error('Aborting pyul!')
        sys.exit(1)

    print(proc.stdout)

    with open(contract.out_dir / 'llvm_ir.ll', 'w') as f:
        f.write(proc.stdout)


def preprocess_ir(logger, data: ContractData, out_dir: Path):
    '''Convert Yul IR into a PYul json file

    :param out_dir: The contract's artifact output directory.
    '''

    # TODO: do we need this? This seems specific to Eurus...
    logger.info(f'Running Yul IR pre-preprocess on {data.name}')
    t = YulTranslator()
    yul_str = t.translate(data.yul_text)

    with open(out_dir / 'ir_prepreprocess.yul', 'w') as f:
        f.write(yul_str)

    # TODO: this is from Eurus. Why do we need this?
    # step 1: make sure there's only one contract in one file (this file)
    nobj = re.findall(r"object \".*?\" \{", yul_str)
    if len(nobj) != 2:
        logger.warning('Yul input invalid. Contained more than one contract object.')

    # Run parser to get json representation.
    input_stream = antlr4.InputStream(yul_str)
    lexer = YulLexer.YulLexer(input_stream)
    stream = antlr4.CommonTokenStream(lexer)
    parser = YulParser.YulParser(stream)
    tree = parser.start()
    printer = YulPrintListener()
    printer.clear_built_string()
    walker = antlr4.ParseTreeWalker()
    walker.walk(printer, tree)

    printer.strip_all_trailing()

    yul_json = printer.built_string
    yul_json_path = out_dir / 'input_yul.json'
    logger.info(f'Dumping input Yul IR json to {yul_json_path}')
    with open(yul_json_path, 'w') as f:
        f.write(yul_json)

    return json.loads(yul_json)


def solc_compile(logger, src_path: Path, artifact_dir: Path,
                 project_dir: Path = None):
    '''Invoke solc'''

    assert project_dir is not None

    solc_bin = shutil.which('solc')
    if solc_bin is None:
        logger.error('solc not found on path')
        return None
    logger.info(f'Using solc at {solc_bin}')

    src_rel_path = src_path.absolute().relative_to(project_dir.absolute())
    # To avoid "escaping" the artifact dir, check the path
    assert artifact_dir in (artifact_dir / src_rel_path).parents

    solc_opts = {
        'language': 'Solidity',
        'sources': {
            str(src_rel_path): {
                'urls': [str(src_path.absolute())],
            }
        },
        'settings': {
            'outputSelection': {
                '*': {
                    '*': ['ir', 'abi', 'storageLayout']
                }
            },
            'viaIR': True
        }
    }
    solc_input_path = artifact_dir / 'solc_input.json'
    with open(solc_input_path, 'w') as f:
        json.dump(solc_opts, f)

    cmd = [solc_bin, '--standard-json', str(solc_input_path)]
    cmd.extend(['--allow-paths', str(project_dir)])

    logger.info(f'Executing: {shlex.join(cmd)}')
    solc_proc = subprocess.run(cmd, text=True, capture_output=True)
    if solc_proc.returncode != 0:
        logger.error(f'failed to execute command: ${shlex.join(cmd)}')
        return None

    # Save the output for debugging
    with open(artifact_dir / 'solc_output.json', 'w') as f:
        f.write(solc_proc.stdout)

    solc_output = json.loads(solc_proc.stdout)

    # Abort if there were errors
    for diagnostic in solc_output.get('errors', []):
        if diagnostic['severity'] == 'error':
            logger.error('solc returned an error:')
            for line in diagnostic['formattedMessage'].splitlines():
                logger.error(line)
            return None

    # Save abi, storage layout, and Yul IR for debugging
    # This uses a layout similar to Hardhat's:
    # https://hardhat.org/hardhat-runner/docs/advanced/artifacts
    # TODO: can we just read Hardhat's directory structure?
    output = SolcOutput()
    for src_p, src_contents in solc_output['contracts'].items():
        src_out_dir = artifact_dir / src_p
        src_out_dir.mkdir(parents=True, exist_ok=True)
        for name, contract in src_contents.items():
            c_dir = src_out_dir / name
            c_dir.mkdir(exist_ok=True)

            with open(c_dir / 'abi.json', 'w') as f:
                json.dump(contract['abi'], f)

            with open(c_dir / 'ir.yul', 'w') as f:
                f.write(contract['ir'])

            with open(c_dir / 'storageLayout.json', 'w') as f:
                json.dump(contract['storageLayout'], f)

            if src_p not in output.contracts:
                output.contracts[src_p] = {}
            output.contracts[src_p][name] = ContractData(
                name=name,
                abi=contract['abi'],
                storageLayout=contract['storageLayout'],
                yul_text=contract['ir'],
                out_dir=c_dir,
            )

    return output


if __name__ == '__main__':
    main()
