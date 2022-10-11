// SPDX-License-Identifier: MIT
/**
This testcase targets storage vaiable in different slot

 */

// RUN: pyul %s --project-dir %S --stop-after preprocess > %t.pre && | 

//XFAIL: *

pragma solidity ^0.8.10;


contract BytesMemoryTestcase {

    function readBytes(bytes memory array, uint256 index) external view returns (bytes1){
        return array[index];
    }

    function writeBytes(bytes memory array, uint256 index, bytes1 value) external {
        array[index] = value;
    }
}