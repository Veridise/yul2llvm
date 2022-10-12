// SPDX-License-Identifier: MIT

pragma solidity ^0.8.0;

import "./subdir/IMyInterface.sol";

contract MainContract {
    MyInterface private _iface;

    constructor(address iface) {
        _iface = MyInterface(iface);
    }
}
