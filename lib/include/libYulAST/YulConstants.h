#pragma once
#define YUL_STATEMENT_KEY "yul_statement"
#define YUL_EXPRESSION_KEY "yul_expression"
#define YUL_IDENTIFIER_KEY "yul_identifier"
#define YUL_BLOCK_KEY "yul_block"
#define YUL_ASSIGNMENT_KEY "yul_assignment"
#define YUL_FUNCTION_ARG_LIST_KEY "yul_function_arg_list"
#define YUL_FUNCTION_RET_LIST_KEY "yul_function_ret_list"
#define YUL_FUNCTION_CALL_KEY "yul_function_call"
#define YUL_FUNCTION_DEFINITION_KEY "yul_function_definition"
#define YUL_LEAVE_KEY "yul_leave"
#define YUL_TYPED_IDENTIFIER_LIST_KEY "yul_typed_identifier_list"
#define YUL_IDENTIFIER_LIST_KEY "yul_identifier_list"
#define YUL_VARIABLE_DECLARATION_KEY "yul_variable_declaration"
#define YUL_LITERAL_KEY "yul_literal"
#define YUL_NUMBER_LITERAL_KEY "yul_number_literal"
#define YUL_STRING_LITERAL_KEY "yul_string_literal"
#define YUL_DEC_NUMBER_LITERAL_KEY "yul_dec_number"
#define YUL_IF_KEY "yul_if"
#define YUL_HEX_NUMBER_LITERAL_KEY "yul_hex_number"
#define YUL_SWITCH_KEY "yul_switch"
#define YUL_CASE_KEY "yul_case"
#define YUL_DEFAULT_KEY "yul_default"
#define YUL_BREAK_KEY "yul_break"
#define YUL_CONTINUE_KEY "yul_continue"
#define YUL_FOR_KEY "yul_for_loop"

namespace yulast {
enum class YUL_AST_NODE_TYPE {
  UNDEFINED_TYPE,
  YUL_AST_NODE_STATEMENT,
  YUL_AST_NODE_FUNCTION_ARG_LIST,
  YUL_AST_NODE_FUNCTION_RET_LIST,
  YUL_AST_NODE_TYPED_IDENTIFIER_LIST,
  YUL_AST_NODE_IDENTIFIER_LIST,
  YUL_AST_NODE_CASE,
  YUL_AST_NODE_DEFAULT,
  YUL_AST_NODE_CONTRACT
};

enum class YUL_AST_STATEMENT_NODE_TYPE {
  YUL_AST_STATEMENT_ASSIGNMENT,
  YUL_AST_STATEMENT_VARIABLE_DECLARATION,
  YUL_AST_STATEMENT_FUNCTION_DEFINITION,
  YUL_AST_STATEMENT_BLOCK,
  YUL_AST_STATEMENT_EXPRESSION,
  YUL_AST_STATEMENT_LEAVE,
  YUL_AST_STATEMENT_IF,
  YUL_AST_STATEMENT_SWITCH,
  YUL_AST_STATEMENT_BREAK,
  YUL_AST_STATEMENT_CONTINUE,
  YUL_AST_STATEMENT_FOR,
};

enum class YUL_AST_EXPRESSION_NODE_TYPE {
  YUL_AST_EXPRESSION_FUNCTION_CALL,
  YUL_AST_EXPRESSION_IDENTIFIER,
  YUL_AST_EXPRESSION_LITERAL,
};

enum class YUL_AST_LITERAL_NODE_TYPE { 
  YUL_AST_LITERAL_NUMBER,
  YUL_AST_LITERAL_STRING
};
}; // namespace yulast