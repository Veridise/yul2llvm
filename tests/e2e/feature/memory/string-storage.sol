// RUN: pyul %s -o %t --project-dir %S | FileCheck %s
// XFAIL: *

// SPDX-License-Identifier: MIT
pragma solidity ^0.8.10;
contract StorageLocationMemoryTest {
    string str;
    constructor(){
        str = "hellhellhellhellhellhellhellhellh";
    }


    function echo() external returns(string memory) {
        return str;
    }
    
    
}

// CHECK-LABEL: define i256* @fun_echo_{{.*}}({{.*}})

// TODO: this currently returns an i256. Shouldn't it be i256* or some sort of struct/array type?
