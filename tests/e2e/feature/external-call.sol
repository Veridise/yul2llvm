// SPDX-License-Identifier: MIT
// RUN: pyul %s -o %t --project-dir %S | FileCheck %s
// XFAIL: *
pragma solidity ^0.8.10;

interface ExtCont{
    function foo() external payable;
}

contract ExternalCallTest {
    function add(address addr) external returns (uint256) {
        ExtCont(addr).foo{value:10}();
    }
}