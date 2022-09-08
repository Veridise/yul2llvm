// RUN: pyul %s -o %t --project-dir %S -d| FileCheck %s

// SPDX-License-Identifier: MIT
pragma solidity ^0.8.10;


contract AdditionContract {
    uint256 x;

    constructor() {}

    function add(uint256 a, uint256 b) external returns (uint256) {
        return a+b;
    }
    
}

// CHECK-LABEL: define {{.*}} @_pyul_selector({{.*}}) {
// CHECK: }

// CHECK-LABEL: define {{.*}} @fun_add_{{.*}}({{.*}}) {
// CHECK: %{{.*}} = add
// CHECK: }
