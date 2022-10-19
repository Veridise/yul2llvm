// SPDX-License-Identifier: MIT

// RUN: pyul %s -o %t --project-dir %S | FileCheck %s
// XFAIL: *
pragma solidity ^0.8.10;


contract Base{
    uint256 baseMember;
    function baseFunction() external view returns(uint256){
        return baseMember;
    }
}

contract Derived is Base {
    uint256 derivedMember;
    function derivedFunction() external view returns(uint256){
        return derivedMember;
    }
}