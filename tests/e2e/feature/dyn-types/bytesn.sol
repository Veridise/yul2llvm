// SPDX-License-Identifier: MIT
/**
This testcase targets storage vaiable in different slot

 */

// RUN: pyul %s --project-dir %S --stop-after preprocess > %t.pre

//XFAIL: *

pragma solidity ^0.8.10;


contract MultipleSlotStorageVariableTestcase {
    bytes2 x;
    bytes2 y;

    function readStorageVariable() external view returns (bytes2){
        return x&y;
    }

    function writeStorageVariable(bytes2 a) external {
        x=a;
    }
}