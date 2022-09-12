
/// @use-src 0:"control-flow/if-then.sol"
object "IfThenTestContract_20" {
    code {
        /// @src 0:118:284  "contract IfThenTestContract {..."
        mstore(64, memoryguard(128))
        if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }

        constructor_IfThenTestContract_20()

        // adapted by Eurus: remove_post_constructor_datacall
        // let _1 := allocate_unbounded()
        // codecopy(_1, dataoffset("IfThenTestContract_20_deployed"), datasize("IfThenTestContract_20_deployed"))
        // return(_1, datasize("IfThenTestContract_20_deployed"))

        function allocate_unbounded() -> memPtr {
            memPtr := mload(64)
        }

        function revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() {
            revert(0, 0)
        }

        /// @src 0:118:284  "contract IfThenTestContract {..."
        function constructor_IfThenTestContract_20() {

            /// @src 0:118:284  "contract IfThenTestContract {..."

        }
        /// @src 0:118:284  "contract IfThenTestContract {..."

    }
    /// @use-src 0:"control-flow/if-then.sol"
    object "IfThenTestContract_20_deployed" {
        code {
            /// @src 0:118:284  "contract IfThenTestContract {..."
            mstore(64, memoryguard(128))

            if iszero(lt(calldatasize(), 4))
            {
                let selector := shift_right_224_unsigned(calldataload(0))
                switch selector

                case 0x6d5433e6
                {
                    // max(uint256,uint256)

                    external_fun_max_19()
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

            function abi_decode_tuple_t_uint256t_uint256(headStart, dataEnd) -> value0, value1 {
                if slt(sub(dataEnd, headStart), 64) { revert_error_dbdddcbe895c83990c08b3492a0e83918d802a52331272ac6fdb6a7c4aea3b1b() }

                {

                    let offset := 0

                    value0 := abi_decode_t_uint256(add(headStart, offset), dataEnd)
                }

                {

                    let offset := 32

                    value1 := abi_decode_t_uint256(add(headStart, offset), dataEnd)
                }

            }

            function abi_encode_t_uint256_to_t_uint256_fromStack(value, pos) {
                mstore(pos, cleanup_t_uint256(value))
            }

            function abi_encode_tuple_t_uint256__to_t_uint256__fromStack(headStart , value0) -> tail {
                tail := add(headStart, 32)

                abi_encode_t_uint256_to_t_uint256_fromStack(value0,  add(headStart, 0))

            }

            function external_fun_max_19() {

                if callvalue() { revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() }
                let param_0, param_1 :=  abi_decode_tuple_t_uint256t_uint256(4, calldatasize())
                let ret_0 :=  fun_max_19(param_0, param_1)
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

            /// @ast-id 19
            /// @src 0:153:272  "function max(uint256 a, uint256 b) external returns (uint256) {..."
            function fun_max_19(var_a_3, var_b_5) -> var__8 {
                /// @src 0:206:213  "uint256"
                let zero_t_uint256_1 := zero_value_for_split_t_uint256()
                var__8 := zero_t_uint256_1

                /// @src 0:228:229  "a"
                let _2 := var_a_3
                let expr_10 := _2
                /// @src 0:230:231  "b"
                let _3 := var_b_5
                let expr_11 := _3
                /// @src 0:228:231  "a>b"
                let expr_12 := gt(cleanup_t_uint256(expr_10), cleanup_t_uint256(expr_11))
                /// @src 0:225:250  "if(a>b)..."
                if expr_12 {
                    /// @src 0:249:250  "a"
                    let _4 := var_a_3
                    let expr_13 := _4
                    /// @src 0:242:250  "return a"
                    var__8 := expr_13
                    leave
                    /// @src 0:225:250  "if(a>b)..."
                }
                /// @src 0:264:265  "b"
                let _5 := var_b_5
                let expr_16 := _5
                /// @src 0:257:265  "return b"
                var__8 := expr_16
                leave

            }
            /// @src 0:118:284  "contract IfThenTestContract {..."

        }

        
    }

}

