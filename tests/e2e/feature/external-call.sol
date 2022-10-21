// SPDX-License-Identifier: MIT
// RUN: pyul %s -o %t --project-dir %S | FileCheck %s
// XFAIL:*
// This will be unxfailed after abi_decode_ is implemented, this requires rets
// to be packed in tuple
pragma solidity ^0.8.10;



contract ExternalCallTest {
    uint256 x;
    function add(address payable addr) external returns (uint256) {
        return x + CalleeContract(addr).add1(10, x);
    }
}

contract CalleeContract{
    function add1(uint256 x, uint256 y) external payable returns (uint256){
        return x+y+1;
    }
}

//CHECK: define i256 @fun_add_
//CHECK: {{call i256 @ext_fun_.*\(i256\*.*\)}}