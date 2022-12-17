// SPDX-License-Identifier: MIT

// RUN: pyul %s -o %t --project-dir %S | FileCheck %s
pragma solidity ^0.8.10;

contract StructTest {
    struct Sb {
        uint256 a;
    }
    struct St {
        uint256 a;
        Sb b;
    }
    St st;
    function readStruct() external view returns (uint256){
        return st.b.a;
    }

    function writeStruct(uint256 v) external {
        st.a=v;
    }
}

//CHECK: define i256 @fun_readStruct_{{.*}}(i256 addrspace(1)* %__self) {
//CHECK: {{.*}} = add i256 0, 1
//CHECK: {{.*}} = add i256 {{.*}}, 0
//CHECK: getelementptr %self, %self addrspace(1)* %2, i32 0, i32 0, i32 1, i32 0
//CHECK: load i256, i256 addrspace(1)* %ptr_self_st_b_a, align 4
//CHECK: ret i256 %{{.*}}
//CHECK: define void @fun_writeStruct_{{.*}}(i256 addrspace(1)* %__self, i256 %{{.*}}) {
//CHECK: {{.*}} = add i256 0, 0
//CHECK: %ptr_self_st_a = getelementptr %self, %self addrspace(1)* %1, i32 0, i32 0, i32 0
//CHECK: store i256 %{{.*}}, i256 addrspace(1)* %ptr_self_st_a, align 4
