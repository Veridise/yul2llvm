// RUN: pyul %s -o %t --project-dir %S

// SPDX-License-Identifier: UNKNOWN
pragma solidity ^0.8.15;

abstract contract MyAbstractContract {
    function myVirtualFn() public virtual;

    function myConcreteFn() public view returns (uint256) {
        return block.timestamp + 10;
    }
}

contract MyConcreteContract is MyAbstractContract {
    function myVirtualFn() public override {}
}
