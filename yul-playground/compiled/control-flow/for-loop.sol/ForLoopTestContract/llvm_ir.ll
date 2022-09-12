; ModuleID = 'yul'
source_filename = "yul"

define void @_pyul_selector() {
entry:
  %selector = alloca i256, align 8
  %calldatasize = call i256 @calldatasize()
  %lt = call i256 @lt(i256 %calldatasize, i256 4)
  %iszero = call i256 @iszero(i256 %lt)
  %0 = trunc i256 %iszero to i1
  br i1 %0, label %if-taken-body, label %if-not-taken-body

if-taken-body:                                    ; preds = %entry
  %calldataload = call i256 @calldataload(i256 0)
  %shift_right_224_unsigned = call i256 @shift_right_224_unsigned(i256 %calldataload)
  store i256 %shift_right_224_unsigned, i256* %selector, align 4
  %selector1 = load i256, i256* %selector, align 4
  switch i256 %selector1, label %default [
    i256 192772462, label %"0xB7D796E-case"
  ]

"0xB7D796E-case":                                 ; preds = %if-taken-body
  %external_fun_loop_28 = call i256 @external_fun_loop_28()
  br label %-switch-cont

default:                                          ; preds = %if-taken-body
  %selector2 = load i256, i256* %selector, align 4
  %eurus_dispatcher_miss = call i256 @eurus_dispatcher_miss(i256 %selector2)
  br label %-switch-cont

-switch-cont:                                     ; preds = %default, %"0xB7D796E-case"
  br label %if-not-taken-body

if-not-taken-body:                                ; preds = %-switch-cont, %entry
  %revert_error_42b3090547df1d2001c96683413b8cf91c1b902ef5e3cb8d9f6f304cf7446f74 = call i256 @revert_error_42b3090547df1d2001c96683413b8cf91c1b902ef5e3cb8d9f6f304cf7446f74()
  ret void
}

declare i256 @iszero(i256)

declare i256 @lt(i256, i256)

declare i256 @calldatasize()

declare i256 @calldataload(i256)

declare i256 @eurus_dispatcher_miss(i256)

define i256 @shift_right_224_unsigned(i256 %value) {
entry:
  %newValue = alloca i256, align 8
  %value_arg = alloca i256, align 8
  store i256 %value, i256* %value_arg, align 4
  %value1 = load i256, i256* %value_arg, align 4
  %shr = call i256 @shr(i256 224, i256 %value1)
  store i256 %shr, i256* %newValue, align 4
  %0 = load i256, i256* %newValue, align 4
  ret i256 %0
}

declare i256 @shr(i256, i256)

define i256 @allocate_unbounded() {
entry:
  %memPtr = alloca i256, align 8
  %mload = call i256 @mload(i256 64)
  store i256 %mload, i256* %memPtr, align 4
  %0 = load i256, i256* %memPtr, align 4
  ret i256 %0
}

declare i256 @mload(i256)

define void @revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb() {
entry:
  call void @revert()
  ret void
}

declare void @revert()

define void @revert_error_dbdddcbe895c83990c08b3492a0e83918d802a52331272ac6fdb6a7c4aea3b1b() {
entry:
  call void @revert()
  ret void
}

define i256 @cleanup_t_uint256(i256 %value) {
entry:
  %cleaned = alloca i256, align 8
  %value_arg = alloca i256, align 8
  store i256 %value, i256* %value_arg, align 4
  %value1 = load i256, i256* %value_arg, align 4
  store i256 %value1, i256* %cleaned, align 4
  %0 = load i256, i256* %cleaned, align 4
  ret i256 %0
}

