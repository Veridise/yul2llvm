// SPDX-License-Identifier: MIT
/**
This testcase targets storage vaiable in same slot

 */

// RUN: pyul %s -o %t --project-dir %S & false| FileCheck %s
//XFAIL: *
pragma solidity ^0.8.10;

contract StructTest {
    struct Sb {
        uint256 a;
    }
    struct St {
        uint256 a;
        Sb b;
    }
    function readStruct(St memory mem) external view returns (uint256){
        St memory ref;
        ref = mem;
        return ref.b.a;
    }
}

