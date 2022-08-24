// SPDX-License-Identifier: MIT
 // RUN: pyul %s -o %t --project-dir %S | FileCheck %s

pragma solidity ^0.8.10;


contract IfThenTestContract {

    function max(uint256 a, uint256 b) external returns (uint256) {
        if(a>b)
	  return a;
	return b;
    }
    
    
}

//CHECK: define {{.* @fun_max_.*}}
//CHECK: taken-body
//CHECK: not-taken-body