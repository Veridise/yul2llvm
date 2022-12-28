// SPDX-License-Identifier: MIT
/**
This testcase targets storage vaiable in same slot

 */

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
        St memory memlocal;
        return memlocal.b.a;
    }

    function writeStruct() external {
        St memory memlocal;
        memlocal.b.a = 1;
    }
}
//CHECK: define i256 @fun_readStruct_{{.*}}(i256 addrspace(1)* %__self) {
//CHECK: %"zero_value_for_split_t_struct$_St_$10_memory_ptr" = call i256 @"zero_value_for_split_t_struct$_St_$10_memory_ptr"(i256 addrspace(1)* %__self)
//CHECK: %mem_St_ptr = getelementptr %St, %St addrspace(2)* %St_casted_ptr, i32 0, i32 1, i32 0
//CHECK: %mem_load = load i256, i256 addrspace(2)* %mem_St_ptr, align 4
//CHECK: ret i256 %mem_load

//CHECK: define void @fun_writeStruct_43(i256 addrspace(1)* %__self) {
//CHECK: %"zero_value_for_split_t_struct$_St_$10_memory_ptr" = call i256 @"zero_value_for_split_t_struct$_St_$10_memory_ptr"(i256 addrspace(1)* %__self)
//CHECK: %0 = add i256 %"zero_value_for_split_t_struct$_St_$10_memory_ptr", 32
//CHECK: %mload = call i256 @mload(i256 %0)
//CHECK: %1 = add i256 %mload, 0
//CHECK: %St_casted_ptr = inttoptr i256 %"zero_value_for_split_t_struct$_St_$10_memory_ptr" to %St addrspace(2)*
//CHECK: %mem_St_ptr = getelementptr %St, %St addrspace(2)* %St_casted_ptr, i32 0, i32 1, i32 0
//CHECK: store i256 1, i256 addrspace(2)* %mem_St_ptr, align 4
