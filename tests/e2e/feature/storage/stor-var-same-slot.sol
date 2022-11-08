// SPDX-License-Identifier: MIT
/**
This testcase targets storage vaiable in same slot

 */

 // RUN: pyul %s -o %t --project-dir %S | FileCheck %s

pragma solidity ^0.8.10;


contract SameSlotStorageVariableTestcase {
    uint32 x;
    uint32 y;

    function readStorageVariable() external view returns (uint32){
        return x+y;
    }

    function writeStorageVariable() external {
        x=10;
        y=20;
    }
}

// CHECK: define i256 @fun_readStorageVariable_15(i256 addrspace(1)* %__self)
// CHECK: getelementptr %{{.*}}, %{{.*}}* %{{.*}}, i32 0, i32 0
// CHECK:  %{{pyul_storage_var_load.*}} = load i32, i32 addrspace(1)* %ptr_self_x, align 4
// CHECK:  %i256_pyul_storage_var_load = zext i32 %{{pyul_storage_var_load.*}} to i256
// CHECK: getelementptr %{{.*}}, %{{.*}}* %{{.*}}, i32 0, i32 1
// CHECK:  %{{pyul_storage_var_load.*}} = load i32, i32 addrspace(1)* %ptr_self_y, align 4
// CHECK:  %{{i256_pyul_storage_var_load.*}} = zext i32 %{{pyul_storage_var_load.*}} to i256


// CHECK: define void @fun_writeStorageVariable_{{.*}}
// CHECK: getelementptr %{{.*}}, %{{.*}}* %{{.*}}, i32 0, i32 0
// CHECK: store i32 {{.*}}, i32 addrspace(1)* %{{.*}}, align 4
// CHECK: getelementptr %{{.*}}, %{{.*}}* %{{.*}}, i32 0, i32 1
// CHECK: store i32 {{.*}}, i32 addrspace(1)* %{{.*}}, align 4