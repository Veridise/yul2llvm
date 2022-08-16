#include <cassert>
#include <iostream>
#include <libYulAST/YulBlockNode.h>

using namespace yulast;

void YulBlockNode::parseRawAST() {
  json topLevelChildren = rawAST->at("children");
  for (json::iterator it = topLevelChildren.begin();
       it != topLevelChildren.end(); it++) {
    YulStatementNode *builtStatement = YulStatementBuilder::Builder(&(*it));
    if(builtStatement == NULL){
      //skipping building some statement take care of it
    } 
    else 
      statements.push_back(builtStatement);
  }
}

YulBlockNode::YulBlockNode(json *rawAST)
    : YulStatementNode(rawAST, YUL_AST_STATEMENT_BLOCK) {
  assert(sanityCheckPassed(YUL_BLOCK_KEY));
  parseRawAST();
}

std::string YulBlockNode::to_string() {
  if (!str.compare("")) {
    str.append("{\n");
    for (std::vector<YulStatementNode *>::iterator it = statements.begin();
         it != statements.end(); it++) {
      str.append((*it)->to_string()).append("\n");
    }
    str.append("\n");
  }
  return str;
}

llvm::Value *YulBlockNode::codegen(llvm::Function *F) {
  for (auto s : statements) {
    // std::cout<<s->to_string()<<std::endl;
    if (s->statementType == YUL_AST_STATEMENT_EXPRESSION) {
      if (((YulExpressionNode *)s)->expressionType ==
          YUL_AST_EXPRESSION_FUNCTION_CALL) {
        YulFunctionCallNode *expr = (YulFunctionCallNode *)s;
        expr->codegen(F);
      }
    } else if (s->statementType == YUL_AST_STATEMENT_VARIABLE_DECLARATION) {
      ((YulVariableDeclarationNode *)s)->codegen(F);
    } else if (s->statementType == YUL_AST_STATEMENT_ASSIGNMENT) {
      ((YulAssignmentNode *)s)->codegen(F);
    }
  }
  return nullptr;
}