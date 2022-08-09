#include<libYulAST/YulASTBase.h>

yulast::YulASTBase::YulASTBase(json *rawAST, YUL_AST_NODE_TYPE nodetype):rawAST(rawAST), nodeType(nodeType){
    
}