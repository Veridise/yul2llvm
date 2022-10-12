// SPDX-License-Identifier: MIT

// RUN: pyul %s -o %t --project-dir %S | FileCheck %s
// XFAIL: *
pragma solidity ^0.8.10;

library ExampleLibrary {
    function myAdd(uint a, uint b) external returns (uint) {
        return a + b;
    }
}

// CHECK: define {{fun_myAdd_[0-9]+}}
