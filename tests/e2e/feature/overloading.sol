// SPDX-License-Identifier: MIT

// RUN: pyul %s -o %t --project-dir %S | FileCheck %s
// XFAIL: *
pragma solidity ^0.8.10;


contract OverloadingTestContract {
    function echo(uint256 a) external returns (uint256) {
        return a;
    }
    function echo(uint256 a, uint256 b) external returns (uint256) {
	    return a+b+1;
    }
}

//CHECK: define {{.* @fun_loop_.*}}