define void @validator_revert_t_uint256(i256 %value) {
entry:
  %value_arg = alloca i256, align 8
  store i256 %value, i256* %value_arg, align 4
  %value1 = load i256, i256* %value_arg, align 4
  %value2 = load i256, i256* %value_arg, align 4
  %cleanup_t_uint256 = call i256 @cleanup_t_uint256(i256 %value2)
  %eq = call i256 @eq(i256 %value1, i256 %cleanup_t_uint256)
  %iszero = call i256 @iszero(i256 %eq)
  %0 = trunc i256 %iszero to i1
  br i1 %0, label %if-taken-body, label %if-not-taken-body

if-taken-body:                                    ; preds = %entry
  call void @revert()
  br label %if-not-taken-body

if-not-taken-body:                                ; preds = %if-taken-body, %entry
  ret void
}

declare i256 @eq(i256, i256)

define i256 @abi_decode_t_uint256(i256 %offset, i256 %end) {
entry:
  %value = alloca i256, align 8
  %end_arg = alloca i256, align 8
  %offset_arg = alloca i256, align 8
  store i256 %offset, i256* %offset_arg, align 4
  store i256 %end, i256* %end_arg, align 4
  %offset1 = load i256, i256* %offset_arg, align 4
  %calldataload = call i256 @calldataload(i256 %offset1)
  store i256 %calldataload, i256* %value, align 4
  %value2 = load i256, i256* %value, align 4
  call void @validator_revert_t_uint256(i256 %value2)
  %0 = load i256, i256* %value, align 4
  ret i256 %0
}

define i256 @abi_decode_tuple_t_uint256(i256 %headStart, i256 %dataEnd) {
entry:
  %offset = alloca i256, align 8
  %value0 = alloca i256, align 8
  %dataEnd_arg = alloca i256, align 8
  %headStart_arg = alloca i256, align 8
  store i256 %headStart, i256* %headStart_arg, align 4
  store i256 %dataEnd, i256* %dataEnd_arg, align 4
  %dataEnd1 = load i256, i256* %dataEnd_arg, align 4
  %headStart2 = load i256, i256* %headStart_arg, align 4
  %sub = call i256 @sub(i256 %dataEnd1, i256 %headStart2)
  %slt = call i256 @slt(i256 %sub, i256 32)
  %0 = trunc i256 %slt to i1
  br i1 %0, label %if-taken-body, label %if-not-taken-body

if-taken-body:                                    ; preds = %entry
  call void @revert_error_dbdddcbe895c83990c08b3492a0e83918d802a52331272ac6fdb6a7c4aea3b1b()
  br label %if-not-taken-body

if-not-taken-body:                                ; preds = %if-taken-body, %entry
  store i256 0, i256* %offset, align 4
  %headStart3 = load i256, i256* %headStart_arg, align 4
  %offset4 = load i256, i256* %offset, align 4
  %add = call i256 @add(i256 %headStart3, i256 %offset4)
  %dataEnd5 = load i256, i256* %dataEnd_arg, align 4
  %abi_decode_t_uint256 = call i256 @abi_decode_t_uint256(i256 %add, i256 %dataEnd5)
  store i256 %abi_decode_t_uint256, i256* %value0, align 4
  %1 = load i256, i256* %value0, align 4
  ret i256 %1
}

declare i256 @slt(i256, i256)

declare i256 @sub(i256, i256)

declare i256 @add(i256, i256)

define void @abi_encode_t_uint256_to_t_uint256_fromStack(i256 %value, i256 %pos) {
entry:
  %pos_arg = alloca i256, align 8
  %value_arg = alloca i256, align 8
  store i256 %value, i256* %value_arg, align 4
  store i256 %pos, i256* %pos_arg, align 4
  %pos1 = load i256, i256* %pos_arg, align 4
  %value2 = load i256, i256* %value_arg, align 4
  %cleanup_t_uint256 = call i256 @cleanup_t_uint256(i256 %value2)
  %mstore = call i256 @mstore(i256 %pos1, i256 %cleanup_t_uint256)
  ret void
}

declare i256 @mstore(i256, i256)

