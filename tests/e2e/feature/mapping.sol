// SPDX-License-Identifier: MIT

// RUN: pyul %s -o %t --project-dir %S | FileCheck %s
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

//CHECK: {{define i256 @fun_mappingRead_[0-9]+\(i256 addrspace\(1\)\* \%__self.*\)}}
//CHECK: {{call i256\* @pyul_map_index\(i256\* \%.+, .+\)}}
//CHECK: {{load i256, i256\*}}


//CHECK: {{define void @fun_mappingWrite_[0-9]+\(i256 addrspace\(1\)\* \%__self.*, .+, .+\)}}
//CHECK: {{call i256\* @pyul_map_index\(i256\* \%.+, .+\)}}
//CHECK: {{store i256 .+, i256\*.*}}