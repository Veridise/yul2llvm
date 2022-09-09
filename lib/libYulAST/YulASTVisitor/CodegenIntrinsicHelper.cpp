#include <libYULAST/YulASTVisitor/
bool LLVMCodegen::isFunctionCallIntrinsic(std::string calleeName){
  if (node.getCalleeName() == "pyul_storage_var_load") {
    return true;
  } else if (node.getCalleeName() == "pyul_storage_var_update") {
    return true;
  }
}