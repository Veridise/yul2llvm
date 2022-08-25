// SPDX-License-Identifier: MIT
/**
This testcase targets storage vaiable in different slot

 */

// RUN: pyul %s --project-dir %S --stop-after preprocess > %t.pre

/////
// Storage vars should be contained in metadata

// RUN: jq '.["metadata"]["state_vars"]' -S < %t.pre \
// RUN:   | FileCheck %s --check-prefix META

// META-LABEL: "name": "x"
// META: "type": "t_uint256"

// META-LABEL: "name": "y"
// META: "type": "t_uint32"
/////

/////
// Storage reads should be rewritten

// RUN: jq '.. | select(.type? == "yul_function_call" and (.children[0].children[0] | startswith("pyul_"))) | .children[0].children[0]' -S < %t.pre \
// RUN:   | FileCheck %s --check-prefix PRE

// PRE: pyul_storage_var_load
/////

pragma solidity ^0.8.10;


contract MultipleSlotStorageVariableTestcase {
    uint256 x;
    uint32 y;

    function readStorageVariable() external view returns (uint256){
        return x+y;
    }

    function writeStorageVariable() external {
        x=10;
        y=20;
    }
}