define i256 @abi_encode_tuple_t_uint256__to_t_uint256__fromStack(i256 %headStart, i256 %value0) {
entry:
  %tail = alloca i256, align 8
  %value0_arg = alloca i256, align 8
  %headStart_arg = alloca i256, align 8
  store i256 %headStart, i256* %headStart_arg, align 4
  store i256 %value0, i256* %value0_arg, align 4
  %headStart1 = load i256, i256* %headStart_arg, align 4
  %add = call i256 @add(i256 %headStart1, i256 32)
  store i256 %add, i256* %tail, align 4
  %value02 = load i256, i256* %value0_arg, align 4
  %headStart3 = load i256, i256* %headStart_arg, align 4
  %add4 = call i256 @add(i256 %headStart3, i256 0)
  call void @abi_encode_t_uint256_to_t_uint256_fromStack(i256 %value02, i256 %add4)
  %0 = load i256, i256* %tail, align 4
  ret i256 %0
}

define void @external_fun_loop_28() {
entry:
  %memEnd = alloca i256, align 8
  %memPos = alloca i256, align 8
  %ret_0 = alloca i256, align 8
  %param_0 = alloca i256, align 8
  %callvalue = call i256 @callvalue()
  %0 = trunc i256 %callvalue to i1
  br i1 %0, label %if-taken-body, label %if-not-taken-body

if-taken-body:                                    ; preds = %entry
  call void @revert_error_ca66f745a3ce8ff40e2ccaf1ad45db7774001b90d25810abd9040049be7bf4bb()
  br label %if-not-taken-body

if-not-taken-body:                                ; preds = %if-taken-body, %entry
  %calldatasize = call i256 @calldatasize()
  %abi_decode_tuple_t_uint256 = call i256 @abi_decode_tuple_t_uint256(i256 4, i256 %calldatasize)
  store i256 %abi_decode_tuple_t_uint256, i256* %param_0, align 4
  %param_01 = load i256, i256* %param_0, align 4
  %fun_loop_28 = call i256 @fun_loop_28(i256 %param_01)
  store i256 %fun_loop_28, i256* %ret_0, align 4
  %allocate_unbounded = call i256 @allocate_unbounded()
  store i256 %allocate_unbounded, i256* %memPos, align 4
  %memPos2 = load i256, i256* %memPos, align 4
  %ret_03 = load i256, i256* %ret_0, align 4
  %abi_encode_tuple_t_uint256__to_t_uint256__fromStack = call i256 @abi_encode_tuple_t_uint256__to_t_uint256__fromStack(i256 %memPos2, i256 %ret_03)
  store i256 %abi_encode_tuple_t_uint256__to_t_uint256__fromStack, i256* %memEnd, align 4
  %memPos4 = load i256, i256* %memPos, align 4
  %memEnd5 = load i256, i256* %memEnd, align 4
  %memPos6 = load i256, i256* %memPos, align 4
  %sub = call i256 @sub(i256 %memEnd5, i256 %memPos6)
  %return = call i256 @return(i256 %memPos4, i256 %sub)
  ret void
}

declare i256 @callvalue()

declare i256 @return(i256, i256)

define void @revert_error_42b3090547df1d2001c96683413b8cf91c1b902ef5e3cb8d9f6f304cf7446f74() {
entry:
  %eurus_dispatcher_revert = call i256 @eurus_dispatcher_revert()
  ret void
}

declare i256 @eurus_dispatcher_revert()

define i256 @zero_value_for_split_t_uint256() {
entry:
  %ret = alloca i256, align 8
  store i256 0, i256* %ret, align 4
  %0 = load i256, i256* %ret, align 4
  ret i256 %0
}

define i256 @cleanup_t_rational_0_by_1(i256 %value) {
entry:
  %cleaned = alloca i256, align 8
  %value_arg = alloca i256, align 8
  store i256 %value, i256* %value_arg, align 4
  %value1 = load i256, i256* %value_arg, align 4
  store i256 %value1, i256* %cleaned, align 4
  %0 = load i256, i256* %cleaned, align 4
  ret i256 %0
}

