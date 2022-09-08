// SPDX-License-Identifier: MIT
// RUN: pyul %s -o %t --project-dir %S -d | FileCheck %s

pragma solidity ^0.8.10;


contract NegativeLiteralTest {
    function decrement(int a) external returns (int) {
        int diff = -0xa;
        return a+ diff;
    }
}

//CHECK: define {{.*fun_decrement_[0-9]+}}
//CHECK: i256 -10