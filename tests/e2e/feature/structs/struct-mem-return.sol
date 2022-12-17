// SPDX-License-Identifier: MIT
/**
This testcase targets storage vaiable in same slot

 */

// RUN: pyul %s -o %t --project-dir %S & false | FileCheck %s
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
    function readStruct() external view returns (St memory){
        St memory st;
        st.b.a=1;
        return st;
    }

    function writeStruct(St memory st, uint256 v) external {
        st.a=v;
    }
}
