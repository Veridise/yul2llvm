// RUN: pyul %s -o %t --project-dir %S | FileCheck %s
// XFAIL: *

pragma solidity ^0.8.0;

contract ReturnString {
    string private _name;

    constructor(string memory name_) {
        _name = name_;
    }

    function name() public view virtual returns (string memory) {
        return _name;
    }
}
