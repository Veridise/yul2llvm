// SPDX-License-Identifier: MIT
/**
This testcase targets storage vaiable in different slot

 */

// RUN: pyul %s --project-dir %S --stop-after preprocess > %t.pre

pragma solidity ^0.8.10;


contract MultipleSlotStorageVariableTestcase {
    uint256 x;
    uint256 y;

    function readStorageVariable() external view returns (uint256){
        return x+y;
    }

    function writeStorageVariable() external {
        x=10;
        y=20;
    }
}

// CHECK: define i256 @fun_readStorageVariable
// CHECK: %self_x = load i256, i256* getelementptr inbounds (%self_type, %self_type* @__self, i32 0, i32 0)
// CHECK: %self_x = load i256, i256* getelementptr inbounds (%self_type, %self_type* @__self, i32 0, i32 1)

// CHECK: define void @fun_writeStorageVariable
// CHECK: {{store .* getelementptr inbounds \(%self_type, %self_type\* @__self, i32 0, i32 0\)}}
// CHECK: {{store .* getelementptr inbounds \(%self_type, %self_type\* @__self, i32 0, i32 1\)}}
