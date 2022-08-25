// SPDX-License-Identifier: MIT
/**
This testcase targets storage vaiable in different slot

 */

 // RUN: pyul %s -o %t --project-dir %S | FileCheck %s
// XFAIL: *
pragma solidity ^0.8.10;


contract MultipleSlotStorageVariableTestcase {
    uint256 x;
    uint32 y;

    function readStorageVariable() external view returns (uint256){
        return x+y;
    }

    function writeStorageVariable() external {
        x=10;
        y=20;
    }
}