// SPDX-License-Identifier: MIT

// RUN: pyul %s -o %t --project-dir %S | FileCheck %s
// XFAIL: *
pragma solidity ^0.8.10;


contract Base{
    uint256 baseMember;
    function baseFunction() external view return(uint256){
        return baseMember;
    }
}

contract Derived is base {
    uint256 derivedMember;
    function derivedFunction() external view return(uint256){
        return derivedMember;
    }
}