define i256 @identity(i256 %value) {
entry:
  %ret = alloca i256, align 8
  %value_arg = alloca i256, align 8
  store i256 %value, i256* %value_arg, align 4
  %value1 = load i256, i256* %value_arg, align 4
  store i256 %value1, i256* %ret, align 4
  %0 = load i256, i256* %ret, align 4
  ret i256 %0
}

define i256 @convert_t_rational_0_by_1_to_t_uint256(i256 %value) {
entry:
  %converted = alloca i256, align 8
  %value_arg = alloca i256, align 8
  store i256 %value, i256* %value_arg, align 4
  %value1 = load i256, i256* %value_arg, align 4
  %cleanup_t_rational_0_by_1 = call i256 @cleanup_t_rational_0_by_1(i256 %value1)
  %identity = call i256 @identity(i256 %cleanup_t_rational_0_by_1)
  %cleanup_t_uint256 = call i256 @cleanup_t_uint256(i256 %identity)
  store i256 %cleanup_t_uint256, i256* %converted, align 4
  %0 = load i256, i256* %converted, align 4
  ret i256 %0
}

define void @panic_error_0x11() {
entry:
  %mstore = call i256 @mstore(i256 0, i256 35408467139433450592217433187231851964531694900788300625387963629091585785856)
  %mstore1 = call i256 @mstore(i256 4, i256 17)
  call void @revert()
  ret void
}

define i256 @increment_t_uint256(i256 %value) {
entry:
  %ret = alloca i256, align 8
  %value_arg = alloca i256, align 8
  store i256 %value, i256* %value_arg, align 4
  %value1 = load i256, i256* %value_arg, align 4
  %cleanup_t_uint256 = call i256 @cleanup_t_uint256(i256 %value1)
  store i256 %cleanup_t_uint256, i256* %value_arg, align 4
  %value2 = load i256, i256* %value_arg, align 4
  %eq = call i256 @eq(i256 %value2, i256 -1)
  %0 = trunc i256 %eq to i1
  br i1 %0, label %if-taken-body, label %if-not-taken-body

if-taken-body:                                    ; preds = %entry
  call void @panic_error_0x11()
  br label %if-not-taken-body

if-not-taken-body:                                ; preds = %if-taken-body, %entry
  %value3 = load i256, i256* %value_arg, align 4
  %add = call i256 @add(i256 %value3, i256 1)
  store i256 %add, i256* %ret, align 4
  %1 = load i256, i256* %ret, align 4
  ret i256 %1
}

define i256 @cleanup_t_rational_10_by_1(i256 %value) {
entry:
  %cleaned = alloca i256, align 8
  %value_arg = alloca i256, align 8
  store i256 %value, i256* %value_arg, align 4
  %value1 = load i256, i256* %value_arg, align 4
  store i256 %value1, i256* %cleaned, align 4
  %0 = load i256, i256* %cleaned, align 4
  ret i256 %0
}

define i256 @convert_t_rational_10_by_1_to_t_uint256(i256 %value) {
entry:
  %converted = alloca i256, align 8
  %value_arg = alloca i256, align 8
  store i256 %value, i256* %value_arg, align 4
  %value1 = load i256, i256* %value_arg, align 4
  %cleanup_t_rational_10_by_1 = call i256 @cleanup_t_rational_10_by_1(i256 %value1)
  %identity = call i256 @identity(i256 %cleanup_t_rational_10_by_1)
  %cleanup_t_uint256 = call i256 @cleanup_t_uint256(i256 %identity)
  store i256 %cleanup_t_uint256, i256* %converted, align 4
  %0 = load i256, i256* %converted, align 4
  ret i256 %0
}

define i256 @cleanup_t_rational_1_by_1(i256 %value) {
entry:
  %cleaned = alloca i256, align 8
  %value_arg = alloca i256, align 8
  store i256 %value, i256* %value_arg, align 4
  %value1 = load i256, i256* %value_arg, align 4
  store i256 %value1, i256* %cleaned, align 4
  %0 = load i256, i256* %cleaned, align 4
  ret i256 %0
}

