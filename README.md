# pyul

a prototype front-end that translates Solidity's Yul to KSIR.

## setup

top level requirements:

- python v3.9.*
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

## running

after following [setup](#setup)

you can now run `pyul` as follows:

```
(pyul) $ poetry run pyul
```

as of now, this should just print `"hello, world"`

## development

see [notes.org](notes.org) for more.
