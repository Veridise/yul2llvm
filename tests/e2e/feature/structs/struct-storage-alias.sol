// SPDX-License-Identifier: MIT
/**
This testcase targets storage vaiable in same slot

 */

// RUN: pyul %s -o %t --project-dir %S | FileCheck %s
//XFAIL: *
pragma solidity ^0.8.10;

contract StructTest {
    struct Sb {
        uint256 a; //slot 1, offset 0
    }
    struct St {
        uint256 a; //slot 0, offset 0 
        Sb b; //slot 1, offset 0
    }
    St st;
    function readStruct() external view returns (uint256){
        St storage ref;
        ref = st;
        return ref.b.a;
    }
}

//CHECK: define i256 @fun_readStruct_{{.*}}(i256 addrspace(1)* %__self) {
//CHECK: {{.*}} = add i256 0, 1
//CHECK: {{.*}} = add i256 {{.*}}, 0
//CHECK: %ptr_self_st_b_a = getelementptr %self, %self addrspace(1)* %2, i32 0, i32 0, i32 1, i32 0
//CHECK: %pyul_storage_var_load = load i256, i256 addrspace(1)* %ptr_self_st_b_a, align 4
//CHECK: ret i256 %i256_pyul_storage_var_load


