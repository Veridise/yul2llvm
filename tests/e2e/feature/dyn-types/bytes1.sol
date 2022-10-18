// SPDX-License-Identifier: MIT
/**
This testcase targets storage vaiable in different slot

 */

// RUN: pyul %s --project-dir %S | FileCheck %s

pragma solidity ^0.8.10;


contract StorageBytes1TestCase {
    bytes1 x;
    bytes1 y;

    function readStorageVariable() external view returns (bytes1){
        return x&y;
    }

    function writeStorageVariable(bytes1 a) external {
        x=a;
    }
}

//CHECK: define i256 @fun_readStorageVariable
//CHECK:   %ptr_self_{{.*}} = getelementptr %{{.*}}, %{{.*}}* %0, i32 0, i32 0
//CHECK:   load i8, i8* %ptr_self_{{.*}}
//CHECK:   zext i8 %{{.*}} to i256
//CHECK:   %ptr_self_y = getelementptr %{{.*}}, %{{.*}}* %1, i32 0, i32 1
//CHECK:   load i8, i8* %ptr_self_y
//CHECK:   zext i8 %{{.*}} to i256


//CHECK: define void @fun_writeStorageVariable
//CHECK: %ptr_self_{{.*}} = getelementptr %{{.*}}, %{{.*}}* %0, i32 {{.*}}, i32 {{.*}}
//CHECK: %{{.*}} = trunc i256 %{{.*}} to i8
//CHECK: store i8 %{{.*}}, i8* %ptr_self_{{.*}}