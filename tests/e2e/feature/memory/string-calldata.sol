// SPDX-License-Identifier: MIT
// RUN: pyul %s -o %t --project-dir %S | FileCheck %s
// XFAIL: *
pragma solidity ^0.8.10;


contract StorageLocationCallDataTest {
    function echo(string calldata a) external returns(string  calldata) {
        return a;
    }
    
    
}