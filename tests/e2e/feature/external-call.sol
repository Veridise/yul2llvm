// SPDX-License-Identifier: MIT
// RUN: pyul %s -o %t --project-dir %S | FileCheck %s
pragma solidity ^0.8.10;



contract ExternalCallTest {
    uint256 x;
    function add(address payable addr) external returns (uint256) {
        return x + CalleeContract(addr).add1(10, x);
    }
}

contract CalleeContract{
    function add1(uint256 x, uint256 y) external payable returns (uint256){
        return x+y+1;
    }
}

//CHECK: define i256 @fun_add_{{.*}}(i256 addrspace(1)* %__self, {{.*}})
//CHECK: %{{gasPtr.*}} = getelementptr %ExtCallContextType, %ExtCallContextType* %{{.*}}, i32 0, i32 0
//CHECK: store i256 %{{.*}}, i256* %{{gasPtr.*}}, align 4
//CHECK: %{{addrPtr.*}} = getelementptr %ExtCallContextType, %ExtCallContextType* %{{.*}}, i32 0, i32 1
//CHECK: store i256 %{{.*}}, i256* %{{addrPtr.*}}, align 4
//CHECK: %{{valuePtr.*}} = getelementptr %ExtCallContextType, %ExtCallContextType* %{{.*}}, i32 0, i32 2
//CHECK: store i256 {{.*}}, i256* %{{valuePtr.*}}, align 4
//CHECK: %{{bufferPtr.*}} = getelementptr %ExtCallContextType, %ExtCallContextType* %{{.*}}, i32 0, i32 3
//CHECK: store i256* %{{.*}}, i256** %{{bufferPtr.*}}, align 8
//CHECK: %{{retLenPtr.*}} = getelementptr %ExtCallContextType, %ExtCallContextType* %{{.*}}, i32 0, i32 4
//CHECK: store i256 {{.*}}, i256* %{{retLenPtr.*}}, align 4
//CHECK: %ret_struct = call %"0x{{.*}}_statusRetType"* @pyul_call_0x{{.*}}(i256 addrspace(1)* %{{.*}}, %ExtCallContextType* %"0x{{.*}}ctx", i256 {{.*}}, i256 %{{.*}})
//CHECK: %ptr_status = getelementptr %"0x{{.*}}_statusRetType", %"0x{{.*}}_statusRetType"* %ret_struct, i32 0, i32 0
//CHECK: %status = load i256, i256* %ptr_status, align 4
//CHECK: %ptr_returns = getelementptr %"{{.*}}_statusRetType", %"{{.*}}_statusRetType"* %ret_struct, i32 0, i32 1
//CHECK: %returns = load i256, i256* %ptr_returns, align 4