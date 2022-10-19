// SPDX-License-Identifier: MIT
/**
This testcase targets storage vaiable in same slot

 */

// RUN: pyul %s -o %t --project-dir %S | FileCheck %s

pragma solidity ^0.8.10;

contract ArrayTest {
    function readArray(uint32[10] memory array, uint256 index) external view returns (uint32){
        return array[index];
    }

    function writeArray(uint32[10] memory array, uint256 index, uint32 value) external {
        array[index] = value;
    }
}

//CHECK: define i256 @fun_readArray_{{[0-9]+}}(i256* %{{.+, .+, .+}})
//CHECK: getelementptr [0 x {{.*}}], [0 x {{.*}}]*
//CHECK: arr_load{{.*}} = load i32, i32*
//CHECK: %word_arr_load = zext i32


//CHECK: define void @fun_writeArray_{{[0-9]+}}(i256* %{{.*, .+, .+, .+}})
//CHECK: getelementptr [0 x {{.*}}], [0 x {{.*}}]*
//CHECK: store i32 {{.*}}, i32*
