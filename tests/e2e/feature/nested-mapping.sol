// SPDX-License-Identifier: MIT

// RUN: pyul %s -o %t --project-dir %S | FileCheck %s
// XFAIL: *
pragma solidity ^0.8.10;


contract NestedMappingTestCase {
    mapping(uint256 => mapping(uint256 => uint256)) public map;

    function mappingRead() external view returns (uint256) {
        return map[0][0];
    }

    function mappingWrite(uint256 k1, uint256 k2, uint256 v) external {
        map[k1][k2] = v;
    }
}

//CHECK: define {{fun_add_[0-9]+}}