import unittest
from pyul.utils.yul_parser import YulPrintListener
from pyul.utils.YulAntlr.YulLexer import YulLexer
from pyul.utils.YulAntlr.YulParser import YulParser
from pyul.utils.YulAntlr.YulListener import YulListener
import antlr4
import pytest

YUL_JSON_KV = '{{"type":"{}","children":[{}]}}'

YUL_FUNCTION_IDENT = "revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb"

YUL_FUNCTION_DEF = '''function revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() {
    revert(0, 0)
}'''

def setup_parser(program_input):
    input_stream = antlr4.InputStream(program_input)
    lex = YulLexer(input_stream)
    stream = antlr4.CommonTokenStream(lex)
    parser = YulParser(stream)
    return parser

@pytest.fixture
def yul_printer():
    printer = YulPrintListener()
    printer.clear_built_string()
    yield printer
    printer.clear_built_string()

def test_string_literal(yul_printer):
    parser = setup_parser('"ERC20: decreased allowance below"')
    tree = parser.yul_string_literal()


    walker = antlr4.ParseTreeWalker()
    walker.walk(yul_printer, tree)

    yul_printer.strip_all_trailing()

    correct = YUL_JSON_KV.format("yul_string_literal", '"ERC20: decreased allowance below"')
    assert correct == yul_printer.built_string

def test_dec_literal(yul_printer):
    parser = setup_parser("64")
    tree = parser.yul_dec_number()

    walker = antlr4.ParseTreeWalker()
    walker.walk(yul_printer, tree)

    yul_printer.strip_all_trailing()

    correct = YUL_JSON_KV.format("yul_dec_number", '"64"')
    assert correct == yul_printer.built_string

def test_hex_literal(yul_printer):
    parser = setup_parser("0x41")
    tree = parser.yul_hex_number()

    walker = antlr4.ParseTreeWalker()
    walker.walk(yul_printer, tree)

    yul_printer.strip_all_trailing()
    correct = YUL_JSON_KV.format("yul_hex_number", '"0x41"')
    assert correct == yul_printer.built_string

def test_yul_number_literal(yul_printer):
    parser = setup_parser("64")
    tree = parser.yul_number_literal()

    walker = antlr4.ParseTreeWalker()
    walker.walk(yul_printer, tree)

    yul_printer.strip_all_trailing()

    yul_dec = YUL_JSON_KV.format("yul_dec_number", '"64"')
    yul_number_lit = YUL_JSON_KV.format("yul_number_literal", yul_dec)

    assert yul_number_lit == yul_printer.built_string

def test_identifier(yul_printer):
    parser = setup_parser("revert()")
    tree = parser.yul_identifier()

    walker = antlr4.ParseTreeWalker()
    walker.walk(yul_printer, tree)

    yul_printer.strip_all_trailing()

    correct = YUL_JSON_KV.format("yul_identifier", '"revert"')

    assert correct == yul_printer.built_string

def test_yul_literal(yul_printer):
    parser = setup_parser("64")
    tree = parser.yul_literal()

    walker = antlr4.ParseTreeWalker()
    walker.walk(yul_printer, tree)

    yul_printer.strip_all_trailing()

    yul_dec = YUL_JSON_KV.format("yul_dec_number", '"64"')
    yul_number_lit = YUL_JSON_KV.format("yul_number_literal", yul_dec)
    yul_literal = YUL_JSON_KV.format("yul_literal", yul_number_lit)

    assert yul_literal == yul_printer.built_string


def test_function_call(yul_printer):
    parser = setup_parser("revert(0,0)")
    tree = parser.yul_function_call()

    walker = antlr4.ParseTreeWalker()
    walker.walk(yul_printer, tree)

    yul_printer.strip_all_trailing()

    _arg_1 = YUL_JSON_KV.format("yul_dec_number", '"0"')
    _yul_number_lit = YUL_JSON_KV.format("yul_number_literal", _arg_1)
    _yul_literal_1 = YUL_JSON_KV.format("yul_literal", _yul_number_lit)
    _yul_literal_2 = _yul_literal_1
    _yul_ident = YUL_JSON_KV.format("yul_identifier", '"revert"')

    correct = YUL_JSON_KV.format("yul_function_call", f"{_yul_ident},{_yul_literal_1},{_yul_literal_2}")

    assert correct == yul_printer.built_string

def test_yul_block(yul_printer):
    parser = setup_parser("{ revert(0,0) }")
    tree = parser.yul_block()

    walker = antlr4.ParseTreeWalker()
    walker.walk(yul_printer, tree)

    yul_printer.strip_all_trailing()

    _arg_1 = YUL_JSON_KV.format("yul_dec_number", '"0"')
    _yul_number_lit = YUL_JSON_KV.format("yul_number_literal", _arg_1)
    _yul_literal_1 = YUL_JSON_KV.format("yul_literal", _yul_number_lit)
    _yul_literal_2 = _yul_literal_1
    _yul_ident = YUL_JSON_KV.format("yul_identifier", '"revert"')
    _yul_function_call = YUL_JSON_KV.format("yul_function_call", f"{_yul_ident},{_yul_literal_1},{_yul_literal_2}")

    correct = YUL_JSON_KV.format("yul_block", _yul_function_call)

    assert correct == yul_printer.built_string

def test_yul_function_def(yul_printer):
    parser = setup_parser(YUL_FUNCTION_DEF)
    tree = parser.yul_function_definition()

    walker = antlr4.ParseTreeWalker()
    walker.walk(yul_printer, tree)

    yul_printer.strip_all_trailing()

    _arg_1 = YUL_JSON_KV.format("yul_dec_number", '"0"')
    _yul_number_lit = YUL_JSON_KV.format("yul_number_literal", _arg_1)
    _yul_literal_1 = YUL_JSON_KV.format("yul_literal", _yul_number_lit)
    _yul_literal_2 = _yul_literal_1
    _yul_ident = YUL_JSON_KV.format("yul_identifier", '"revert"')
    _yul_function_call = YUL_JSON_KV.format("yul_function_call", f"{_yul_ident},{_yul_literal_1},{_yul_literal_2}")
    _yul_block = YUL_JSON_KV.format("yul_block", _yul_function_call)
    _yul_function_ident = YUL_JSON_KV.format("yul_identifier", f'"{YUL_FUNCTION_IDENT}"')
    
    correct = YUL_JSON_KV.format("yul_function_definition", f"{_yul_function_ident},{_yul_block}")

    assert correct == yul_printer.built_string
