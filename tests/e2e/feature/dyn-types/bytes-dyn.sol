// SPDX-License-Identifier: MIT
/**
This testcase targets storage vaiable in different slot

 */

// RUN: pyul %s --project-dir %S | FileCheck %s

pragma solidity ^0.8.10;


contract BytesMemoryTestcase {

    function readBytes(bytes memory array, uint256 index) external view returns (bytes1){
        return array[index];
    }

    function writeBytes(bytes memory array, uint256 index, bytes1 value) external {
        array[index] = value;
    }
}

//CHECK: define {{.*}} @fun_readBytes{{.*}}(i256 addrspace(1)* %__self, {{.*}}, {{.*}}) {
//CHECK: getelementptr [0 x i8], [0 x i8]* %{{.*}}, i32 0, i256 %{{.*}}
//CHECK: load i8, i8* %{{.*}}

//CHECK: define void @fun_writeBytes_{{.*}}(i256 addrspace(1)* %{{.*}}, i256 %{{.*}}, i256 %{{.*}}, i256 %{{.*}}) {
//CHECK: getelementptr [0 x i8], [0 x i8]* %{{.*}}, i32 0, i256 %{{.*}}
//CHECK: trunc i256 %var_value_21 to i8
//CHECK: store i8 %{{.*}}, i8* %{{.*}}


