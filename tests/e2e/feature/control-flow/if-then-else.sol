// SPDX-License-Identifier: MIT
// COM: RUN: pyul %s -o %t --project-dir %S | FileCheck %s
// COM: RUN: pyul %s --project-dir %S  | FileCheck %s

pragma solidity ^0.8.10;


contract IfThenElseContract {

  function max(uint256 a, uint256 b) external returns (uint256) {
    uint256 r;
    if(a>b)
      r = a;
    else
      r = b;
    
    return r;    
  }
}

//CHECK: define {{.* @fun_max_.*\(i256 addrspace\(1\)\* \%__self.*\)}}
//CHECK: switch
//check: case