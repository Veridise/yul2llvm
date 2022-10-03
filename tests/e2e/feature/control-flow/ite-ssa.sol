// SPDX-License-Identifier: MIT
// COM: RUN: pyul %s -o %t --project-dir %S -d| FileCheck %s
// COM: RUN: pyul %s --project-dir %S -d | FileCheck %s

pragma solidity ^0.8.10;


contract IfThenElseContract {

  function max(uint256 a, uint256 b) external returns (uint256) {
    if(a>b)
      a=10;
    else
      a= b;
    return a;
  }
    
    
}

//CHECK: define {{.* @fun_max_.*\(i256\* \%.*\)}}
//CHECK: switch
//CHECK: case
//CHECK: default