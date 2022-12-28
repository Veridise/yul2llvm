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
    function readStruct() external view returns (St memory){
        return st;
    }
}

//CHECK: define i256 @fun_readStruct_{{.*}}(i256 addrspace(1)* %__self) {
//CHECK:   %{{newSt}} = call i8 addrspace(2)* @alloc_mem(i32 10)
//CHECK:   call void @llvm.memcpy.p2i8.p1i8.i32(i8 addrspace(2)* align 8 %{{.*}}, i8 addrspace(1)* align 8 null, i32 10, i1 false)
//CHECK:   %{{.*}} = ptrtoint i8 addrspace(2)* %{{.*}} to i256
//CHECK:   ret i256 %{{.*}}
//CHECK: }