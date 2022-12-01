// RUN: pyul %s -o %t --project-dir %S
// XFAIL: *

// SPDX-License-Identifier: UNKNOWN

contract LowLevelCallTest {
    function foo() payable public {
        msg.sender.call{value: 10}("");
    }
}
