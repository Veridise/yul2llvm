// SPDX-License-Identifier: MIT
/**
This testcase targets storage vaiable in same slot

 */

// RUN: pyul %s -o %t --project-dir %S | FileCheck %s
// XFAIL: *

pragma solidity ^0.8.10;


contract ArrayTest {
    uint256[10] array;
    

    function readArray(uint256 index) external view returns (uint256){
        return array[index];
    }

    function writeArray(uint256 index, uint256 value) external {
        array[index] = value;
    }
}