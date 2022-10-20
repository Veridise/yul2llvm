// SPDX-License-Identifier: MIT

// RUN: pyul %s -o %t --project-dir %S | FileCheck %s
pragma solidity ^0.8.10;

library ExampleLibrary {
    function myAdd(uint a, uint b) external returns (uint) {
        return a + b;
    }
}

// CHECK: define i256 @fun_myAdd_{{[0-9]+}}({{[^)]+}}) {
