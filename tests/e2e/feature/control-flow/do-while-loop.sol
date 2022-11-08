// SPDX-License-Identifier: MIT

 // RUN: pyul %s -o %t --project-dir %S | FileCheck %s

pragma solidity ^0.8.10;


contract DoWhileTestContract {

    function loop() external returns (uint256) {
        int i = 0;
	    int a = 1;
        do{
            a=a+1;
            i=i+1;
	    } while(i<10);
    }
    
    
}
//CHECK: define {{.* @fun_loop_.*\(i256 addrspace\(1\)\*\ %__self\)}}
//CHECK: for-cond
//CHECK: for-body
//CHECK: for-incr