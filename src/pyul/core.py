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
    contract_fns : list = field(default_factory=list)
    function_defs : dict = field(default_factory=dict)
    unknown_symbols : dict = field(default_factory=dict)

def generate_output(ast):
    """generates output in simpler form for CLI"""

    return (list(map(get_name, ast.contract_fns)), 
            list(ast.function_defs),
            list(ast.unknown_symbols))


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
                ast.contract_fns.append(_obj)

            ast.function_defs[fn_name] = parent_ctx
            walk_json(_obj["children"], fn_name, ast)
        elif t == "yul_function_call":
            called_name = get_name(_obj)
            # lazy shortcut to avoid checking later for contract functions with defs that haven't been seen yet
            if called_name not in ast.contract_fns and not re.match(FN_REGX, called_name):
                ast.unknown_symbols[called_name] = parent_ctx
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

    # TODO: I'm clearly doing a logic mistake between how I define a known symbol vs uknown symbol and
    #       supported symbols because the arithmetic doesn't come out correct for this set diff operation
    # ast.unknown_symbols = {k:v for (k,v) in ast.function_defs.items() if k not in ast.contract_fns }

    return generate_output(ast)