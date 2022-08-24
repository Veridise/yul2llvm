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