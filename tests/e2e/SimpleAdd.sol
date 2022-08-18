// RUN: pyul all %s -o "${fs-tmp-root}"
// RUN: yul2llvm_cpp "${fs-tmp-root}"/"$(basename "%s" | cut -f 1 -d '.')"/"$(basename "%s" | cut -f 1 -d '.')".json -o "%t"
// RUN: cat %t | FileCheck %s

// TODO: clean up pyul so it prints the json to stdout


// SPDX-License-Identifier: MIT
pragma solidity ^0.8.10;


contract AdditionContract {
    uint256 x;

    constructor() {}

    function add(uint256 a, uint256 b) external returns (uint256) {
        return a+b;
    }
    
}

// CHECK-LABEL: define {{.*}} @fun_add_{{.*}}({{.*}}) {
// CHECK: %{{.*}} = add
// CHECK: }
