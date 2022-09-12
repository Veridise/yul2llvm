#include <iostream>
#include <libYulAST/YulASTBase.h>

using namespace yulast;

bool YulASTBase::sanityCheckPassed(const json *rawAST, std::string key) {
  if (!rawAST->contains("type")) {
    std::cout << "type not present" << std::endl;
    std::cout << rawAST->dump() << std::endl;
    return false;
  }
  if (!rawAST->contains("children")) {
    std::cout << "children not present" << std::endl;
    std::cout << rawAST->dump() << std::endl;
    return false;
  }
  if ((rawAST->size() != 2)) {
    std::cout << "size not equal 2" << std::endl;
    std::cout << rawAST->dump() << std::endl;
    return false;
  }
  if ((*rawAST)["type"].get<std::string>().compare(key)) {
    std::cout << "wrong key" << std::endl;
    std::cout << rawAST->dump() << std::endl;
    return false;
  }
  return true;
}

std::string YulASTBase::to_string() { return "to_str not defined for base"; }

void YulASTBase::parseRawAST(const json *rawAST) {
  std::cout << "Parsing Not Implemented" << std::endl;
}

YulASTBase::YulASTBase(const json *rawAST, YUL_AST_NODE_TYPE nodeType)
    : nodeType(nodeType) {}

YUL_AST_NODE_TYPE YulASTBase::getType(){
  return nodeType;
}