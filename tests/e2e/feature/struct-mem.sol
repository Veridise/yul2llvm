// SPDX-License-Identifier: MIT
/**
This testcase targets storage vaiable in same slot

 */

// RUN: pyul %s -o %t --project-dir %S | FileCheck %s
//XFAIL: *
pragma solidity ^0.8.10;

contract StructTest {
    struct St {
        uint256 a;
    }
    function readStruct(St memory st) external view returns (uint256){
        return st.a;
    }

    function writeStruct(St memory st, uint256 v) external {
        st.a=v;
    }
}

//CHECK: define i256 @fun_readArray_{{[0-9]+}}(i256 addrspace(1)* %__self, {{.+, .+}})
//CHECK: getelementptr [0 x {{.*}}], [0 x {{.*}}]*
//CHECK: arr_load{{.*}} = load i32, i32*
//CHECK: %word_arr_load = zext i32


//CHECK: define void @fun_writeArray_{{[0-9]+}}(i256 addrspace(1)* %__self, {{.+, .+, .+}})
//CHECK: getelementptr [0 x {{.*}}], [0 x {{.*}}]*
//CHECK: store i32 {{.*}}, i32*
