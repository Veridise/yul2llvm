// SPDX-License-Identifier: MIT
// RUN: pyul %s -o %t --project-dir %S | FileCheck %s

pragma solidity ^0.8.10;


contract IfThenElseContract {
    uint256 x;

    constructor(){
        // x=100;
    }

    function max(uint256 a, uint256 b) external returns (uint256) {
        if(a>b)
	  return a;
	else
	  return b;
    }
    
    
}

//CHECK: switch
//check: case