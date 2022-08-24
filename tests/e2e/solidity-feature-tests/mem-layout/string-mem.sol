// SPDX-License-Identifier: MIT
pragma solidity ^0.8.10;
// RUN: pyul %s -o %t --project-dir %S | FileCheck %s
// XFAIL: *
contract AdditionContract {

    constructor(){
    }


    function add() external returns(string memory) {
        return "Hello World!";
    }
    
    
}