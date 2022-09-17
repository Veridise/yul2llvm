// SPDX-License-Identifier: MIT
// RUN: pyul %s -o %t --project-dir %S | FileCheck %s
// XFAIL: *
pragma solidity ^0.8.10;



contract ExternalCallTest {
    function add(address payable addr) external returns (uint256) {
        ExtCont(addr).foo{value:10}();
    }
}
        // addr.transfer(10);

interface ExtCont{
    function foo() external payable;
}