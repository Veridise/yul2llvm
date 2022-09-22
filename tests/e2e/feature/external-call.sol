// SPDX-License-Identifier: MIT
// RUN: pyul %s -o %t --project-dir %S | FileCheck %s
// XFAIL: *
pragma solidity ^0.8.10;



contract ExternalCallTest {
    uint256 x;
    function add(address payable addr) external returns (uint256) {
        return x+ CalleeContract(addr).add1{value:10}(10);
    }
}
        // addr.transfer(10);

contract CalleeContract{
    function add1(uint256 x) external payable returns (uint256){
        return x+1;
    }
}