// SPDX-License-Identifier: MIT
pragma solidity ^0.8.10;
// RUN: pyul %s -o %t --project-dir %S | FileCheck %s
// XFAIL: *
contract StorageLocationMemoryTest {

    constructor(){
    }


    function echo() external returns(string memory) {
        return "Hello World!";
    }
    
    
}