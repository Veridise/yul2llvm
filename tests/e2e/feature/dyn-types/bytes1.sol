// SPDX-License-Identifier: MIT
/**
This testcase targets storage vaiable in different slot

 */

// RUN: pyul %s --project-dir %S --stop-after preprocess > %t.pre && false

//XFAIL: *

pragma solidity ^0.8.10;


contract StorageBytes1TestCase {
    bytes1 x;
    bytes1 y;

    function readStorageVariable() external view returns (bytes1){
        return x&y;
    }

    function writeStorageVariable(bytes1 a) external {
        x=a;
    }
}