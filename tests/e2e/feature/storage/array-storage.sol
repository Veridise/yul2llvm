// SPDX-License-Identifier: MIT
/**
This testcase targets storage vaiable in same slot

 */

// RUN: pyul %s -o %t --project-dir %S | FileCheck %s

pragma solidity ^0.8.10;


contract ArrayTest {
    uint32 x;
    uint32 y;
    uint32[10][10] array;
    

    function readArray(uint256 index) external view returns (uint32){
        return array[index][0];
    }

    function writeArray(uint256 index, uint32 value) external {
        array[index][0] = value;
    }
}


//CHECK: define i256 @fun_readArray_25(i256 addrspace(1)* %__self, i256 %var_index_13)
//CHECK: %{{.*}} = trunc i256 %{{.*}} to i32
//CHECK: %{{.*}} = getelementptr %{{.*}}, %{{.*}}, i32 {{.*}}
//CHECK: %{{.*}} = load i256 addrspace(1)*, i256 addrspace(1)* addrspace(1)* %{{.*}}, align 8
//CHECK: bitcast {{.*}}* %{{.*}} to [0 x {{.*}}*] addrspace(1)*
//CHECK: getelementptr [0 x {{.*}}], [0 x {{.*}}*] addrspace(1)* %{{.*}}, i32 0, i32 %{{.*}}
//CHECK: {{(bitcast)|(inttoptr)}} {{.*}} %{{.*}} to i256 addrspace(1)* 
//CHECK: %{{.*}} = load i256 addrspace(1)*, i256 addrspace(1)* %{{.*}}, align 8
//CHECK: bitcast {{.*}}* %{{.*}} to [0 x {{.*}}] addrspace(1)*
//CHECK: getelementptr [0 x {{.*}}], [0 x {{.*}}] addrspace(1)* %{{.*}}, i32 0, i32 {{.*}}
//CHECK: ret i256 %read_from_storage_split_dynamic_t_uint32 

//CHECK: define void @fun_writeArray_{{.*}}(i256 addrspace(1)* %{{.*}}, {{.*}} %{{.*}}, {{.*}} %{{.*}}) 
//CHECK: %{{.*}} = trunc i256 %{{.*}} to i32
//CHECK: %{{.*}} = getelementptr %{{.*}}, %{{.*}}* %0, i32 {{.*}}
//CHECK: %{{.*}} = load i256 addrspace(1)*, i256 addrspace(1)* addrspace(1)* %{{.*}}, align 8
//CHECK: %1 = bitcast {{.*}}* %{{.*}} to [0 x {{.*}}*] addrspace(1)*
//CHECK: %"{{.*}}[{{.*}}]" = getelementptr [0 x {{.*}} addrspace(1)*], [0 x {{.*}} addrspace(1)*] addrspace(1)* %{{.*}}, i32 0, i32 %{{.*}}
//CHECK: %{{.*}} = load i256 addrspace(1)*, i256 addrspace(1)* %{{.*}}, align 8
//CHECK: bitcast {{.*}}* %{{.*}} to [0 x {{.*}}] addrspace(1)*
//CHECK: %"{{.*}}[{{.*}}]" = getelementptr [0 x {{.*}}], [0 x {{.*}}] addrspace(1)* %{{.*}}, i32 0, i32 {{.*}}
