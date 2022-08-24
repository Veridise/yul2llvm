#pragma once

#include <libYulAST/YulBlockNode.h>
#include <libYulAST/YulDefaultNode.h>
#include <nlohmann/json.hpp>

namespace yulast {

class YulForNode : public YulStatementNode {
protected:
  std::unique_ptr<YulBlockNode> initializationNode;
  std::unique_ptr<YulBlockNode> incrementNode;
  std::unique_ptr<YulBlockNode> body;
  std::unique_ptr<YulBlockNode> conditionNode;
  static int loopCreated;
  int loopNumber;
  void createBodyAndCondition(std::unique_ptr<YulBlockNode> &);

public:
  std::string str = "";
  llvm::Value *codegen(llvm::Function *) override;
  virtual void parseRawAST(const json *rawAST) override;
  virtual std::string to_string() override;
  YulForNode(const json *rawAST);
  std::unique_ptr<YulExpressionNode> &getCondition();
  std::unique_ptr<YulBlockNode> &getConditionNode();
  std::unique_ptr<YulBlockNode> &getInitializationNode();
  std::unique_ptr<YulBlockNode> &getIncrementNode();
  std::unique_ptr<YulBlockNode> &getBody();
};
}; // namespace yulast