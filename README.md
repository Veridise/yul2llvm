# yul2llvm

A prototype front-end that translates Solidity's Yul IR to LLVM.

This project is broken into two parts:

* `pyul` (Preprocessed Yul): a Python program that takes `.sol` files as input
  and generates an LLVM IR representation as output.
* `yul2llvm_cpp`: a C++ program that is invoked by `pyul` that takes a Yul
  dialect called PYul in JSON format and translates it to LLVM IR.

## Running pyul

### Installing as a user

The easiest way to run `pyul` is through Nix:

```sh
nix run 'git+ssh://git@github.com/Veridise/yul2llvm?ref=main#yul2llvm'
```

Otherwise: 1) you will have to compile the `yul2llvm_cpp` from source and put it
on PATH; 2) you will have to install `pyul` (e.g., by running `pip install`
locally or with this repository URL).

### Usage

`pyul` takes a source file as an argument and prints out a string representation
of the final output to stdout:

```sh
pyul test/e2e/SimpleAdd.sol
```

* See `pyul -h` for all help options.
* `pyul` saves intermediate artifacts such as solc output and Yul IR in a
  so-called artifact directory.
  By default, this directory is a temporary folder (i.e., deleted after `pyul`
  finishes).
  It may be convenient to save this folder by specifying a concrete location
  with `-o`.
* In some cases, the `--project-dir` option must be provided, because `solc`
  expects all source file locations to be provided as relative paths.
  * By default, the `--project-dir` is set to the current working directory.
  * All source files must be contained in the project directory.
  * `pyul` will automatically convert absolute paths into paths relative to the
    `--project-dir` setting.

### Artifacts

The artifact directory has the following layout:

```plain
/pyul.log                     - log file of pyul
/solc_input.json              - input command provided to solc --standard-json
/solc_output.json             - JSON output produced by solc
/relative/path/to/file.sol/   - folder corresponding to each source file
  ContractName/               - each contract in each source file has a sub folder
    abi.json                  - abi info output by solc
    storageLayout.json        - storage layout info output by solc
    ir.yul                    - textual Yul IR output by solc
    ir_prepreprocess.yul      - textual Yul IR after some cleanup
    input_yul.json            - prepreprocessed Yul IR in JSON format
    yul.json                  - final preprocessed Yul IR in JSON format
```

### Preprocessing

`pyul` will perform the following preprocessing.

#### Deploy code

* All top-level statements that are not function definitions
  will be deleted. Functions not reachable from the constructor will be deleted.

#### Deployed code

* The memory initialization code at the beginning will be removed.
* The selector code will be moved into a special top-level function named
  `_pyul_selector`.

### Metadata

The top-level JSON object created by `pyul` will contain a `"metadata"` object
with the following schema:

```javascript
{
  // The name of the main constructor function.
  "main_ctor": "constructor_AdditionContract_21",

  // The list of the functions (names) that are part of the contract's ABI
  "external_funs": [],
}
```

## Development Setup

Two methods: Nix (recommended for end-to-end and C++ component) and manual
(easier for Python component only).

### Nix setup

This project is configured with Nix flakes, so you can enter a developer shell
with:

```
nix develop .
```

The shell will:
* Automatically create a Python virtual env and install the Python component in
  editable mode
* Bring all the C++ component dependencies into scope

Once in the shell:
* To configure the C++ CMake project, run: `phases=configurePhase genericBuild`
* In the build folder, build with `cmake --build .`
* Run C++ only tests with `cmake --build . --target check`

### Manual setup

#### Python component

Top level requirements:

- Python >= 3.8
- pip

Inside of a virtual environment, install in editable mode with:

```sh
pip install -e '.[dev]'
```

The development environment can also be set up with `poetry` as follows:

```sh
poetry install --extras dev
```

* Run tests with `pytest`
* Run mypy with `mypy src --package-root src`

#### C++ component

Dependencies:
* LLVM 13
* nlohmann json
* (Optional) GoogleTest (for unit tests)

Steps: TODO (this will be very involved)

## Building C++ component
To build

```
<yul2llvm-root>$ phases=configurePhase genericBuild
# This will automatically create and change to the directory to build directory
```
To run
```
$ <yul2llvm-root>/build$ /bin/yul2llvm_cpp <inputFile> -o <outputFile>
```
Where,
* inputFile: The file containing the AST in json format
* outputFIle: The generated llvm code

## Testing

Testing for this project takes place in three locations:
* pytest unit tests for Python
* lit tests for C++
* lit tests for end-to-end

### Running lit tests

One of the main test suites is the lit test suite located in `tests/`. This is
broken up into two folders:

* `tests/e2e`: end-to-end tests from `pyul` and `solc` to LLVM IR.
* `tests/cpp`: lit tests for exclusively testing the C++ component. The C++
  `check-lit` target only executes the tests in this folder.

To run the lit tests, start the `nix develop` shell and run:

```bash
lit ./tests -v

# If you need to debug test failures, set the environment variable Y2L_TEST_RUN
# to a path to save the test artifacts.
# NOTE: the provided directory must exist!
Y2L_TEST_RUN=/tmp/dir/to/save/artifacts lit ./tests -v
```

### Writing lit tests

Resources:
* [LLVM lit](https://llvm.org/docs/CommandGuide/lit.html)
* [LLVM FileCheck](https://llvm.org/docs/CommandGuide/FileCheck.html)
* [LLVM Testing Infrastructure](https://llvm.org/docs/TestingGuide.html#writing-new-regression-tests)
  (note: this guide is specific to the LLVM project, but much of the information
  is relevant to lit use cases outside of LLVM).

To write end-to-end tests, run `pyul` as follows:

```
// RUN: pyul %s -o %t --project-dir %S | FileCheck %s
```

This instructs `pyul` to:
* take the current file as input
* save the artifacts in the temporary location corresponding to the file
  (typically `${filename}.tmp`)
* resolve source paths relative to the current file's directory, so as to enable should
  enable support for `import` statements (this needs to be checked, however)

## Notes

### Helpful jq commands

List all function definitions:
```
jq '[.["object_body"]["contract_body"]["children"][0]["children"] | .[] | select(.["type"] == "yul_function_definition")]'
```

(Note: this is because the nested is `start / contract / deployed / body / block`)
