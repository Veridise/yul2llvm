// SPDX-License-Identifier: MIT
/**
This testcase targets storage vaiable in same slot

 */

// RUN: pyul %s -o %t --project-dir %S & false | FileCheck %s
//XFAIL: *
// Wont work because copying mem to storage does not work
pragma solidity ^0.8.10;

contract StructTest {
    struct Sb {
        uint256 a;
    }
    struct St {
        uint256 a;
        Sb b;
    }
    St st;
    function readStruct(St memory mem) external view returns (uint256){
        St storage ref = st ;
        ref.a = mem;
        return ref.b.a;
    }

    function writeStruct(uint256 v) external {
        st.a=v;
    }
}