define i256 @convert_t_rational_1_by_1_to_t_uint256(i256 %value) {
entry:
  %converted = alloca i256, align 8
  %value_arg = alloca i256, align 8
  store i256 %value, i256* %value_arg, align 4
  %value1 = load i256, i256* %value_arg, align 4
  %cleanup_t_rational_1_by_1 = call i256 @cleanup_t_rational_1_by_1(i256 %value1)
  %identity = call i256 @identity(i256 %cleanup_t_rational_1_by_1)
  %cleanup_t_uint256 = call i256 @cleanup_t_uint256(i256 %identity)
  store i256 %cleanup_t_uint256, i256* %converted, align 4
  %0 = load i256, i256* %converted, align 4
  ret i256 %0
}

define i256 @checked_add_t_uint256(i256 %x, i256 %y) {
entry:
  %sum = alloca i256, align 8
  %y_arg = alloca i256, align 8
  %x_arg = alloca i256, align 8
  store i256 %x, i256* %x_arg, align 4
  store i256 %y, i256* %y_arg, align 4
  %x1 = load i256, i256* %x_arg, align 4
  %cleanup_t_uint256 = call i256 @cleanup_t_uint256(i256 %x1)
  store i256 %cleanup_t_uint256, i256* %x_arg, align 4
  %y2 = load i256, i256* %y_arg, align 4
  %cleanup_t_uint2563 = call i256 @cleanup_t_uint256(i256 %y2)
  store i256 %cleanup_t_uint2563, i256* %y_arg, align 4
  %x4 = load i256, i256* %x_arg, align 4
  %y5 = load i256, i256* %y_arg, align 4
  %sub = call i256 @sub(i256 -1, i256 %y5)
  %gt = call i256 @gt(i256 %x4, i256 %sub)
  %0 = trunc i256 %gt to i1
  br i1 %0, label %if-taken-body, label %if-not-taken-body

if-taken-body:                                    ; preds = %entry
  call void @panic_error_0x11()
  br label %if-not-taken-body

if-not-taken-body:                                ; preds = %if-taken-body, %entry
  %x6 = load i256, i256* %x_arg, align 4
  %y7 = load i256, i256* %y_arg, align 4
  %add = call i256 @add(i256 %x6, i256 %y7)
  store i256 %add, i256* %sum, align 4
  %1 = load i256, i256* %sum, align 4
  ret i256 %1
}

declare i256 @gt(i256, i256)

