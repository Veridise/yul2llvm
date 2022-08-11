# yul2llvm

A prototype front-end that translates Solidity's Yul IR to LLVM.

## Setup

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

- Python >= 3.9
- pip
- a virtual env setup of your choice

with these, activate your virtual env (let's call it `pyul`) and install `poetry`.

```
(pyul) $ pip install -U pip poetry # this updates pip in addition to installing poetry
```

if you made as far as acquiring pip and your own virtual env setup, the above command should work. if it doesn't, try
the same command but with `--user` in addition to `-U`.

after successfully installing poetry, install the project:

```
(pyul) $ poetry install
```

#### C++ component

Dependencies:
* LLVM 13
* nlohmann json
* (Optional) GoogleTest (for unit tests)

Steps: TODO (this will be very involved)

## Building C++ component
To build

```
$ mkdir <build-dir>
$ cd <build-dir>
$ cmake <path-of-source-root>
```
To run
```
$ <build-dir>/bin/yul2llvm_cpp <inputFile> -o <outputFile>
```
Where,
* inputFile: The file containing the AST in json format
* outputFIle: The generated llvm code

## Running


after following [setup](#setup)

you can now run `pyul` as follows:

```
(pyul) $ pyul --help
```

this should give you a print out of the existing commands and their descriptions.

### commands

- **compile**: compiles a `.sol` file into its Yul IR equivalent along with its storage layout and ABI files.
- **translate**: runs some preprocessing on a Yul file to make it easier to parse
- **parse**: parses a Yul file (pre-processed) and generates the corresponding JSON AST.
- **init**: helps setup a dev environment for handling Solidity files. WIP.

### processing file example

For some Solidity contract, let's say [`SimpleAdd.sol`](./corpus/SimpleAdd.sol), we can compile the file as follows:

```
(pyul) $ pyul compile corpus/SimpleAdd.sol
```
by default, this will generate a directory path of `compiled/SimpleAdd_${RANDOM_INT}/` where `SimpleAdd.yul` will be found. Run the following to perform some preprocessing of our new Yul file before generating our final JSON. For the sake of simplicity, assume our path is `compiled/SimpleAdd_1000/`.

```
(pyul) $ pyul translate -P compiled/SimpleAdd_1000/SimpleAdd.yul
```

This will do two things for us:
1. Preprocess our Yul file in preparation of it's parsing into a JSON AST.
2. the `-P` flag automatically parses our file for us and generates the aforementioned AST.

You will now find two new files under `compiled/SimpleAdd_1000/`, `SimpleAdd.yul.tmp` and `SimpleAdd.yul.tmp.json` which contain the processed Yul and the respective JSON AST.

## development

### testing pyul

As of now, it is assumed that solc is installed on your path and with the correct permissions. Please either install solc manually or try `pyul init`. After that, try:
```
(pyul) $  poetry run pytest
```

This will test the `pyul compile` command on your system as shown above.

#### more

see [notes.org](notes.org) for more on pyul.
