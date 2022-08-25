// SPDX-License-Identifier: MIT
/**
This testcase targets storage vaiable in different slot

 */


// pyul %s -o %t --project-dir %S | FileCheck %s

// RUN: pyul %s --project-dir %S --stop-after preprocess \
// RUN:   | jq '.["metadata"]["state_vars"]' -S \
// RUN:   | FileCheck %s --check-prefix PRE

// PRE-LABEL: "name": "x"
// PRE: "type": "t_uint256"

// PRE-LABEL: "name": "y"
// PRE: "type": "t_uint32"

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
