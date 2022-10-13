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

//CHECK: define {{.*}} @fun_readBytes{{.*}}(i256* %{{.*}}, {{.*}}, {{.*}}) {
//CHECK: getelementptr [0 x i256], [0 x i256]* %{{.*}}, i32 0, i256 %{{.*}}
//CHECK: load i256, i256* %{{.*}}
//CHECK: and i256 %{{.*}}, -452312848583266388373324160190187140051835877600158453279131187530910662656

//CHECK: define void @fun_writeBytes_{{.*}}(i256* %{{.*}}, i256 %{{.*}}, i256 %{{.*}}, i256 %{{.*}}) {
//CHECK: getelementptr [0 x i256], [0 x i256]* %{{.*}}, i32 0, i256 %{{.*}}
//CHECK: trunc i256 %var_value_21 to i8
//CHECK: %{{.*}} = bitcast i256* %0 to i8*
//CHECK: store i8 %{{.*}}, i8* %{{.*}}


