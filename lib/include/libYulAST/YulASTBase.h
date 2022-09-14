#pragma once
#include <libYulAST/YulConstants.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace yulast {
class YulASTBase {
protected:
  YUL_AST_NODE_TYPE nodeType;
  virtual void parseRawAST(const json *rawAst);

public:
  virtual ~YulASTBase(){};
  virtual std::string to_string();
  YulASTBase(const json *rawAST, YUL_AST_NODE_TYPE nodeType);
  bool sanityCheckPassed(const json *rawAST, std::string);
  YUL_AST_NODE_TYPE getType() const;
};
}; // namespace yulast