define i256 @fun_loop_28(i256 %var_a_3) {
entry:
  %expr_25 = alloca i256, align 8
  %_6 = alloca i256, align 8
  %expr_16 = alloca i256, align 8
  %_2 = alloca i256, align 8
  %_3 = alloca i256, align 8
  %expr_22 = alloca i256, align 8
  %expr_21 = alloca i256, align 8
  %expr_20 = alloca i256, align 8
  %expr_19 = alloca i256, align 8
  %_5 = alloca i256, align 8
  %expr_14 = alloca i256, align 8
  %expr_13 = alloca i256, align 8
  %expr_12 = alloca i256, align 8
  %_4 = alloca i256, align 8
  %var_i_9 = alloca i256, align 8
  %expr_10 = alloca i256, align 8
  %zero_t_uint256_1 = alloca i256, align 8
  %var__6 = alloca i256, align 8
  %var_a_3_arg = alloca i256, align 8
  store i256 %var_a_3, i256* %var_a_3_arg, align 4
  %zero_value_for_split_t_uint256 = call i256 @zero_value_for_split_t_uint256()
  store i256 %zero_value_for_split_t_uint256, i256* %zero_t_uint256_1, align 4
  %zero_t_uint256_11 = load i256, i256* %zero_t_uint256_1, align 4
  store i256 %zero_t_uint256_11, i256* %var__6, align 4
  store i256 0, i256* %expr_10, align 4
  %expr_102 = load i256, i256* %expr_10, align 4
  %convert_t_rational_0_by_1_to_t_uint256 = call i256 @convert_t_rational_0_by_1_to_t_uint256(i256 %expr_102)
  store i256 %convert_t_rational_0_by_1_to_t_uint256, i256* %var_i_9, align 4

loop-cond:                                        ; preds = %for-incr
  %var_i_93 = load i256, i256* %var_i_9, align 4
  store i256 %var_i_93, i256* %_4, align 4
  %_44 = load i256, i256* %_4, align 4
  store i256 %_44, i256* %expr_12, align 4
  store i256 10, i256* %expr_13, align 4
  %expr_125 = load i256, i256* %expr_12, align 4
  %cleanup_t_uint256 = call i256 @cleanup_t_uint256(i256 %expr_125)
  %expr_136 = load i256, i256* %expr_13, align 4
  %convert_t_rational_10_by_1_to_t_uint256 = call i256 @convert_t_rational_10_by_1_to_t_uint256(i256 %expr_136)
  %lt = call i256 @lt(i256 %cleanup_t_uint256, i256 %convert_t_rational_10_by_1_to_t_uint256)
  store i256 %lt, i256* %expr_14, align 4
  %expr_147 = load i256, i256* %expr_14, align 4
  %iszero = call i256 @iszero(i256 %expr_147)
  %0 = trunc i256 %iszero to i1
  br i1 %0, label %if-taken-body, label %if-not-taken-body

for-incr:                                         ; No predecessors!
  %var_i_914 = load i256, i256* %var_i_9, align 4
  store i256 %var_i_914, i256* %_3, align 4
  %_315 = load i256, i256* %_3, align 4
  %increment_t_uint256 = call i256 @increment_t_uint256(i256 %_315)
  store i256 %increment_t_uint256, i256* %_2, align 4
  %_216 = load i256, i256* %_2, align 4
  store i256 %_216, i256* %var_i_9, align 4
  %_317 = load i256, i256* %_3, align 4
  store i256 %_317, i256* %expr_16, align 4
  br label %loop-cond

if-taken-body:                                    ; preds = %loop-cond
  br label %for-cont
  br label %if-not-taken-body

if-not-taken-body:                                ; preds = %if-taken-body, %loop-cond
  %var_a_38 = load i256, i256* %var_a_3_arg, align 4
  store i256 %var_a_38, i256* %_5, align 4
  %_59 = load i256, i256* %_5, align 4
  store i256 %_59, i256* %expr_19, align 4
  store i256 1, i256* %expr_20, align 4
  %expr_1910 = load i256, i256* %expr_19, align 4
  %expr_2011 = load i256, i256* %expr_20, align 4
  %convert_t_rational_1_by_1_to_t_uint256 = call i256 @convert_t_rational_1_by_1_to_t_uint256(i256 %expr_2011)
  %1 = add i256 %expr_1910, %convert_t_rational_1_by_1_to_t_uint256
  store i256 %1, i256* %expr_21, align 4
  %expr_2112 = load i256, i256* %expr_21, align 4
  store i256 %expr_2112, i256* %var_a_3_arg, align 4
  %expr_2113 = load i256, i256* %expr_21, align 4
  store i256 %expr_2113, i256* %expr_22, align 4

for-cont:                                         ; preds = %if-taken-body
  %var_a_318 = load i256, i256* %var_a_3_arg, align 4
  store i256 %var_a_318, i256* %_6, align 4
  %_619 = load i256, i256* %_6, align 4
  store i256 %_619, i256* %expr_25, align 4
  %expr_2520 = load i256, i256* %expr_25, align 4
  store i256 %expr_2520, i256* %var__6, align 4
  %2 = load i256, i256* %var__6, align 4
  ret i256 %2
}

define void @constructor_ForLoopTestContract_29() {
entry:
  ret void
}
