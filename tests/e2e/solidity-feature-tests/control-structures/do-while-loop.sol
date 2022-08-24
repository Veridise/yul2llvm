// SPDX-License-Identifier: MIT

 // RUN: pyul %s -o %t --project-dir %S | FileCheck %s

pragma solidity ^0.8.10;


contract IfThenElseContract {

    constructor(){
    }

    function loop() external returns (uint256) {
        int i = 0;
	int a = 1;
       do{
	  a=a+1;
	  i=i+1;
	} while(i<10);
    }
    
    
}
//CHECK: {{_[0-9]+\w+}}={{\w+}}1
//CHECK: for-cond
//CHECK: for-body
//CHECK: for-incr