#include<libYulAST/YulIdentifierNode.h>
#include<libYulAST/YulFunctionCallNode.h>
#include<libYulAST/YulFunctionDefinitionNode.h>
#include<libYulAST/YulAssignmentNode.h>
#include<libYulAST/YulBlockNode.h>
#include<libYulAST/YulTypedIdentifierListNode.h>
#include<nlohmann/json.hpp>
#include<iostream>
#include<fstream>

using json = nlohmann::json;
using namespace yulast;

void identifierTest(){
    json a = R"({
            "type":"yul_identifier", 
            "children" : [
                "fun_add_21"
            ]
        })"_json;
    YulIdentifierNode n(&a);
    std::cout<<n.getIdentfierValue()<<std::endl;

}

void typedIdentifierListTest(){
    json j = R"(
{
    "type":"yul_typed_identifier_list",
    "children":
    [
        {
            "type":"yul_identifier",
            "children":[
                "var_a_9"
            ]
        },
        {
            "type":"yul_identifier",
            "children":[
                "var_b_11"
            ]
        }
    ]   
}
    )"_json;
    YulTypedIdentifierListNode ytI(&j);

}

void functionDefinitionTest(){
     json j = R"({
    "type":"yul_function_definition", 
    "children":[
        {
            "type":"yul_identifier", 
            "children" : [
                "fun_add_21"
            ]
        },
        {
            "type": "yul_function_arg_list",
            "children": [
                {
                    "type": "yul_typed_identifier_list",
                    "children":
                    [
                        {
                            "type":"yul_identifier",
                            "children":[
                                "var_a_9"
                            ]
                        },
                        {
                            "type":"yul_identifier",
                            "children":[
                                "var_b_11"
                            ]
                        }
                    ]   
                }
            ]
        },
        {
            "type": "yul_function_ret_list",
            "children": [
                {
                    "type":"yul_typed_identifier_list", 
                    "children":
                    [
                        {
                            "type":"yul_identifier", 
                            "children":[
                                "var__14"
                            ]
                        }
                    ]
                }
            ]
        },
        {
            "type":"yul_block",
            "children": [
                {
                    
                    "type": "yul_variable_declaration",
                    "children":[
                        {
                            "type":"yul_typed_identifier_list",
                            "children":
                            [
                                {
                                    "type":"yul_identifier",
                                    "children":[
                                        "zero_t_uint256_1"
                                    ]
                                }
                            ]
                        }
                    ]
                },
                {
                    
                    "type": "yul_function_call", 
                    "children":
                    [
                        {
                            "type":"yul_identifier",
                            "children":[
                                "zero_value_for_split_t_uint256"
                            ]    
                        }
                    ]
                    
                },
                {                
                    "type":"yul_assignment",
                    "children":[
                        {
                            "type":"yul_identifier_list",
                            "children":[
                                {
                                    "type":"yul_identifier",
                                    "children": [
                                        "var__14"
                                    ]
                                }
                            ]
                        },
                        {
                            "type":"yul_identifier",
                            "children":[
                                "zero_t_uint256_1"
                            ]
                        }
                    ]
                },
                {
                    "type":"yul_variable_declaration",
                    "children":[
                        {
                            "type":"yul_typed_identifier_list",
                            "children":[
                                {
                                    "type":"yul_identifier",
                                    "children":[
                                        "_2"
                                    ]
                                }
                            ]
                        },
                        {
                            "type": "yul_identifier",
                            "children":[
                               "var_a_9"
                            ]
                        }
                    ]
                },
                {
                   
                    "type":"yul_variable_declaration",
                    "children":[
                        {
                            "type":"yul_typed_identifier_list",
                            "children":[
                                {
                                    "type":"yul_identifier",
                                    "children":[
                                        "expr_16"
                                    ]
                                }
                            ]
                        },
                        {
                            "type":"yul_identifier",
														"children": [
				                         "_2"
														]
                            
                        }
                    ]
                    
                },
                {
                    
                    "type":"yul_variable_declaration",
                    "children":[
                        {
                            "type":"yul_typed_identifier_list",
                            "children":[
                                {
                                    "type":"yul_identifier",
                                    "children":[
                                        "_3"
                                    ]
                                }
                            ]
                        },
                        {
                            "type":"yul_identifier",
                            "children":[
                                "var_b_11"
                            ]
                        }
                    ]
                },
                {  
                    "type":"yul_variable_declaration",
                    "children":[
                        {
                            "type":"yul_typed_identifier_list",
                            "children":[
                                {
                                    "type":"yul_identifier",
                                    "children": [ 
                                        "expr_17"
                                    ]
                                }
                            ]
                        },
                        {
                            "type":"yul_identifier",
                            "children": [
                                "_3"
                            ]
                            
                        }
                    ]
                    
                },
                {
                    "type": "yul_variable_declaration",
                    "children":[
                        {
                            "type":"yul_typed_identifier_list",
                            "children":[
                                {
                                    "type":"yul_identifier",
                                    "children":[
                                        "expr_18"
                                    ]
                                }
                            ]
                        },
                        {
                            
                            "type":"yul_function_call",
                            "children":
                            [
                                {
                                    "type": "yul_identifier",
                                    "children":[
                                        "checked_add_t_uint256"
                                    ]
                                },
                                {
                                    "type":"yul_function_arg_list",
                                    "children":[
                                        {
                                            "type":"yul_typed_identifier_list",
                                            "children": [
                                                {
                                                    "type":"yul_identifier",
                                                    "children":[
                                                        "expr_16"
                                                    ]
                                                },
                                                {
                                                    "type": "yul_identifier",
                                                    "children":[
                                                        "expr_17"
                                                    ]
                                                }
                                            ]
                                        }
                                        
                                    ]
                                }
                            ]
                            
                        }
                    ]
                    
                },
                {
                    "type":"yul_assignment",
                    "children": [
                        {
                            "type":"yul_identifier_list",
                            "children":[
                                {
                                    "type":"yul_identifier",
                                    "children":[
                                        "var__14"
                                    ]
                                }
                            ]
                        },
                        {
                            "type":"yul_identifier",
                            "children":[
                                "expr_18"
                            ] 
                        }
                    ]
                },
                {
                    "type": "yul_leave_statement",
										"children":[]
                }
            ]
        }
    ]
})"_json;

    YulFunctionDefinitionNode f(&j);
    std::cout<<"String representation of ast ------------"<<std::endl;
    std::cout<<f.to_string()<<std::endl;
    std::cout<<"llvm representation of ast ------------"<<std::endl;
    f.codegen(NULL);

    
}

int main(){
    functionDefinitionTest();
    return 0;
}