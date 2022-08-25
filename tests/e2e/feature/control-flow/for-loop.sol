// SPDX-License-Identifier: MIT

 // RUN: pyul %s -o %t --project-dir %S | FileCheck %s

pragma solidity ^0.8.10;


contract ForLoopTestContract {

    function loop(uint256 a) external returns (uint256) {
        for(uint256 i=0;i<10;i++)
            a=a+1;    
        return a;
    }
}

//CHECK: define {{.* @fun_loop_.*}}
//CHECK: for-cond
//CHECK: for-body
//CHECK: for-incr