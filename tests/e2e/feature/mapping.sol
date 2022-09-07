// SPDX-License-Identifier: MIT

// RUN: pyul %s -o %t --project-dir %S | FileCheck %s
// XFAIL: *
pragma solidity ^0.8.10;


contract MappingTestCase {
    mapping(uint256 => uint256) public map;

    function mappingRead() external view returns (uint256) {
        return map[0];
    }

    function mappingWrite(uint256 k, uint256 v) external {
        map[k]=v;
    }
}

//CHECK: define {{fun_add_[0-9]+}}