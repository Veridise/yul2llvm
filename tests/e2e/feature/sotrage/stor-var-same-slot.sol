// SPDX-License-Identifier: MIT
/**
This testcase targets storage vaiable in same slot

 */

 // RUN: pyul %s -o %t --project-dir %S | FileCheck %s
// XFAIL: *

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

// CHECK: define i256 @fun_readStorageVariable
// CHECK: %self_x = load i256, i256* getelementptr inbounds (%self_type, %self_type* @__self, i32 0, i32 0)

// CHECK: define void @fun_writeStorageVariable
// CHECK: {{store .* getelementptr inbounds \(%self_type, %self_type\* @__self, i32 0, i32 0\)}}
