// SPDX-License-Identifier: MIT
// COM: RUN: pyul %s -o %t --project-dir %S | FileCheck %s
// COM: RUN: pyul %s --project-dir %S  | FileCheck %s

pragma solidity ^0.8.10;


contract IfThenElseContract {

    function max(uint256 a, uint256 b) external returns (uint256) {
        if(a>b)
	  return a;
	else
	  return b;
    }
    
    
}

//CHECK: define {{.* @fun_max_.*}}
//CHECK: switch
//check: case