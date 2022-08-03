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

## Running

after following [setup](#setup)

you can now run `pyul` as follows:

```
(pyul) $ poetry run pyul
```

as of now, this should just print `"hello, world"`

## development

see [notes.org](notes.org) for more.
