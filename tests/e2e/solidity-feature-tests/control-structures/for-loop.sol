// SPDX-License-Identifier: MIT

 // RUN: pyul %s -o %t --project-dir %S | FileCheck %s

pragma solidity ^0.8.10;


contract IfThenElseContract {
    uint256 x;

    constructor(){
        // x=100;
    }

    function loop(uint256 a) external returns (uint256) {
        
        for(uint256 i=0;i<10;i++)
	  a=a+1;
	
	return a;
    }

    function loop(uint256 a, uint256 b) external returns (uint256) {
        
        for(uint256 i=0;i<10;i++)
	  a=a+1;
	
	return b+1;
    }
}

//CHECK: for-cond
//CHECK: for-body
//CHECK: for-incr