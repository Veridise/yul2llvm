// SPDX-License-Identifier: MIT
// RUN: pyul %s -o %t --project-dir %S | FileCheck %s
//
// xfail due to crash in C++ component
// XFAIL: *

pragma solidity ^0.8.10;

contract AddressTransfer {
    function sendMoney() external payable {
        payable(msg.sender).transfer(1);
        payable(msg.sender).send(1);
    }
}
