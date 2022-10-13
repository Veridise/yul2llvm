// SPDX-License-Identifier: MIT

// RUN: pyul %s -o %t --project-dir %S | FileCheck %s
pragma solidity ^0.8.10;


contract NestedMappingTestCase {
    mapping(uint256 => mapping(uint256 => uint32)) public map;

    function mappingRead(uint256 x, uint256 y) external view returns (uint32) {
        return map[x][y];
    }

    function mappingWrite(uint256 k1, uint256 k2, uint32 v) external {
        map[k1][k2] = v;
    }
}

//CHECK: {{define i256 @fun_mappingRead_[0-9]+\(i256\* \%.+, .+, .+\)}}
//CHECK: {{call i256\* @pyul_map_index\(i256\* \%.+, .+\)}}
//CHECK: {{call i256\* @pyul_map_index\(i256\* \%.+, .+\)}}
//CHECK: {{load i256, i256\*}}


//CHECK: {{define void @fun_mappingWrite_[0-9]+\(i256\* \%.+, .+, .+, .+\)}}
//CHECK: {{call i256\* @pyul_map_index\(i256\* \%.+, .+\)}}
//CHECK: {{call i256\* @pyul_map_index\(i256\* \%.+, .+\)}}
//CHECK: {{store i256 .+, i256\*.*}}