from dataclasses import dataclass
import re

from antlr4 import *
from .YulAntlr.YulLexer import YulLexer
from .YulAntlr.YulParser import YulParser
from .YulAntlr.YulListener import YulListener


class YulPrintListener(YulListener):
    def __init__(self, *args, **kwargs):
        super(YulPrintListener, self).__init__(*args, **kwargs)
        # the string representation of the total AST as traversed by our listener
        self.built_string = ""
        # flag that tracks first contract object
        self.first_obj = True

    def clear_built_string(self):
        self.built_string = ""

    # Enter a parse tree produced by YulParser#start.
    def enterStart(self, ctx:YulParser.StartContext):
        self.built_string += '{"type":"start","yul_ast":'

    # Exit a parse tree produced by YulParser#start.
    def exitStart(self, ctx:YulParser.StartContext):
        self.built_string += '},'

    # Enter a parse tree produced by YulParser#yul_object.
    def enterYul_object(self, ctx:YulParser.Yul_objectContext):
        if self.first_obj:
            self.built_string += '{"type":"yul_object","object_name":'
            self.first_obj = False
        else:
            self.built_string += '"object_body":{"type":"yul_object","object_name":'


    # Exit a parse tree produced by YulParser#yul_object.
    def exitYul_object(self, ctx:YulParser.Yul_objectContext):
        self.built_string += "},"

    # Enter a parse tree produced by YulParser#yul_code.
    def enterYul_code(self, ctx:YulParser.Yul_codeContext):
        self.built_string += '"contract_body":{"type":"yul_code","children":['

    # Exit a parse tree produced by YulParser#yul_code.
    def exitYul_code(self, ctx:YulParser.Yul_codeContext):
        self.built_string += ']},'

    # Enter a parse tree produced by YulParser#yul_if.
    def enterYul_if(self, ctx:YulParser.Yul_ifContext):
        self.built_string += '{"type":"yul_if","children": ['

    # Exit a parse tree produced by YulParser#yul_if.
    def exitYul_if(self, ctx:YulParser.Yul_ifContext):
        self.built_string += ']},'

    # Enter a parse tree produced by YulParser#yul_switch.
    def enterYul_switch(self, ctx:YulParser.Yul_switchContext):
        self.built_string += '{"type":"yul_switch","children":['

    # Exit a parse tree produced by YulParser#yul_switch.
    def exitYul_switch(self, ctx:YulParser.Yul_switchContext):
        self.built_string += ']},'

    # Enter a parse tree produced by YulParser#yul_case.
    def enterYul_case(self, ctx:YulParser.Yul_caseContext):
        self.built_string += '{"type":"yul_case","children":['

    # Exit a parse tree produced by YulParser#yul_case.
    def exitYul_case(self, ctx:YulParser.Yul_caseContext):
        self.built_string += ']},'

    # Enter a parse tree produced by YulParser#yul_default.
    def enterYul_default(self, ctx:YulParser.Yul_defaultContext):
        self.built_string += '{"type":"yul_default","children":['

    # Exit a parse tree produced by YulParser#yul_default.
    def exitYul_default(self, ctx:YulParser.Yul_defaultContext):
        self.built_string += ']},'

    # Enter a parse tree produced by YulParser#yul_for_loop.
    def enterYul_for_loop(self, ctx:YulParser.Yul_for_loopContext):
        self.built_string += '{"type":"yul_for_loop","children":['

    # Exit a parse tree produced by YulParser#yul_for_loop.
    def exitYul_for_loop(self, ctx:YulParser.Yul_for_loopContext):
        self.built_string += ']},'

    # Enter a parse tree produced by YulParser#yul_break.
    def enterYul_break(self, ctx:YulParser.Yul_breakContext):
        # leave as nothing but as a leaf of the concrete value in the AST. possibly remove if the semantics of a break
        # don't actually matter at this phase but I doubt?
        self.built_string += '{"type":"yul_break"},'

    # Exit a parse tree produced by YulParser#yul_break.
    def exitYul_break(self, ctx:YulParser.Yul_breakContext):
        # see :enterYul_break:
        pass

    # Enter a parse tree produced by YulParser#yul_continue.
    def enterYul_continue(self, ctx:YulParser.Yul_continueContext):
        # leaf of literal value in AST
        self.built_string += '{"type":"yul_continue"},'

    # Exit a parse tree produced by YulParser#yul_continue.
    def exitYul_continue(self, ctx:YulParser.Yul_continueContext):
        # see :enterYul_continue:
        pass

    # Enter a parse tree produced by YulParser#yul_leave.
    def enterYul_leave(self, ctx:YulParser.Yul_leaveContext):
        # leaf of literal value in AST
        self.built_string += '{"type":"yul_leave"},'

    # Exit a parse tree produced by YulParser#yul_leave.
    def exitYul_leave(self, ctx:YulParser.Yul_leaveContext):
        # see :enterYul_leave:
        pass

    # Enter a parse tree produced by YulParser#yul_function_definition.
    def enterYul_function_definition(self, ctx:YulParser.Yul_function_definitionContext):
        self.built_string += '{"type":"yul_function_definition","children":['

    # Exit a parse tree produced by YulParser#yul_function_definition.
    def exitYul_function_definition(self, ctx:YulParser.Yul_function_definitionContext):
        self.built_string += ']},'

    # Enter a parse tree produced by YulParser#yul_variable_declaration.
    def enterYul_variable_declaration(self, ctx:YulParser.Yul_variable_declarationContext):
        self.built_string += '{"type":"yul_variable_declaration","children":['

    # Exit a parse tree produced by YulParser#yul_variable_declaration.
    def exitYul_variable_declaration(self, ctx:YulParser.Yul_variable_declarationContext):
        self.built_string += ']},'

    # Enter a parse tree produced by YulParser#yul_function_arg_list.
    def enterYul_function_arg_list(self, ctx:YulParser.Yul_function_arg_listContext):
        self.built_string += '{"type":"yul_function_arg_list","children":['

    # Exit a parse tree produced by YulParser#yul_function_arg_list.
    def exitYul_function_arg_list(self, ctx:YulParser.Yul_function_arg_listContext):
        self.built_string += ']},'

    # Enter a parse tree produced by YulParser#yul_function_ret_list.
    def enterYul_function_ret_list(self, ctx:YulParser.Yul_function_ret_listContext):
        self.built_string += '{"type":"yul_function_ret_list","children":['

    # Exit a parse tree produced by YulParser#yul_function_ret_list.
    def exitYul_function_ret_list(self, ctx:YulParser.Yul_function_ret_listContext):
        self.built_string += ']},'

    # Enter a parse tree produced by YulParser#yul_typed_identifier_list.
    def enterYul_typed_identifier_list(self, ctx:YulParser.Yul_typed_identifier_listContext):
        self.built_string += '{"type":"yul_typed_identifier_list","children":['

    # Exit a parse tree produced by YulParser#yul_typed_identifier_list.
    def exitYul_typed_identifier_list(self, ctx:YulParser.Yul_typed_identifier_listContext):
        self.built_string += ']},'

    # Enter a parse tree produced by YulParser#yul_identifier_list.
    def enterYul_identifier_list(self, ctx:YulParser.Yul_identifier_listContext):
        self.built_string += '{"type":"yul_identifier_list","children":['

    # Exit a parse tree produced by YulParser#yul_identifier_list.
    def exitYul_identifier_list(self, ctx:YulParser.Yul_identifier_listContext):
        self.built_string += ']},'

    # Enter a parse tree produced by YulParser#yul_block.
    def enterYul_block(self, ctx:YulParser.Yul_blockContext):
        self.built_string += '{"type":"yul_block","children":['

    # Exit a parse tree produced by YulParser#yul_block.
    def exitYul_block(self, ctx:YulParser.Yul_blockContext):
        self.built_string += ']},'

    # Enter a parse tree produced by YulParser#yul_statement.
    def enterYul_statement(self, ctx:YulParser.Yul_statementContext):
        pass

    # Exit a parse tree produced by YulParser#yul_statement.
    def exitYul_statement(self, ctx:YulParser.Yul_statementContext):
        pass

    # Enter a parse tree produced by YulParser#yul_assignment.
    def enterYul_assignment(self, ctx:YulParser.Yul_assignmentContext):
        self.built_string += '{"type":"yul_assignment","children":['

    # Exit a parse tree produced by YulParser#yul_assignment.
    def exitYul_assignment(self, ctx:YulParser.Yul_assignmentContext):
        self.built_string += ']},'

    # Enter a parse tree produced by YulParser#yul_expression.
    def enterYul_expression(self, ctx:YulParser.Yul_expressionContext):
        pass

    # Exit a parse tree produced by YulParser#yul_expression.
    def exitYul_expression(self, ctx:YulParser.Yul_expressionContext):
        pass

    # Enter a parse tree produced by YulParser#yul_function_call.
    def enterYul_function_call(self, ctx:YulParser.Yul_function_callContext):
        self.built_string += '{"type":"yul_function_call","children":['

    # Exit a parse tree produced by YulParser#yul_function_call.
    def exitYul_function_call(self, ctx:YulParser.Yul_function_callContext):
        self.built_string += ']},'

    # Enter a parse tree produced by YulParser#yul_literal.
    def enterYul_literal(self, ctx:YulParser.Yul_literalContext):
        self.built_string += '{"type":"yul_literal","children":['

    # Exit a parse tree produced by YulParser#yul_literal.
    def exitYul_literal(self, ctx:YulParser.Yul_literalContext):
        self.built_string += ']},'

    # Enter a parse tree produced by YulParser#yul_number_literal.
    def enterYul_number_literal(self, ctx:YulParser.Yul_number_literalContext):
        self.built_string += '{"type":"yul_number_literal","children":['

    # Exit a parse tree produced by YulParser#yul_number_literal.
    def exitYul_number_literal(self, ctx:YulParser.Yul_number_literalContext):
        self.built_string += ']},'

    # @ejmg NOTE: For the next set of production rules where the node is nothing but a leaf of a concrete literal
    # please bear in mind that the surrounding `{` and `}` of the emitted JSON object has to be escaped by using
    # two of each. This is because the .format() method is interpolating on the `{}` found in each `"children"`.

    # Enter a parse tree produced by YulParser#yul_true_literal.
    def enterYul_true_literal(self, ctx:YulParser.Yul_true_literalContext):
        self.built_string += '{{"type":"yul_true_literal","children":["{}"]}},'.format(ctx.TRUE_LITERAL())
    # Exit a parse tree produced by YulParser#yul_true_literal.
    def exitYul_true_literal(self, ctx:YulParser.Yul_true_literalContext):
        pass

    # Enter a parse tree produced by YulParser#yul_false_literal.
    def enterYul_false_literal(self, ctx:YulParser.Yul_false_literalContext):
        self.built_string += '{{"type":"yul_false_literal","children":["{}"]}},'.format(ctx.FALSE_LITERAL())
    # Exit a parse tree produced by YulParser#yul_false_literal.
    def exitYul_false_literal(self, ctx:YulParser.Yul_false_literalContext):
        pass

    # Enter a parse tree produced by YulParser#yul_hex_number.
    def enterYul_hex_number(self, ctx:YulParser.Yul_hex_numberContext):
        self.built_string += '{{"type":"yul_hex_number","children":["{}"]}},'.format(ctx.HEX_NUMBER())
    # Exit a parse tree produced by YulParser#yul_hex_number.
    def exitYul_hex_number(self, ctx:YulParser.Yul_hex_numberContext):
        pass

    # Enter a parse tree produced by YulParser#yul_dec_number.
    def enterYul_dec_number(self, ctx:YulParser.Yul_dec_numberContext):
        self.built_string += '{{"type":"yul_dec_number","children":["{}"]}},'.format(ctx.DEC_NUMBER())
    # Exit a parse tree produced by YulParser#yul_dec_number.
    def exitYul_dec_number(self, ctx:YulParser.Yul_dec_numberContext):
        pass

    # Enter a parse tree produced by YulParser#yul_type_name.
    def enterYul_type_name(self, ctx:YulParser.Yul_type_nameContext):
        self.built_string += '{{"type":"yul_type_name","children":["{}"]}},'.format(ctx.ID_LITERAL())

    # Exit a parse tree produced by YulParser#yul_type_name.
    def exitYul_type_name(self, ctx:YulParser.Yul_type_nameContext):
        pass

    # Enter a parse tree produced by YulParser#yul_identifier.
    def enterYul_identifier(self, ctx:YulParser.Yul_identifierContext):
        self.built_string += '{{"type":"yul_identifier","children":["{}"]}},'.format(ctx.ID_LITERAL())

    # Exit a parse tree produced by YulParser#yul_identifier.
    def exitYul_identifier(self, ctx:YulParser.Yul_identifierContext):
        pass

    # Enter a parse tree produced by YulParser#yul_string_literal.
    def enterYul_string_literal(self, ctx:YulParser.Yul_string_literalContext):
        # note: get rid of quotes around string literal
        self.built_string += '{{"type":"yul_string_literal","children":[{}]}},'.format(ctx.STRING_LITERAL())
    # Exit a parse tree produced by YulParser#yul_string_literal.
    def exitYul_string_literal(self, ctx:YulParser.Yul_string_literalContext):
        pass

    def strip_all_trailing(self):
        self.built_string = re.sub(r",\}", "}", self.built_string)
        self.built_string = re.sub(r",]", "]", self.built_string)
        self.built_string = self.built_string.strip(",")
        self.built_string = self.built_string.strip()

def make_json_arr(ls):
    """transform a list of JSON-ish objects in a python list into a proper string representation"""
    if len(ls) > 0:
        arr = []
        for fn in ls:
            arr.append(f"{fn},")
        json_arr = "".join(arr)
        # trailing comma
        json_arr = json_arr[:-1]
        return f"[{json_arr}]"
    else:
        return f"[{ls[0]}]"
