// SPDX-License-Identifier: MIT
// RUN: pyul %s -o %t --project-dir %S | FileCheck %s

pragma solidity ^0.8.10;


contract AdditionContract {
    function add(int a, int b) external returns (int) {
        if(a>-2)
            return 0;
        return a+b;
    }
}

//CHECK: define {{.*fun_add_[0-9]+}}