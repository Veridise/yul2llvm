
/// @use-src 0:"control-flow/for-loop.sol"
object "ForLoopTestContract_29" {
    code {
        /// @src 0:119:291  "contract ForLoopTestContract {..."
        mstore(64, memoryguard(128))
        if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }

        constructor_ForLoopTestContract_29()

        // adapted by Eurus: remove_post_constructor_datacall
        // let _1 := allocate_unbounded()
        // codecopy(_1, dataoffset("ForLoopTestContract_29_deployed"), datasize("ForLoopTestContract_29_deployed"))
        // return(_1, datasize("ForLoopTestContract_29_deployed"))

        function allocate_unbounded() -> memPtr {
            memPtr := mload(64)
        }

        function revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() {
            revert(0, 0)
        }

        /// @src 0:119:291  "contract ForLoopTestContract {..."
        function constructor_ForLoopTestContract_29() {

            /// @src 0:119:291  "contract ForLoopTestContract {..."

        }
        /// @src 0:119:291  "contract ForLoopTestContract {..."

    }
    /// @use-src 0:"control-flow/for-loop.sol"
    object "ForLoopTestContract_29_deployed" {
        code {
            /// @src 0:119:291  "contract ForLoopTestContract {..."
            mstore(64, memoryguard(128))

            if iszero(lt(calldatasize(), 4))
            {
                let selector := shift_right_224_unsigned(calldataload(0))
                switch selector

                case 0x0b7d796e
                {
                    // loop(uint256)

                    external_fun_loop_28()
                }

                default {
                    // adapted by Eurus: hijack_dispatcher_miss
                    eurus_dispatcher_miss(selector)
                }
            }

            revert_error_42b3090547df1d2001c96683413b8cf91c1b902ef5e3cb8d9f6f304cf7446f74()

            function shift_right_224_unsigned(value) -> newValue {
                newValue :=

                shr(224, value)

            }

            function allocate_unbounded() -> memPtr {
                memPtr := mload(64)
            }

            function revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() {
                revert(0, 0)
            }

            function revert_error_dbdddcbe895c83990c08b3492a0e83918d802a52331272ac6fdb6a7c4aea3b1b() {
                revert(0, 0)
            }

            function revert_error_c1322bf8034eace5e0b5c7295db60986aa89aae5e0ea0873e4689e076861a5db() {
                revert(0, 0)
            }

            function cleanup_t_uint256(value) -> cleaned {
                cleaned := value
            }

            function validator_revert_t_uint256(value) {
                if iszero(eq(value, cleanup_t_uint256(value))) { revert(0, 0) }
            }

            function abi_decode_t_uint256(offset, end) -> value {
                value := calldataload(offset)
                validator_revert_t_uint256(value)
            }

            function abi_decode_tuple_t_uint256(headStart, dataEnd) -> value0 {
                if slt(sub(dataEnd, headStart), 32) { revert_error_dbdddcbe895c83990c08b3492a0e83918d802a52331272ac6fdb6a7c4aea3b1b() }

                {

                    let offset := 0

                    value0 := abi_decode_t_uint256(add(headStart, offset), dataEnd)
                }

            }

            function abi_encode_t_uint256_to_t_uint256_fromStack(value, pos) {
                mstore(pos, cleanup_t_uint256(value))
            }

            function abi_encode_tuple_t_uint256__to_t_uint256__fromStack(headStart , value0) -> tail {
                tail := add(headStart, 32)

                abi_encode_t_uint256_to_t_uint256_fromStack(value0,  add(headStart, 0))

            }

            function external_fun_loop_28() {

                if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }
                let param_0 :=  abi_decode_tuple_t_uint256(4, calldatasize())
                let ret_0 :=  fun_loop_28(param_0)
                let memPos := allocate_unbounded()
                let memEnd := abi_encode_tuple_t_uint256__to_t_uint256__fromStack(memPos , ret_0)
                return(memPos, sub(memEnd, memPos))

            }

            function revert_error_42b3090547df1d2001c96683413b8cf91c1b902ef5e3cb8d9f6f304cf7446f74() {
                // adapted by Eurus: hijack_dispatcher_revert
                eurus_dispatcher_revert()
            }

            function zero_value_for_split_t_uint256() -> ret {
                ret := 0
            }

            function cleanup_t_rational_0_by_1(value) -> cleaned {
                cleaned := value
            }

            function identity(value) -> ret {
                ret := value
            }

            function convert_t_rational_0_by_1_to_t_uint256(value) -> converted {
                converted := cleanup_t_uint256(identity(cleanup_t_rational_0_by_1(value)))
            }

            function panic_error_0x11() {
                mstore(0, 35408467139433450592217433187231851964531694900788300625387963629091585785856)
                mstore(4, 0x11)
                revert(0, 0x24)
            }

            function increment_t_uint256(value) -> ret {
                value := cleanup_t_uint256(value)
                if eq(value, 0xffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff) { panic_error_0x11() }
                ret := add(value, 1)
            }

            function cleanup_t_rational_10_by_1(value) -> cleaned {
                cleaned := value
            }

            function convert_t_rational_10_by_1_to_t_uint256(value) -> converted {
                converted := cleanup_t_uint256(identity(cleanup_t_rational_10_by_1(value)))
            }

            function cleanup_t_rational_1_by_1(value) -> cleaned {
                cleaned := value
            }

            function convert_t_rational_1_by_1_to_t_uint256(value) -> converted {
                converted := cleanup_t_uint256(identity(cleanup_t_rational_1_by_1(value)))
            }

            function checked_add_t_uint256(x, y) -> sum {
                x := cleanup_t_uint256(x)
                y := cleanup_t_uint256(y)

                // overflow, if x > (maxValue - y)
                if gt(x, sub(0xffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff, y)) { panic_error_0x11() }

                sum := add(x, y)
            }

            /// @ast-id 28
            /// @src 0:155:289  "function loop(uint256 a) external returns (uint256) {..."
            function fun_loop_28(var_a_3) -> var__6 {
                /// @src 0:198:205  "uint256"
                let zero_t_uint256_1 := zero_value_for_split_t_uint256()
                var__6 := zero_t_uint256_1

                /// @src 0:217:260  "for(uint256 i=0;i<10;i++)..."
                for {
                    /// @src 0:231:232  "0"
                    let expr_10 := 0x00
                    /// @src 0:221:232  "uint256 i=0"
                    let var_i_9 := convert_t_rational_0_by_1_to_t_uint256(expr_10)
                    } 1 {
                    /// @src 0:238:241  "i++"
                    let _3 := var_i_9
                    let _2 := increment_t_uint256(_3)
                    var_i_9 := _2
                    let expr_16 := _3
                }
                {
                    /// @src 0:233:234  "i"
                    let _4 := var_i_9
                    let expr_12 := _4
                    /// @src 0:235:237  "10"
                    let expr_13 := 0x0a
                    /// @src 0:233:237  "i<10"
                    let expr_14 := lt(cleanup_t_uint256(expr_12), convert_t_rational_10_by_1_to_t_uint256(expr_13))
                    if iszero(expr_14) { break }
                    /// @src 0:257:258  "a"
                    let _5 := var_a_3
                    let expr_19 := _5
                    /// @src 0:259:260  "1"
                    let expr_20 := 0x01
                    /// @src 0:257:260  "a+1"
                    let expr_21 := checked_add_t_uint256(expr_19, convert_t_rational_1_by_1_to_t_uint256(expr_20))

                    /// @src 0:255:260  "a=a+1"
                    var_a_3 := expr_21
                    let expr_22 := expr_21
                }
                /// @src 0:281:282  "a"
                let _6 := var_a_3
                let expr_25 := _6
                /// @src 0:274:282  "return a"
                var__6 := expr_25
                leave

            }
            /// @src 0:119:291  "contract ForLoopTestContract {..."

        }

        
    }

}

