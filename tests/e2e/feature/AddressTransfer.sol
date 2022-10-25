// SPDX-License-Identifier: MIT
// RUN: pyul %s -o %t --project-dir %S | FileCheck %s


pragma solidity ^0.8.10;

contract AddressTransfer {
    function sendMoney() external payable {
        payable(msg.sender).transfer(1);
        payable(msg.sender).send(1);
    }
}


//CHECK: %{{gasPtr.*}} = getelementptr %ExtCallContextType, %ExtCallContextType* %{{.*}}, i32 0, i32 0
//CHECK: store i256 %{{.*}}, i256* %{{gasPtr.*}}, align 4
//CHECK: %{{addrPtr.*}} = getelementptr %ExtCallContextType, %ExtCallContextType* %{{.*}}, i32 0, i32 1
//CHECK: store i256 %{{.*}}, i256* %{{addrPtr.*}}, align 4
//CHECK: %{{valuePtr.*}} = getelementptr %ExtCallContextType, %ExtCallContextType* %{{.*}}, i32 0, i32 2
//CHECK: store i256 {{.*}}, i256* %{{valuePtr.*}}, align 4
//CHECK: %{{bufferPtr.*}} = getelementptr %ExtCallContextType, %ExtCallContextType* %{{.*}}, i32 0, i32 3
//CHECK: store i256* %{{.*}}, i256** %{{bufferPtr.*}}, align 8
//CHECK: %{{retLenPtr.*}} = getelementptr %ExtCallContextType, %ExtCallContextType* %{{.*}}, i32 0, i32 4
//CHECK: store i256 {{.*}}, i256* %{{retLenPtr.*}}, align 4
//CHECK: %call_rv = call i256 @pyul_call_0x0(i256* %__self, %ExtCallContextType* %{{.*}})

//CHECK: %{{gasPtr.*}} = getelementptr %ExtCallContextType, %ExtCallContextType* %{{.*}}, i32 0, i32 0
//CHECK: store i256 %{{.*}}, i256* %{{gasPtr.*}}, align 4
//CHECK: %{{addrPtr.*}} = getelementptr %ExtCallContextType, %ExtCallContextType* %{{.*}}, i32 0, i32 1
//CHECK: store i256 %{{.*}}, i256* %{{addrPtr.*}}, align 4
//CHECK: %{{valuePtr.*}} = getelementptr %ExtCallContextType, %ExtCallContextType* %{{.*}}, i32 0, i32 2
//CHECK: store i256 {{.*}}, i256* %{{valuePtr.*}}, align 4
//CHECK: %{{bufferPtr.*}} = getelementptr %ExtCallContextType, %ExtCallContextType* %{{.*}}, i32 0, i32 3
//CHECK: store i256* %{{.*}}, i256** %{{bufferPtr.*}}, align 8
//CHECK: %{{retLenPtr.*}} = getelementptr %ExtCallContextType, %ExtCallContextType* %{{.*}}, i32 0, i32 4
//CHECK: store i256 {{.*}}, i256* %{{retLenPtr.*}}, align 4
//CHECK: %{{call_rv.*}} = call i256 @pyul_call_0x0(i256* %__self, %ExtCallContextType* %{{.*}})

