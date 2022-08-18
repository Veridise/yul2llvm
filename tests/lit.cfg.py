#!/usr/bin/env python3
# Modified version of
# https://raw.githubusercontent.com/llvm/llvm-project/main/mlir/examples/standalone/test/lit.cfg.py
# from LLVM, which is licensed under Apache 2.0 with LLVM Exceptions.

# -*- Python -*-

import os
import platform
import re
import subprocess
import tempfile

import lit.formats
import lit.util

from lit.llvm import llvm_config
from lit.llvm.subst import ToolSubst
from lit.llvm.subst import FindTool

# Configuration file for the 'lit' test runner.

# TODO: this needs to support:
# 1. running Python tests only
# 2. running C++ tests only
# 3. running end-to-end tests

assert hasattr(config, 'llvm_tools_dir')
assert hasattr(config, 'cpp_bin_dir')
assert hasattr(config, 'extra_suffixes')
assert hasattr(config, 'test_exec_root')
assert hasattr(config, 'test_source_root')


# name: The name of this test suite.
config.name = 'yul2llvm'

config.test_format = lit.formats.ShTest(not llvm_config.use_lit_shell)

# suffixes: A list of file extensions to treat as test files.
config.suffixes = ['.sol', '.yul', '.json']
config.suffixes.extend(config.extra_suffixes)

# test_source_root: The root path where tests are located.

# excludes: A list of directories to exclude from the testsuite. The 'Inputs'
# subdirectories contain auxiliary inputs for various tests in their parent
# directories.
config.excludes = ['Inputs', 'CMakeLists.txt', 'README.txt', 'LICENSE.txt']


# Set up environment variables

llvm_config.with_system_environment(
    ['HOME', 'INCLUDE', 'LIB', 'TMP', 'TEMP'])

llvm_config.use_default_substitutions()
llvm_config.with_environment('PATH', config.cpp_bin_dir, append_path=True)
llvm_config.with_environment('PATH', config.llvm_tools_dir, append_path=True)

tool_dirs = [config.cpp_bin_dir]
tools = [
    "yul2llvm_cpp"
]

# Set up variable substitutions
llvm_config.add_tool_substitutions(tools, tool_dirs)
config.substitutions.append(('%PATH%', config.environment['PATH']))
