// SPDX-License-Identifier: MIT
/**
This testcase targets storage vaiable in different slot

 */

// RUN: pyul %s --project-dir %S --stop-after preprocess > %t.pre

//XFAIL: *

pragma solidity ^0.8.10;


contract MultipleSlotStorageVariableTestcase {

    function readStorageVariable(bytes memory array, uint256 index) external view returns (bytes1){
        return array[index];
    }

    function writeStorageVariable(bytes memory array, uint256 index, bytes1 value) external {
        array[index] = value;
    }
}