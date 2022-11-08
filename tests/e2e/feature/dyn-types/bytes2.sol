// SPDX-License-Identifier: MIT
/**
This testcase targets storage vaiable in different slot

 */

// RUN: pyul %s --project-dir %S | FileCheck %s

pragma solidity ^0.8.10;


contract StorageBytes2testCase {
    bytes2 x;
    bytes2 y;

    function readStorageVariable() external view returns (bytes2){
        return x&y;
    }

    function writeStorageVariable(bytes2 a) external {
        x=a;
    }
}

//CHECK: define i256 @fun_readStorageVariable
//CHECK:   %ptr_self_{{.*}} = getelementptr %{{.*}}, %{{.*}}* %0, i32 0, i32 0
//CHECK:   load i16, i16 addrspace(1)* %ptr_self_{{.*}}
//CHECK:   zext i16 %{{.*}} to i256
//CHECK:   %ptr_self_y = getelementptr %{{.*}}, %{{.*}}* %1, i32 0, i32 1
//CHECK:   load i16, i16 addrspace(1)* %ptr_self_y
//CHECK:   zext i16 %{{.*}} to i256


//CHECK: define void @fun_writeStorageVariable
//CHECK: %ptr_self_{{.*}} = getelementptr %{{.*}}, %{{.*}}* %0, i32 {{.*}}, i32 {{.*}}
//CHECK: %{{.*}} = trunc i256 %{{.*}} to i16
//CHECK: store i16 %{{.*}}, i16 addrspace(1)* %ptr_self_{{.*}}

