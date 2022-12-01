// RUN: pyul %s -o %t --project-dir %S | FileCheck %s
// XFAIL: *

// SPDX-License-Identifier: MIT
pragma solidity ^0.8.10;


contract StorageLocationCallDataTest {
    function echo(string calldata a) external returns(string  calldata) {
        return a;
    }
    
    
}

// CHECK-LABEL: define i256* @fun_echo_{{.*}}({{.*}})

// TODO: this currently returns an i256. Shouldn't it be i256* or some sort of struct/array type?
