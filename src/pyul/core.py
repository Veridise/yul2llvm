"""functions for extracting information from Yul AST JSON"""
from dataclasses import dataclass, field
import json
import re


YUL_FN_REGX = [
    # private function
    "^fun__",
    # normal function
    "^fun_",
    # contract constructor
    "^constructor_*",
    # TODO: PAT's for other helper functions provided by Yul and that we care about
]

FN_REGX = "(" + ")|(".join(YUL_FN_REGX) + ")"

@dataclass
class AstMeta:
    """struct that holds AST data during inspection."""
    CONTRACT_FNS : list = field(default_factory=list)
    FUNCTION_DEFS : dict = field(default_factory=dict)
    UNKNOWN_SYMBOLS : dict = field(default_factory=dict)

def generate_output(ast):
    """generates output in simpler form for CLI"""
    fn_names = []
    all_defs = []
    unknown_names = []
    for fn in ast.CONTRACT_FNS:
        fn_names.append(get_name(fn))
    for known in ast.FUNCTION_DEFS.keys():
        all_defs.append(known)
    for unknown in ast.UNKNOWN_SYMBOLS.keys():
        unknown_names.append(unknown)

    return (fn_names, all_defs, unknown_names)


def get_name(_obj):
    """grab the identifier name for a Yul node"""
    return _obj["children"][0]["children"][0]

def walk_json(json_arr, parent_ctx, ast):
    """Walk down the AST """
    for _obj in json_arr:
        t = _obj["type"]
        if t == "yul_function_definition":
            fn_name = get_name(_obj)

            # filter for the contract functions we support
            if re.match(FN_REGX, fn_name):
                ast.CONTRACT_FNS.append(_obj)

            ast.FUNCTION_DEFS[fn_name] = parent_ctx
            walk_json(_obj["children"], fn_name, ast)
        elif t == "yul_function_call":
            called_name = get_name(_obj)
            # lazy shortcut to avoid checking later for contract functions with defs that haven't been seen yet
            if called_name not in ast.CONTRACT_FNS and not re.match(FN_REGX, called_name):
                ast.UNKNOWN_SYMBOLS[called_name] = parent_ctx
            walk_json(_obj["children"], called_name, ast)
        elif t == "yul_literal" or t == "yul_identifier":
            pass
        elif _obj.get("children") and len(_obj["children"]) > 0:
            walk_json(_obj["children"], parent_ctx, ast)

def inspect_json_ast(_file):

    ast = AstMeta()

    with open(_file, "r") as f:
        _outer_json = json.load(f)
        # TODO: remove the outer layer of the AST. useless indirection.
        yul_json = _outer_json["yul_ast"]

    # top most level of our JSON:
    # { ..., "object_name":
    #     {..., "children": ["$contract_name"]},
    #     "contract_body": 
    #         {"type": "yul_code", "children":
    #             [{"type": "yul_block", "children": [...]}]},
    #         "object_body": 
    #             { ..., contract_body: { ..., "children": [...] }}}
    contract_name = yul_json["object_name"]["children"][0]
    contract_code = yul_json["contract_body"]["children"][0]["children"]

    deployed_name = yul_json["object_body"]["object_name"]["children"][0]
    deployed_code = yul_json["object_body"]["contract_body"]["children"][0]["children"]

    walk_json(contract_code, contract_name, ast)
    walk_json(deployed_code, deployed_name, ast)

    return generate_output(ast)