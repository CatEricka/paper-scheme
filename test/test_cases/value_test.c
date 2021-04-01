#include "lib/utest.h"
#include "paper-scheme/runtime.h"

/**
 * 值类型测试
 * 测试基础类型是否工作正常
 */

UTEST(value_test, all_value_type_function_test) {
    context_t context = context_make(0x100, 2, 0x100000);
    gc_var4(context, obj, tmp1, tmp2, tmp3);

    int64_t start = utest_ns();
    gc_collect(context);
    int64_t time = utest_ns() - start;
    UTEST_PRINTF("gc time: %"
                         PRId64
                         " ns\n", time);
    ASSERT_TRUE(context->heap->first_node->data == context->heap->first_node->free_ptr);
    int gc_print = 0;

    // null imm
    obj = NULL;
    ASSERT_TRUE(is_null(obj));
    ASSERT_FALSE(is_imm(obj));
    ASSERT_FALSE(is_unique_imm(obj));
    ASSERT_FALSE(is_i64_real(obj));
    ASSERT_FALSE(is_imm_i64(obj));
    ASSERT_FALSE(is_imm_char(obj));
    ASSERT_FALSE(is_object(obj));
    ASSERT_FALSE(is_i64(obj));
    ASSERT_FALSE(is_doublenum(obj));
    ASSERT_FALSE(is_pair(obj));
    ASSERT_FALSE(is_string(obj));
    ASSERT_FALSE(is_symbol(obj));
    ASSERT_FALSE(is_vector(obj));
    ASSERT_FALSE(is_bytes(obj));
    ASSERT_FALSE(is_string_buffer(obj));
    ASSERT_FALSE(is_stack(obj));
    ASSERT_FALSE(is_string_port(obj));
    ASSERT_FALSE(is_string_port_input(obj));
    ASSERT_FALSE(is_string_port_output(obj));
    ASSERT_FALSE(is_string_port_in_out_put(obj));
    ASSERT_FALSE(is_string_port_eof(obj));
    ASSERT_FALSE(is_stdio_port(obj));
    ASSERT_FALSE(is_stdio_port_input(obj));
    ASSERT_FALSE(is_stdio_port_output(obj));
    ASSERT_FALSE(is_stdio_port_in_out_put(obj));
    ASSERT_FALSE(is_stdio_port_eof(obj));
    ASSERT_FALSE(is_port_input(obj));
    ASSERT_FALSE(is_port_output(obj));
    ASSERT_FALSE(is_port_in_out_put(obj));
    ASSERT_FALSE(is_port_eof(obj));
    ASSERT_FALSE(is_srfi6_port(obj));

    start = utest_ns();
    gc_collect(context);
    time = utest_ns() - start;
    if (gc_print) {
        UTEST_PRINTF("gc time: %"
                             PRId64
                             " ns\n", time);
    }

    // true imm
    obj = IMM_TRUE;
    ASSERT_FALSE(is_null(obj));
    ASSERT_FALSE(is_null(obj));
    ASSERT_TRUE(is_imm(obj));
    ASSERT_TRUE(is_unique_imm(obj));
    ASSERT_FALSE(is_i64_real(obj));
    ASSERT_FALSE(is_imm_i64(obj));
    ASSERT_FALSE(is_imm_char(obj));
    ASSERT_FALSE(is_object(obj));
    ASSERT_FALSE(is_i64(obj));
    ASSERT_FALSE(is_doublenum(obj));
    ASSERT_FALSE(is_pair(obj));
    ASSERT_FALSE(is_string(obj));
    ASSERT_FALSE(is_symbol(obj));
    ASSERT_FALSE(is_vector(obj));
    ASSERT_FALSE(is_bytes(obj));
    ASSERT_FALSE(is_string_buffer(obj));
    ASSERT_FALSE(is_stack(obj));
    ASSERT_FALSE(is_string_port(obj));
    ASSERT_FALSE(is_string_port_input(obj));
    ASSERT_FALSE(is_string_port_output(obj));
    ASSERT_FALSE(is_string_port_in_out_put(obj));
    ASSERT_FALSE(is_string_port_eof(obj));
    ASSERT_FALSE(is_stdio_port(obj));
    ASSERT_FALSE(is_stdio_port_input(obj));
    ASSERT_FALSE(is_stdio_port_output(obj));
    ASSERT_FALSE(is_stdio_port_in_out_put(obj));
    ASSERT_FALSE(is_stdio_port_eof(obj));
    ASSERT_FALSE(is_port_input(obj));
    ASSERT_FALSE(is_port_output(obj));
    ASSERT_FALSE(is_port_in_out_put(obj));
    ASSERT_FALSE(is_port_eof(obj));
    ASSERT_FALSE(is_srfi6_port(obj));

    start = utest_ns();
    gc_collect(context);
    time = utest_ns() - start;
    if (gc_print) {
        UTEST_PRINTF("gc time: %"
                             PRId64
                             " ns\n", time);
    }

    // false imm
    obj = IMM_FALSE;
    ASSERT_FALSE(is_null(obj));
    ASSERT_TRUE(is_imm(obj));
    ASSERT_TRUE(is_unique_imm(obj));
    ASSERT_FALSE(is_i64_real(obj));
    ASSERT_FALSE(is_imm_i64(obj));
    ASSERT_FALSE(is_imm_char(obj));
    ASSERT_FALSE(is_object(obj));
    ASSERT_FALSE(is_i64(obj));
    ASSERT_FALSE(is_doublenum(obj));
    ASSERT_FALSE(is_pair(obj));
    ASSERT_FALSE(is_string(obj));
    ASSERT_FALSE(is_symbol(obj));
    ASSERT_FALSE(is_vector(obj));
    ASSERT_FALSE(is_bytes(obj));
    ASSERT_FALSE(is_string_buffer(obj));
    ASSERT_FALSE(is_stack(obj));
    ASSERT_FALSE(is_string_port(obj));
    ASSERT_FALSE(is_string_port_input(obj));
    ASSERT_FALSE(is_string_port_output(obj));
    ASSERT_FALSE(is_string_port_in_out_put(obj));
    ASSERT_FALSE(is_string_port_eof(obj));
    ASSERT_FALSE(is_stdio_port(obj));
    ASSERT_FALSE(is_stdio_port_input(obj));
    ASSERT_FALSE(is_stdio_port_output(obj));
    ASSERT_FALSE(is_stdio_port_in_out_put(obj));
    ASSERT_FALSE(is_stdio_port_eof(obj));
    ASSERT_FALSE(is_port_input(obj));
    ASSERT_FALSE(is_port_output(obj));
    ASSERT_FALSE(is_port_in_out_put(obj));
    ASSERT_FALSE(is_port_eof(obj));
    ASSERT_FALSE(is_srfi6_port(obj));

    start = utest_ns();
    gc_collect(context);
    time = utest_ns() - start;
    if (gc_print) {
        UTEST_PRINTF("gc time: %"
                             PRId64
                             " ns\n", time);
    }

    // unit imm
    obj = IMM_UNIT;
    ASSERT_FALSE(is_null(obj));
    ASSERT_TRUE(is_imm(obj));
    ASSERT_TRUE(is_unique_imm(obj));
    ASSERT_FALSE(is_i64_real(obj));
    ASSERT_FALSE(is_imm_i64(obj));
    ASSERT_FALSE(is_imm_char(obj));
    ASSERT_FALSE(is_object(obj));
    ASSERT_FALSE(is_i64(obj));
    ASSERT_FALSE(is_doublenum(obj));
    ASSERT_FALSE(is_pair(obj));
    ASSERT_FALSE(is_string(obj));
    ASSERT_FALSE(is_symbol(obj));
    ASSERT_FALSE(is_vector(obj));
    ASSERT_FALSE(is_bytes(obj));
    ASSERT_FALSE(is_string_buffer(obj));
    ASSERT_FALSE(is_stack(obj));
    ASSERT_FALSE(is_string_port(obj));
    ASSERT_FALSE(is_string_port_input(obj));
    ASSERT_FALSE(is_string_port_output(obj));
    ASSERT_FALSE(is_string_port_in_out_put(obj));
    ASSERT_FALSE(is_string_port_eof(obj));
    ASSERT_FALSE(is_stdio_port(obj));
    ASSERT_FALSE(is_stdio_port_input(obj));
    ASSERT_FALSE(is_stdio_port_output(obj));
    ASSERT_FALSE(is_stdio_port_in_out_put(obj));
    ASSERT_FALSE(is_stdio_port_eof(obj));
    ASSERT_FALSE(is_port_input(obj));
    ASSERT_FALSE(is_port_output(obj));
    ASSERT_FALSE(is_port_in_out_put(obj));
    ASSERT_FALSE(is_port_eof(obj));
    ASSERT_FALSE(is_srfi6_port(obj));

    start = utest_ns();
    gc_collect(context);
    time = utest_ns() - start;
    if (gc_print) {
        UTEST_PRINTF("gc time: %"
                             PRId64
                             " ns\n", time);
    }

    // char imm
    object imm_char = char_imm_make('x');
    obj = imm_char;
    ASSERT_EQ('x', char_imm_getvalue(obj));
    ASSERT_FALSE(is_null(obj));
    ASSERT_TRUE(is_imm(obj));
    ASSERT_FALSE(is_unique_imm(obj));
    ASSERT_FALSE(is_i64_real(obj));
    ASSERT_FALSE(is_imm_i64(obj));
    ASSERT_TRUE(is_imm_char(obj));
    ASSERT_FALSE(is_object(obj));
    ASSERT_FALSE(is_i64(obj));
    ASSERT_FALSE(is_doublenum(obj));
    ASSERT_FALSE(is_pair(obj));
    ASSERT_FALSE(is_string(obj));
    ASSERT_FALSE(is_symbol(obj));
    ASSERT_FALSE(is_vector(obj));
    ASSERT_FALSE(is_bytes(obj));
    ASSERT_FALSE(is_string_buffer(obj));
    ASSERT_FALSE(is_stack(obj));
    ASSERT_FALSE(is_string_port(obj));
    ASSERT_FALSE(is_string_port_input(obj));
    ASSERT_FALSE(is_string_port_output(obj));
    ASSERT_FALSE(is_string_port_in_out_put(obj));
    ASSERT_FALSE(is_string_port_eof(obj));
    ASSERT_FALSE(is_stdio_port(obj));
    ASSERT_FALSE(is_stdio_port_input(obj));
    ASSERT_FALSE(is_stdio_port_output(obj));
    ASSERT_FALSE(is_stdio_port_in_out_put(obj));
    ASSERT_FALSE(is_stdio_port_eof(obj));
    ASSERT_FALSE(is_port_input(obj));
    ASSERT_FALSE(is_port_output(obj));
    ASSERT_FALSE(is_port_in_out_put(obj));
    ASSERT_FALSE(is_port_eof(obj));
    ASSERT_FALSE(is_srfi6_port(obj));

    start = utest_ns();
    gc_collect(context);
    time = utest_ns() - start;
    if (gc_print) {
        UTEST_PRINTF("gc time: %"
                             PRId64
                             " ns\n", time);
    }

    imm_char = char_imm_make('\0');
    obj = imm_char;
    ASSERT_EQ('\0', char_imm_getvalue(obj));
    ASSERT_FALSE(is_null(obj));
    ASSERT_TRUE(is_imm(obj));
    ASSERT_FALSE(is_unique_imm(obj));
    ASSERT_FALSE(is_i64_real(obj));
    ASSERT_FALSE(is_imm_i64(obj));
    ASSERT_TRUE(is_imm_char(obj));
    ASSERT_FALSE(is_object(obj));
    ASSERT_FALSE(is_i64(obj));
    ASSERT_FALSE(is_doublenum(obj));
    ASSERT_FALSE(is_pair(obj));
    ASSERT_FALSE(is_string(obj));
    ASSERT_FALSE(is_symbol(obj));
    ASSERT_FALSE(is_vector(obj));
    ASSERT_FALSE(is_bytes(obj));
    ASSERT_FALSE(is_string_buffer(obj));
    ASSERT_FALSE(is_stack(obj));
    ASSERT_FALSE(is_string_port(obj));
    ASSERT_FALSE(is_string_port_input(obj));
    ASSERT_FALSE(is_string_port_output(obj));
    ASSERT_FALSE(is_string_port_in_out_put(obj));
    ASSERT_FALSE(is_string_port_eof(obj));
    ASSERT_FALSE(is_stdio_port(obj));
    ASSERT_FALSE(is_stdio_port_input(obj));
    ASSERT_FALSE(is_stdio_port_output(obj));
    ASSERT_FALSE(is_stdio_port_in_out_put(obj));
    ASSERT_FALSE(is_stdio_port_eof(obj));
    ASSERT_FALSE(is_port_input(obj));
    ASSERT_FALSE(is_port_output(obj));
    ASSERT_FALSE(is_port_in_out_put(obj));
    ASSERT_FALSE(is_port_eof(obj));
    ASSERT_FALSE(is_srfi6_port(obj));

    start = utest_ns();
    gc_collect(context);
    time = utest_ns() - start;
    if (gc_print) {
        UTEST_PRINTF("gc time: %"
                             PRId64
                             " ns\n", time);
    }

    // i64 imm
    object imm_i64 = i64_imm_make(123);
    obj = imm_i64;
    ASSERT_EQ(123, i64_getvalue(obj));
    ASSERT_FALSE(is_null(obj));
    ASSERT_TRUE(is_imm(obj));
    ASSERT_FALSE(is_unique_imm(obj));
    ASSERT_FALSE(is_i64_real(obj));
    ASSERT_TRUE(is_imm_i64(obj));
    ASSERT_FALSE(is_imm_char(obj));
    ASSERT_FALSE(is_object(obj));
    ASSERT_TRUE(is_i64(obj));
    ASSERT_FALSE(is_doublenum(obj));
    ASSERT_FALSE(is_pair(obj));
    ASSERT_FALSE(is_string(obj));
    ASSERT_FALSE(is_symbol(obj));
    ASSERT_FALSE(is_vector(obj));
    ASSERT_FALSE(is_bytes(obj));
    ASSERT_FALSE(is_string_buffer(obj));
    ASSERT_FALSE(is_stack(obj));
    ASSERT_FALSE(is_string_port(obj));
    ASSERT_FALSE(is_string_port_input(obj));
    ASSERT_FALSE(is_string_port_output(obj));
    ASSERT_FALSE(is_string_port_in_out_put(obj));
    ASSERT_FALSE(is_string_port_eof(obj));
    ASSERT_FALSE(is_stdio_port(obj));
    ASSERT_FALSE(is_stdio_port_input(obj));
    ASSERT_FALSE(is_stdio_port_output(obj));
    ASSERT_FALSE(is_stdio_port_in_out_put(obj));
    ASSERT_FALSE(is_stdio_port_eof(obj));
    ASSERT_FALSE(is_port_input(obj));
    ASSERT_FALSE(is_port_output(obj));
    ASSERT_FALSE(is_port_in_out_put(obj));
    ASSERT_FALSE(is_port_eof(obj));
    ASSERT_FALSE(is_srfi6_port(obj));

    start = utest_ns();
    gc_collect(context);
    time = utest_ns() - start;
    if (gc_print) {
        UTEST_PRINTF("gc time: %"
                             PRId64
                             " ns\n", time);
    }

    // i64
    obj = i64_make_real_object_op(context, 20);
    ASSERT_EQ(20, i64_getvalue(obj));
    ASSERT_FALSE(is_null(obj));
    ASSERT_FALSE(is_imm(obj));
    ASSERT_FALSE(is_unique_imm(obj));
    ASSERT_TRUE(is_i64_real(obj));
    ASSERT_FALSE(is_imm_i64(obj));
    ASSERT_FALSE(is_imm_char(obj));
    ASSERT_TRUE(is_object(obj));
    ASSERT_TRUE(is_i64(obj));
    ASSERT_FALSE(is_doublenum(obj));
    ASSERT_FALSE(is_pair(obj));
    ASSERT_FALSE(is_string(obj));
    ASSERT_FALSE(is_symbol(obj));
    ASSERT_FALSE(is_vector(obj));
    ASSERT_FALSE(is_bytes(obj));
    ASSERT_FALSE(is_string_buffer(obj));
    ASSERT_FALSE(is_stack(obj));
    ASSERT_FALSE(is_string_port(obj));
    ASSERT_FALSE(is_string_port_input(obj));
    ASSERT_FALSE(is_string_port_output(obj));
    ASSERT_FALSE(is_string_port_in_out_put(obj));
    ASSERT_FALSE(is_string_port_eof(obj));
    ASSERT_FALSE(is_stdio_port(obj));
    ASSERT_FALSE(is_stdio_port_input(obj));
    ASSERT_FALSE(is_stdio_port_output(obj));
    ASSERT_FALSE(is_stdio_port_in_out_put(obj));
    ASSERT_FALSE(is_stdio_port_eof(obj));
    ASSERT_FALSE(is_port_input(obj));
    ASSERT_FALSE(is_port_output(obj));
    ASSERT_FALSE(is_port_in_out_put(obj));
    ASSERT_FALSE(is_port_eof(obj));
    ASSERT_FALSE(is_srfi6_port(obj));

    start = utest_ns();
    gc_collect(context);
    time = utest_ns() - start;
    if (gc_print) {
        UTEST_PRINTF("gc time: %"
                             PRId64
                             " ns\n", time);
    }
    obj = i64_make_op(context, INT64_MAX);
    ASSERT_EQ(INT64_MAX, i64_getvalue(obj));
    ASSERT_FALSE(is_null(obj));
    ASSERT_FALSE(is_imm(obj));
    ASSERT_FALSE(is_unique_imm(obj));
    ASSERT_TRUE(is_i64_real(obj));
    ASSERT_FALSE(is_imm_i64(obj));
    ASSERT_FALSE(is_imm_char(obj));
    ASSERT_TRUE(is_object(obj));
    ASSERT_TRUE(is_i64(obj));
    ASSERT_FALSE(is_doublenum(obj));
    ASSERT_FALSE(is_pair(obj));
    ASSERT_FALSE(is_string(obj));
    ASSERT_FALSE(is_symbol(obj));
    ASSERT_FALSE(is_vector(obj));
    ASSERT_FALSE(is_bytes(obj));
    ASSERT_FALSE(is_string_buffer(obj));
    ASSERT_FALSE(is_stack(obj));
    ASSERT_FALSE(is_string_port(obj));
    ASSERT_FALSE(is_string_port_input(obj));
    ASSERT_FALSE(is_string_port_output(obj));
    ASSERT_FALSE(is_string_port_in_out_put(obj));
    ASSERT_FALSE(is_string_port_eof(obj));
    ASSERT_FALSE(is_stdio_port(obj));
    ASSERT_FALSE(is_stdio_port_input(obj));
    ASSERT_FALSE(is_stdio_port_output(obj));
    ASSERT_FALSE(is_stdio_port_in_out_put(obj));
    ASSERT_FALSE(is_stdio_port_eof(obj));
    ASSERT_FALSE(is_port_input(obj));
    ASSERT_FALSE(is_port_output(obj));
    ASSERT_FALSE(is_port_in_out_put(obj));
    ASSERT_FALSE(is_port_eof(obj));
    ASSERT_FALSE(is_srfi6_port(obj));

    start = utest_ns();
    gc_collect(context);
    time = utest_ns() - start;
    if (gc_print) {
        UTEST_PRINTF("gc time: %"
                             PRId64
                             " ns\n", time);
    }

    obj = i64_make_real_object_op(context, INT64_MIN);
    ASSERT_EQ(INT64_MIN, i64_getvalue(obj));
    ASSERT_FALSE(is_null(obj));
    ASSERT_FALSE(is_imm(obj));
    ASSERT_FALSE(is_unique_imm(obj));
    ASSERT_TRUE(is_i64_real(obj));
    ASSERT_FALSE(is_imm_i64(obj));
    ASSERT_FALSE(is_imm_char(obj));
    ASSERT_TRUE(is_object(obj));
    ASSERT_TRUE(is_i64(obj));
    ASSERT_FALSE(is_doublenum(obj));
    ASSERT_FALSE(is_pair(obj));
    ASSERT_FALSE(is_string(obj));
    ASSERT_FALSE(is_symbol(obj));
    ASSERT_FALSE(is_vector(obj));
    ASSERT_FALSE(is_bytes(obj));
    ASSERT_FALSE(is_string_buffer(obj));
    ASSERT_FALSE(is_stack(obj));
    ASSERT_FALSE(is_string_port(obj));
    ASSERT_FALSE(is_string_port_input(obj));
    ASSERT_FALSE(is_string_port_output(obj));
    ASSERT_FALSE(is_string_port_in_out_put(obj));
    ASSERT_FALSE(is_string_port_eof(obj));
    ASSERT_FALSE(is_stdio_port(obj));
    ASSERT_FALSE(is_stdio_port_input(obj));
    ASSERT_FALSE(is_stdio_port_output(obj));
    ASSERT_FALSE(is_stdio_port_in_out_put(obj));
    ASSERT_FALSE(is_stdio_port_eof(obj));
    ASSERT_FALSE(is_port_input(obj));
    ASSERT_FALSE(is_port_output(obj));
    ASSERT_FALSE(is_port_in_out_put(obj));
    ASSERT_FALSE(is_port_eof(obj));
    ASSERT_FALSE(is_srfi6_port(obj));

    start = utest_ns();
    gc_collect(context);
    time = utest_ns() - start;
    if (gc_print) {
        UTEST_PRINTF("gc time: %"
                             PRId64
                             " ns\n", time);
    }

    // double number
    obj = doublenum_make_op(context, 200.0);
    ASSERT_EQ(doublenum_getvalue(obj), 200.0);
    ASSERT_FALSE(is_null(obj));
    ASSERT_FALSE(is_imm(obj));
    ASSERT_FALSE(is_unique_imm(obj));
    ASSERT_FALSE(is_i64_real(obj));
    ASSERT_FALSE(is_imm_i64(obj));
    ASSERT_FALSE(is_imm_char(obj));
    ASSERT_TRUE(is_object(obj));
    ASSERT_FALSE(is_i64(obj));
    ASSERT_TRUE(is_doublenum(obj));
    ASSERT_FALSE(is_pair(obj));
    ASSERT_FALSE(is_string(obj));
    ASSERT_FALSE(is_symbol(obj));
    ASSERT_FALSE(is_vector(obj));
    ASSERT_FALSE(is_bytes(obj));
    ASSERT_FALSE(is_string_buffer(obj));
    ASSERT_FALSE(is_stack(obj));
    ASSERT_FALSE(is_string_port(obj));
    ASSERT_FALSE(is_string_port_input(obj));
    ASSERT_FALSE(is_string_port_output(obj));
    ASSERT_FALSE(is_string_port_in_out_put(obj));
    ASSERT_FALSE(is_string_port_eof(obj));
    ASSERT_FALSE(is_stdio_port(obj));
    ASSERT_FALSE(is_stdio_port_input(obj));
    ASSERT_FALSE(is_stdio_port_output(obj));
    ASSERT_FALSE(is_stdio_port_in_out_put(obj));
    ASSERT_FALSE(is_stdio_port_eof(obj));
    ASSERT_FALSE(is_port_input(obj));
    ASSERT_FALSE(is_port_output(obj));
    ASSERT_FALSE(is_port_in_out_put(obj));
    ASSERT_FALSE(is_port_eof(obj));
    ASSERT_FALSE(is_srfi6_port(obj));

    start = utest_ns();
    gc_collect(context);
    time = utest_ns() - start;
    if (gc_print) {
        UTEST_PRINTF("gc time: %"
                             PRId64
                             " ns\n", time);
    }

    // pair
    tmp1 = obj; // doublenum
    object pair_carImmI64_cdrDouble = pair_make_op(context, imm_i64, tmp1);
    obj = pair_carImmI64_cdrDouble;
    ASSERT_EQ(pair_car(obj), imm_i64);
    ASSERT_EQ(i64_getvalue(pair_car(obj)), i64_getvalue(imm_i64));
    ASSERT_EQ(doublenum_getvalue(pair_cdr(obj)), doublenum_getvalue(tmp1));
    ASSERT_EQ(pair_cdr(obj), tmp1);
    ASSERT_FALSE(is_null(obj));
    ASSERT_FALSE(is_imm(obj));
    ASSERT_FALSE(is_unique_imm(obj));
    ASSERT_FALSE(is_i64_real(obj));
    ASSERT_FALSE(is_imm_i64(obj));
    ASSERT_FALSE(is_imm_char(obj));
    ASSERT_TRUE(is_object(obj));
    ASSERT_FALSE(is_i64(obj));
    ASSERT_FALSE(is_doublenum(obj));
    ASSERT_TRUE(is_pair(obj));
    ASSERT_FALSE(is_string(obj));
    ASSERT_FALSE(is_symbol(obj));
    ASSERT_FALSE(is_vector(obj));
    ASSERT_FALSE(is_bytes(obj));
    ASSERT_FALSE(is_string_buffer(obj));
    ASSERT_FALSE(is_stack(obj));
    ASSERT_FALSE(is_string_port(obj));
    ASSERT_FALSE(is_string_port_input(obj));
    ASSERT_FALSE(is_string_port_output(obj));
    ASSERT_FALSE(is_string_port_in_out_put(obj));
    ASSERT_FALSE(is_string_port_eof(obj));
    ASSERT_FALSE(is_stdio_port(obj));
    ASSERT_FALSE(is_stdio_port_input(obj));
    ASSERT_FALSE(is_stdio_port_output(obj));
    ASSERT_FALSE(is_stdio_port_in_out_put(obj));
    ASSERT_FALSE(is_stdio_port_eof(obj));
    ASSERT_FALSE(is_port_input(obj));
    ASSERT_FALSE(is_port_output(obj));
    ASSERT_FALSE(is_port_in_out_put(obj));
    ASSERT_FALSE(is_port_eof(obj));
    ASSERT_FALSE(is_srfi6_port(obj));

    start = utest_ns();
    gc_collect(context);
    time = utest_ns() - start;
    if (gc_print) {
        UTEST_PRINTF("gc time: %"
                             PRId64
                             " ns\n", time);
    }

    // string
    char str[] = "this is a string object";
    obj = string_make_from_cstr_op(context, str);
    ASSERT_EQ(strlen(str), string_len(obj));
    for (size_t i = 0; i < strlen(str); i++) {
        ASSERT_EQ(str[i], string_index(obj, i));
    }
    for (size_t i = 0; i < strlen(str); i++) {
        ASSERT_EQ(str[i], string_get_cstr(obj)[i]);
    }
    ASSERT_EQ(string_index(obj, string_len(obj)), '\0');
    ASSERT_FALSE(is_null(obj));
    ASSERT_FALSE(is_imm(obj));
    ASSERT_FALSE(is_unique_imm(obj));
    ASSERT_FALSE(is_i64_real(obj));
    ASSERT_FALSE(is_imm_i64(obj));
    ASSERT_FALSE(is_imm_char(obj));
    ASSERT_TRUE(is_object(obj));
    ASSERT_FALSE(is_i64(obj));
    ASSERT_FALSE(is_doublenum(obj));
    ASSERT_FALSE(is_pair(obj));
    ASSERT_TRUE(is_string(obj));
    ASSERT_FALSE(is_symbol(obj));
    ASSERT_FALSE(is_vector(obj));
    ASSERT_FALSE(is_bytes(obj));
    ASSERT_FALSE(is_string_buffer(obj));
    ASSERT_FALSE(is_stack(obj));
    ASSERT_FALSE(is_string_port(obj));
    ASSERT_FALSE(is_string_port_input(obj));
    ASSERT_FALSE(is_string_port_output(obj));
    ASSERT_FALSE(is_string_port_in_out_put(obj));
    ASSERT_FALSE(is_string_port_eof(obj));
    ASSERT_FALSE(is_stdio_port(obj));
    ASSERT_FALSE(is_stdio_port_input(obj));
    ASSERT_FALSE(is_stdio_port_output(obj));
    ASSERT_FALSE(is_stdio_port_in_out_put(obj));
    ASSERT_FALSE(is_stdio_port_eof(obj));
    ASSERT_FALSE(is_port_input(obj));
    ASSERT_FALSE(is_port_output(obj));
    ASSERT_FALSE(is_port_in_out_put(obj));
    ASSERT_FALSE(is_port_eof(obj));
    ASSERT_FALSE(is_srfi6_port(obj));

    start = utest_ns();
    gc_collect(context);
    time = utest_ns() - start;
    if (gc_print) {
        UTEST_PRINTF("gc time: %"
                             PRId64
                             " ns\n", time);
    }

    obj = string_make_from_cstr_op(context, NULL);
    ASSERT_EQ(0, string_len(obj));
    ASSERT_EQ('\0', string_index(obj, 0));
    ASSERT_FALSE(is_null(obj));
    ASSERT_FALSE(is_imm(obj));
    ASSERT_FALSE(is_unique_imm(obj));
    ASSERT_FALSE(is_i64_real(obj));
    ASSERT_FALSE(is_imm_i64(obj));
    ASSERT_FALSE(is_imm_char(obj));
    ASSERT_TRUE(is_object(obj));
    ASSERT_FALSE(is_i64(obj));
    ASSERT_FALSE(is_doublenum(obj));
    ASSERT_FALSE(is_pair(obj));
    ASSERT_TRUE(is_string(obj));
    ASSERT_FALSE(is_symbol(obj));
    ASSERT_FALSE(is_vector(obj));
    ASSERT_FALSE(is_bytes(obj));
    ASSERT_FALSE(is_string_buffer(obj));
    ASSERT_FALSE(is_stack(obj));
    ASSERT_FALSE(is_string_port(obj));
    ASSERT_FALSE(is_string_port_input(obj));
    ASSERT_FALSE(is_string_port_output(obj));
    ASSERT_FALSE(is_string_port_in_out_put(obj));
    ASSERT_FALSE(is_string_port_eof(obj));
    ASSERT_FALSE(is_stdio_port(obj));
    ASSERT_FALSE(is_stdio_port_input(obj));
    ASSERT_FALSE(is_stdio_port_output(obj));
    ASSERT_FALSE(is_stdio_port_in_out_put(obj));
    ASSERT_FALSE(is_stdio_port_eof(obj));
    ASSERT_FALSE(is_port_input(obj));
    ASSERT_FALSE(is_port_output(obj));
    ASSERT_FALSE(is_port_in_out_put(obj));
    ASSERT_FALSE(is_port_eof(obj));
    ASSERT_FALSE(is_srfi6_port(obj));

    start = utest_ns();
    gc_collect(context);
    time = utest_ns() - start;
    if (gc_print) {
        UTEST_PRINTF("gc time: %"
                             PRId64
                             " ns\n", time);
    }

    obj = string_make_from_cstr_op(context, "");
    ASSERT_EQ(0, string_len(obj));
    ASSERT_EQ('\0', string_index(obj, 0));
    ASSERT_FALSE(is_null(obj));
    ASSERT_FALSE(is_imm(obj));
    ASSERT_FALSE(is_unique_imm(obj));
    ASSERT_FALSE(is_i64_real(obj));
    ASSERT_FALSE(is_imm_i64(obj));
    ASSERT_FALSE(is_imm_char(obj));
    ASSERT_TRUE(is_object(obj));
    ASSERT_FALSE(is_i64(obj));
    ASSERT_FALSE(is_doublenum(obj));
    ASSERT_FALSE(is_pair(obj));
    ASSERT_TRUE(is_string(obj));
    ASSERT_FALSE(is_symbol(obj));
    ASSERT_FALSE(is_vector(obj));
    ASSERT_FALSE(is_bytes(obj));
    ASSERT_FALSE(is_string_buffer(obj));
    ASSERT_FALSE(is_stack(obj));
    ASSERT_FALSE(is_string_port(obj));
    ASSERT_FALSE(is_string_port_input(obj));
    ASSERT_FALSE(is_string_port_output(obj));
    ASSERT_FALSE(is_string_port_in_out_put(obj));
    ASSERT_FALSE(is_string_port_eof(obj));
    ASSERT_FALSE(is_stdio_port(obj));
    ASSERT_FALSE(is_stdio_port_input(obj));
    ASSERT_FALSE(is_stdio_port_output(obj));
    ASSERT_FALSE(is_stdio_port_in_out_put(obj));
    ASSERT_FALSE(is_stdio_port_eof(obj));
    ASSERT_FALSE(is_port_input(obj));
    ASSERT_FALSE(is_port_output(obj));
    ASSERT_FALSE(is_port_in_out_put(obj));
    ASSERT_FALSE(is_port_eof(obj));
    ASSERT_FALSE(is_srfi6_port(obj));

    start = utest_ns();
    gc_collect(context);
    time = utest_ns() - start;
    if (gc_print) {
        UTEST_PRINTF("gc time: %"
                             PRId64
                             " ns\n", time);
    }

    // symbol
    char symbol_str[] = "this is a symbol object";
    obj = symbol_make_from_cstr_op(context, symbol_str);
    ASSERT_EQ(strlen(str), symbol_len(obj));
    for (size_t i = 0; i <= strlen(symbol_str); i++) {
        ASSERT_EQ(symbol_str[i], symbol_index(obj, i));
    }
    for (size_t i = 0; i <= strlen(symbol_str); i++) {
        ASSERT_EQ(symbol_str[i], symbol_get_cstr(obj)[i]);
    }
    ASSERT_EQ(symbol_index(obj, symbol_len(obj)), '\0');
    ASSERT_FALSE(is_null(obj));
    ASSERT_FALSE(is_imm(obj));
    ASSERT_FALSE(is_unique_imm(obj));
    ASSERT_FALSE(is_i64_real(obj));
    ASSERT_FALSE(is_imm_i64(obj));
    ASSERT_FALSE(is_imm_char(obj));
    ASSERT_TRUE(is_object(obj));
    ASSERT_FALSE(is_i64(obj));
    ASSERT_FALSE(is_doublenum(obj));
    ASSERT_FALSE(is_pair(obj));
    ASSERT_FALSE(is_string(obj));
    ASSERT_TRUE(is_symbol(obj));
    ASSERT_FALSE(is_vector(obj));
    ASSERT_FALSE(is_bytes(obj));
    ASSERT_FALSE(is_string_buffer(obj));
    ASSERT_FALSE(is_stack(obj));
    ASSERT_FALSE(is_string_port(obj));
    ASSERT_FALSE(is_string_port_input(obj));
    ASSERT_FALSE(is_string_port_output(obj));
    ASSERT_FALSE(is_string_port_in_out_put(obj));
    ASSERT_FALSE(is_string_port_eof(obj));
    ASSERT_FALSE(is_stdio_port(obj));
    ASSERT_FALSE(is_stdio_port_input(obj));
    ASSERT_FALSE(is_stdio_port_output(obj));
    ASSERT_FALSE(is_stdio_port_in_out_put(obj));
    ASSERT_FALSE(is_stdio_port_eof(obj));
    ASSERT_FALSE(is_port_input(obj));
    ASSERT_FALSE(is_port_output(obj));
    ASSERT_FALSE(is_port_in_out_put(obj));
    ASSERT_FALSE(is_port_eof(obj));
    ASSERT_FALSE(is_srfi6_port(obj));

    start = utest_ns();
    gc_collect(context);
    time = utest_ns() - start;
    if (gc_print) {
        UTEST_PRINTF("gc time: %"
                             PRId64
                             " ns\n", time);
    }

    obj = symbol_make_from_cstr_op(context, NULL);
    ASSERT_EQ(0, symbol_len(obj));
    ASSERT_EQ('\0', symbol_index(obj, 0));
    ASSERT_FALSE(is_null(obj));
    ASSERT_FALSE(is_imm(obj));
    ASSERT_FALSE(is_unique_imm(obj));
    ASSERT_FALSE(is_i64_real(obj));
    ASSERT_FALSE(is_imm_i64(obj));
    ASSERT_FALSE(is_imm_char(obj));
    ASSERT_TRUE(is_object(obj));
    ASSERT_FALSE(is_i64(obj));
    ASSERT_FALSE(is_doublenum(obj));
    ASSERT_FALSE(is_pair(obj));
    ASSERT_FALSE(is_string(obj));
    ASSERT_TRUE(is_symbol(obj));
    ASSERT_FALSE(is_vector(obj));
    ASSERT_FALSE(is_bytes(obj));
    ASSERT_FALSE(is_string_buffer(obj));
    ASSERT_FALSE(is_stack(obj));
    ASSERT_FALSE(is_string_port(obj));
    ASSERT_FALSE(is_string_port_input(obj));
    ASSERT_FALSE(is_string_port_output(obj));
    ASSERT_FALSE(is_string_port_in_out_put(obj));
    ASSERT_FALSE(is_string_port_eof(obj));
    ASSERT_FALSE(is_stdio_port(obj));
    ASSERT_FALSE(is_stdio_port_input(obj));
    ASSERT_FALSE(is_stdio_port_output(obj));
    ASSERT_FALSE(is_stdio_port_in_out_put(obj));
    ASSERT_FALSE(is_stdio_port_eof(obj));
    ASSERT_FALSE(is_port_input(obj));
    ASSERT_FALSE(is_port_output(obj));
    ASSERT_FALSE(is_port_in_out_put(obj));
    ASSERT_FALSE(is_port_eof(obj));
    ASSERT_FALSE(is_srfi6_port(obj));

    start = utest_ns();
    gc_collect(context);
    time = utest_ns() - start;
    if (gc_print) {
        UTEST_PRINTF("gc time: %"
                             PRId64
                             " ns\n", time);
    }

    obj = symbol_make_from_cstr_op(context, "");
    ASSERT_EQ(0, symbol_len(obj));
    ASSERT_EQ('\0', symbol_index(obj, 0));
    ASSERT_FALSE(is_null(obj));
    ASSERT_FALSE(is_imm(obj));
    ASSERT_FALSE(is_unique_imm(obj));
    ASSERT_FALSE(is_i64_real(obj));
    ASSERT_FALSE(is_imm_i64(obj));
    ASSERT_FALSE(is_imm_char(obj));
    ASSERT_TRUE(is_object(obj));
    ASSERT_FALSE(is_i64(obj));
    ASSERT_FALSE(is_doublenum(obj));
    ASSERT_FALSE(is_pair(obj));
    ASSERT_FALSE(is_string(obj));
    ASSERT_TRUE(is_symbol(obj));
    ASSERT_FALSE(is_vector(obj));
    ASSERT_FALSE(is_bytes(obj));
    ASSERT_FALSE(is_string_buffer(obj));
    ASSERT_FALSE(is_stack(obj));
    ASSERT_FALSE(is_string_port(obj));
    ASSERT_FALSE(is_string_port_input(obj));
    ASSERT_FALSE(is_string_port_output(obj));
    ASSERT_FALSE(is_string_port_in_out_put(obj));
    ASSERT_FALSE(is_string_port_eof(obj));
    ASSERT_FALSE(is_stdio_port(obj));
    ASSERT_FALSE(is_stdio_port_input(obj));
    ASSERT_FALSE(is_stdio_port_output(obj));
    ASSERT_FALSE(is_stdio_port_in_out_put(obj));
    ASSERT_FALSE(is_stdio_port_eof(obj));
    ASSERT_FALSE(is_port_input(obj));
    ASSERT_FALSE(is_port_output(obj));
    ASSERT_FALSE(is_port_in_out_put(obj));
    ASSERT_FALSE(is_port_eof(obj));
    ASSERT_FALSE(is_srfi6_port(obj));

    start = utest_ns();
    gc_collect(context);
    time = utest_ns() - start;
    if (gc_print) {
        UTEST_PRINTF("gc time: %"
                             PRId64
                             " ns\n", time);
    }

    // vector
    obj = vector_make_op(context, 10);
    for (size_t i = 0; i < vector_len(obj); i++) {
        ASSERT_EQ(IMM_UNIT, vector_ref(obj, i));
    }
    vector_ref(obj, 1) = imm_i64;
    ASSERT_EQ(vector_ref(obj, 1), imm_i64);
    ASSERT_EQ(10, vector_len(obj));
    ASSERT_FALSE(is_null(obj));
    ASSERT_FALSE(is_imm(obj));
    ASSERT_FALSE(is_unique_imm(obj));
    ASSERT_FALSE(is_i64_real(obj));
    ASSERT_FALSE(is_imm_i64(obj));
    ASSERT_FALSE(is_imm_char(obj));
    ASSERT_TRUE(is_object(obj));
    ASSERT_FALSE(is_i64(obj));
    ASSERT_FALSE(is_doublenum(obj));
    ASSERT_FALSE(is_pair(obj));
    ASSERT_FALSE(is_string(obj));
    ASSERT_FALSE(is_symbol(obj));
    ASSERT_TRUE(is_vector(obj));
    ASSERT_FALSE(is_bytes(obj));
    ASSERT_FALSE(is_string_buffer(obj));
    ASSERT_FALSE(is_stack(obj));
    ASSERT_FALSE(is_string_port(obj));
    ASSERT_FALSE(is_string_port_input(obj));
    ASSERT_FALSE(is_string_port_output(obj));
    ASSERT_FALSE(is_string_port_in_out_put(obj));
    ASSERT_FALSE(is_string_port_eof(obj));
    ASSERT_FALSE(is_stdio_port(obj));
    ASSERT_FALSE(is_stdio_port_input(obj));
    ASSERT_FALSE(is_stdio_port_output(obj));
    ASSERT_FALSE(is_stdio_port_in_out_put(obj));
    ASSERT_FALSE(is_stdio_port_eof(obj));
    ASSERT_FALSE(is_port_input(obj));
    ASSERT_FALSE(is_port_output(obj));
    ASSERT_FALSE(is_port_in_out_put(obj));
    ASSERT_FALSE(is_port_eof(obj));
    ASSERT_FALSE(is_srfi6_port(obj));

    vector_set(obj, 0, char_imm_make('x'));
    obj = vector_capacity_increase(context, obj, 10);
    ASSERT_EQ(vector_ref(obj, 0), char_imm_make('x'));
    ASSERT_EQ(vector_ref(obj, 1), imm_i64);
    for (size_t i = 2; i < vector_len(obj); i++) {
        ASSERT_EQ(IMM_UNIT, vector_ref(obj, i));
    }
    ASSERT_EQ(20, vector_len(obj));
    ASSERT_FALSE(is_null(obj));
    ASSERT_FALSE(is_imm(obj));
    ASSERT_FALSE(is_unique_imm(obj));
    ASSERT_FALSE(is_i64_real(obj));
    ASSERT_FALSE(is_imm_i64(obj));
    ASSERT_FALSE(is_imm_char(obj));
    ASSERT_TRUE(is_object(obj));
    ASSERT_FALSE(is_i64(obj));
    ASSERT_FALSE(is_doublenum(obj));
    ASSERT_FALSE(is_pair(obj));
    ASSERT_FALSE(is_string(obj));
    ASSERT_FALSE(is_symbol(obj));
    ASSERT_TRUE(is_vector(obj));
    ASSERT_FALSE(is_bytes(obj));
    ASSERT_FALSE(is_string_buffer(obj));
    ASSERT_FALSE(is_stack(obj));
    ASSERT_FALSE(is_string_port(obj));
    ASSERT_FALSE(is_string_port_input(obj));
    ASSERT_FALSE(is_string_port_output(obj));
    ASSERT_FALSE(is_string_port_in_out_put(obj));
    ASSERT_FALSE(is_string_port_eof(obj));
    ASSERT_FALSE(is_stdio_port(obj));
    ASSERT_FALSE(is_stdio_port_input(obj));
    ASSERT_FALSE(is_stdio_port_output(obj));
    ASSERT_FALSE(is_stdio_port_in_out_put(obj));
    ASSERT_FALSE(is_stdio_port_eof(obj));
    ASSERT_FALSE(is_port_input(obj));
    ASSERT_FALSE(is_port_output(obj));
    ASSERT_FALSE(is_port_in_out_put(obj));
    ASSERT_FALSE(is_port_eof(obj));
    ASSERT_FALSE(is_srfi6_port(obj));

    start = utest_ns();
    gc_collect(context);
    time = utest_ns() - start;
    if (gc_print) {
        UTEST_PRINTF("gc time: %"
                             PRId64
                             " ns\n", time);
    }

    obj = vector_make_op(context, 20);
    for (size_t i = 0; i < vector_len(obj); i++) {
        ASSERT_EQ(IMM_UNIT, vector_ref(obj, i));
    }
    ASSERT_EQ(20, vector_len(obj));
    ASSERT_FALSE(is_null(obj));
    ASSERT_FALSE(is_imm(obj));
    ASSERT_FALSE(is_unique_imm(obj));
    ASSERT_FALSE(is_i64_real(obj));
    ASSERT_FALSE(is_imm_i64(obj));
    ASSERT_FALSE(is_imm_char(obj));
    ASSERT_TRUE(is_object(obj));
    ASSERT_FALSE(is_i64(obj));
    ASSERT_FALSE(is_doublenum(obj));
    ASSERT_FALSE(is_pair(obj));
    ASSERT_FALSE(is_string(obj));
    ASSERT_FALSE(is_symbol(obj));
    ASSERT_TRUE(is_vector(obj));
    ASSERT_FALSE(is_bytes(obj));
    ASSERT_FALSE(is_string_buffer(obj));
    ASSERT_FALSE(is_stack(obj));
    ASSERT_FALSE(is_string_port(obj));
    ASSERT_FALSE(is_string_port_input(obj));
    ASSERT_FALSE(is_string_port_output(obj));
    ASSERT_FALSE(is_string_port_in_out_put(obj));
    ASSERT_FALSE(is_string_port_eof(obj));
    ASSERT_FALSE(is_stdio_port(obj));
    ASSERT_FALSE(is_stdio_port_input(obj));
    ASSERT_FALSE(is_stdio_port_output(obj));
    ASSERT_FALSE(is_stdio_port_in_out_put(obj));
    ASSERT_FALSE(is_stdio_port_eof(obj));
    ASSERT_FALSE(is_port_input(obj));
    ASSERT_FALSE(is_port_output(obj));
    ASSERT_FALSE(is_port_in_out_put(obj));
    ASSERT_FALSE(is_port_eof(obj));
    ASSERT_FALSE(is_srfi6_port(obj));

    start = utest_ns();
    gc_collect(context);
    time = utest_ns() - start;
    if (gc_print) {
        UTEST_PRINTF("gc time: %"
                             PRId64
                             " ns\n", time);
    }

    // vector
    obj = vector_make_op(context, 0);
    for (size_t i = 0; i < vector_len(obj); i++) {
        ASSERT_EQ(IMM_UNIT, vector_ref(obj, i));
    }
    ASSERT_EQ(0, vector_len(obj));
    ASSERT_FALSE(is_null(obj));
    ASSERT_FALSE(is_imm(obj));
    ASSERT_FALSE(is_unique_imm(obj));
    ASSERT_FALSE(is_i64_real(obj));
    ASSERT_FALSE(is_imm_i64(obj));
    ASSERT_FALSE(is_imm_char(obj));
    ASSERT_TRUE(is_object(obj));
    ASSERT_FALSE(is_i64(obj));
    ASSERT_FALSE(is_doublenum(obj));
    ASSERT_FALSE(is_pair(obj));
    ASSERT_FALSE(is_string(obj));
    ASSERT_FALSE(is_symbol(obj));
    ASSERT_TRUE(is_vector(obj));
    ASSERT_FALSE(is_bytes(obj));
    ASSERT_FALSE(is_string_buffer(obj));
    ASSERT_FALSE(is_stack(obj));
    ASSERT_FALSE(is_string_port(obj));
    ASSERT_FALSE(is_string_port_input(obj));
    ASSERT_FALSE(is_string_port_output(obj));
    ASSERT_FALSE(is_string_port_in_out_put(obj));
    ASSERT_FALSE(is_string_port_eof(obj));
    ASSERT_FALSE(is_stdio_port(obj));
    ASSERT_FALSE(is_stdio_port_input(obj));
    ASSERT_FALSE(is_stdio_port_output(obj));
    ASSERT_FALSE(is_stdio_port_in_out_put(obj));
    ASSERT_FALSE(is_stdio_port_eof(obj));
    ASSERT_FALSE(is_port_input(obj));
    ASSERT_FALSE(is_port_output(obj));
    ASSERT_FALSE(is_port_in_out_put(obj));
    ASSERT_FALSE(is_port_eof(obj));
    ASSERT_FALSE(is_srfi6_port(obj));

    start = utest_ns();
    gc_collect(context);
    time = utest_ns() - start;
    if (gc_print) {
        UTEST_PRINTF("gc time: %"
                             PRId64
                             " ns\n", time);
    }

    // bytes
    obj = bytes_make_op(context, 10);
    for (size_t i = 0; bytes_capacity(obj) > i; i++) {
        ASSERT_EQ(bytes_index(obj, i), 0);
    }
    bytes_index(obj, 0) = 2;
    ASSERT_EQ(bytes_data(obj), obj->value.bytes.data);
    ASSERT_EQ(obj->value.bytes.data[0], bytes_index(obj, 0));
    ASSERT_EQ(2, bytes_index(obj, 0));
    ASSERT_EQ(10, bytes_capacity(obj));
    ASSERT_FALSE(is_null(obj));
    ASSERT_FALSE(is_imm(obj));
    ASSERT_FALSE(is_unique_imm(obj));
    ASSERT_FALSE(is_i64_real(obj));
    ASSERT_FALSE(is_imm_i64(obj));
    ASSERT_FALSE(is_imm_char(obj));
    ASSERT_TRUE(is_object(obj));
    ASSERT_FALSE(is_i64(obj));
    ASSERT_FALSE(is_doublenum(obj));
    ASSERT_FALSE(is_pair(obj));
    ASSERT_FALSE(is_string(obj));
    ASSERT_FALSE(is_symbol(obj));
    ASSERT_FALSE(is_vector(obj));
    ASSERT_TRUE(is_bytes(obj));
    ASSERT_FALSE(is_string_buffer(obj));
    ASSERT_FALSE(is_stack(obj));
    ASSERT_FALSE(is_string_port(obj));
    ASSERT_FALSE(is_string_port_input(obj));
    ASSERT_FALSE(is_string_port_output(obj));
    ASSERT_FALSE(is_string_port_in_out_put(obj));
    ASSERT_FALSE(is_string_port_eof(obj));
    ASSERT_FALSE(is_stdio_port(obj));
    ASSERT_FALSE(is_stdio_port_input(obj));
    ASSERT_FALSE(is_stdio_port_output(obj));
    ASSERT_FALSE(is_stdio_port_in_out_put(obj));
    ASSERT_FALSE(is_stdio_port_eof(obj));
    ASSERT_FALSE(is_port_input(obj));
    ASSERT_FALSE(is_port_output(obj));
    ASSERT_FALSE(is_port_in_out_put(obj));
    ASSERT_FALSE(is_port_eof(obj));
    ASSERT_FALSE(is_srfi6_port(obj));

    start = utest_ns();
    gc_collect(context);
    time = utest_ns() - start;
    if (gc_print) {
        UTEST_PRINTF("gc time: %"
                             PRId64
                             " ns\n", time);
    }


    // string buffer
    obj = string_buffer_make_op(context, 1);
    ASSERT_GE(string_buffer_capacity(obj), string_buffer_length(obj));
    ASSERT_TRUE(string_buffer_empty(obj));
    ASSERT_FALSE(string_buffer_full(obj));
    tmp1 = string_buffer_to_string(context, obj);
    ASSERT_EQ(string_len(obj), 0);
    ASSERT_EQ(memcmp(string_get_cstr(tmp1), "\0", string_len(tmp1) + 1), 0);

    start = utest_ns();
    gc_collect(context);
    time = utest_ns() - start;
    if (gc_print) {
        UTEST_PRINTF("gc time: %"
                             PRId64
                             " ns\n", time);
    }

    string_buffer_append_imm_char_op(context, obj, char_imm_make('a'));
    ASSERT_GE(string_buffer_capacity(obj), string_buffer_length(obj));
    ASSERT_EQ('a', string_buffer_index(obj, 0));
    ASSERT_TRUE(memcmp(string_buffer_bytes_data(obj), "a", 1) == 0);
    ASSERT_EQ(1, string_buffer_length(obj));
    ASSERT_FALSE(string_buffer_empty(obj));
    ASSERT_TRUE(string_buffer_full(obj));
    tmp1 = string_buffer_to_string(context, obj);
    ASSERT_EQ(string_len(tmp1), 1);
    ASSERT_EQ(memcmp(string_get_cstr(tmp1), "a\0", string_len(tmp1) + 1), 0);

    start = utest_ns();
    gc_collect(context);
    time = utest_ns() - start;
    if (gc_print) {
        UTEST_PRINTF("gc time: %"
                             PRId64
                             " ns\n", time);
    }

    string_buffer_append_imm_char_op(context, obj, char_imm_make('b'));
    ASSERT_GE(string_buffer_capacity(obj), string_buffer_length(obj));
    ASSERT_EQ('b', string_buffer_index(obj, 1));
    ASSERT_TRUE(memcmp(string_buffer_bytes_data(obj), "ab", 2) == 0);
    ASSERT_EQ(2, string_buffer_length(obj));
    ASSERT_FALSE(string_buffer_empty(obj));
    ASSERT_FALSE(string_buffer_full(obj));
    tmp1 = string_buffer_to_string(context, obj);
    ASSERT_EQ(string_len(tmp1), 2);
    ASSERT_EQ(memcmp(string_get_cstr(tmp1), "ab\0", string_len(tmp1) + 1), 0);

    start = utest_ns();
    gc_collect(context);
    time = utest_ns() - start;
    if (gc_print) {
        UTEST_PRINTF("gc time: %"
                             PRId64
                             " ns\n", time);
    }

    tmp1 = string_make_from_cstr_op(context, "cdefg");
    string_buffer_append_string_op(context, obj, tmp1);
    ASSERT_GE(string_buffer_capacity(obj), string_buffer_length(obj));
    ASSERT_TRUE(memcmp(string_buffer_bytes_data(obj), "abcdefg", 7) == 0);
    ASSERT_EQ(7, string_buffer_length(obj));
    ASSERT_FALSE(string_buffer_empty(obj));
    ASSERT_FALSE(string_buffer_full(obj));
    tmp1 = string_buffer_to_string(context, obj);
    ASSERT_EQ(string_len(tmp1), 7);
    ASSERT_EQ(memcmp(string_get_cstr(tmp1), "abcdefg\0", string_len(tmp1) + 1), 0);

    start = utest_ns();
    gc_collect(context);
    time = utest_ns() - start;
    if (gc_print) {
        UTEST_PRINTF("gc time: %"
                             PRId64
                             " ns\n", time);
    }

    tmp1 = char_to_string(context, 'h');
    string_buffer_append_string_op(context, obj, tmp1);
    ASSERT_GE(string_buffer_capacity(obj), string_buffer_length(obj));
    ASSERT_TRUE(memcmp(string_buffer_bytes_data(obj), "abcdefgh", 8) == 0);
    ASSERT_EQ(8, string_buffer_length(obj));
    ASSERT_FALSE(string_buffer_empty(obj));
    ASSERT_FALSE(string_buffer_full(obj));
    tmp1 = string_buffer_to_string(context, obj);
    ASSERT_EQ(string_len(tmp1), 8);
    ASSERT_EQ(memcmp(string_get_cstr(tmp1), "abcdefgh\0", string_len(tmp1) + 1), 0);

    start = utest_ns();
    gc_collect(context);
    time = utest_ns() - start;
    if (gc_print) {
        UTEST_PRINTF("gc time: %"
                             PRId64
                             " ns\n", time);
    }

    tmp1 = imm_char_to_string(context, char_imm_make('i'));
    string_buffer_append_string_op(context, obj, tmp1);
    ASSERT_GE(string_buffer_capacity(obj), string_buffer_length(obj));
    ASSERT_TRUE(memcmp(string_buffer_bytes_data(obj), "abcdefghi", 9) == 0);
    ASSERT_EQ(9, string_buffer_length(obj));
    ASSERT_FALSE(string_buffer_empty(obj));
    ASSERT_FALSE(string_buffer_full(obj));
    tmp1 = string_buffer_to_string(context, obj);
    ASSERT_EQ(string_len(tmp1), 9);
    ASSERT_EQ(memcmp(string_get_cstr(tmp1), "abcdefghi\0", string_len(tmp1) + 1), 0);

    start = utest_ns();
    gc_collect(context);
    time = utest_ns() - start;
    if (gc_print) {
        UTEST_PRINTF("gc time: %"
                             PRId64
                             " ns\n", time);
    }

    tmp1 = imm_char_to_string(context, char_imm_make('\0'));
    string_buffer_append_string_op(context, obj, tmp1);
    ASSERT_GE(string_buffer_capacity(obj), string_buffer_length(obj));
    ASSERT_TRUE(memcmp(string_buffer_bytes_data(obj), "abcdefghi", 9) == 0);
    ASSERT_EQ(9, string_buffer_length(obj));
    ASSERT_FALSE(string_buffer_empty(obj));
    ASSERT_FALSE(string_buffer_full(obj));
    tmp1 = string_buffer_to_string(context, obj);
    ASSERT_EQ(string_len(tmp1), 9);
    ASSERT_EQ(memcmp(string_get_cstr(tmp1), "abcdefghi\0", string_len(tmp1) + 1), 0);

    start = utest_ns();
    gc_collect(context);
    time = utest_ns() - start;
    if (gc_print) {
        UTEST_PRINTF("gc time: %"
                             PRId64
                             " ns\n", time);
    }

    tmp1 = string_make_from_cstr_op(context, "\0\0abcabc\0");
    string_buffer_append_string_op(context, obj, tmp1);
    ASSERT_GE(string_buffer_capacity(obj), string_buffer_length(obj));
    ASSERT_TRUE(memcmp(string_buffer_bytes_data(obj), "abcdefghi", 9) == 0);
    ASSERT_EQ(9, string_buffer_length(obj));
    ASSERT_FALSE(string_buffer_empty(obj));
    ASSERT_FALSE(string_buffer_full(obj));
    tmp1 = string_buffer_to_string(context, obj);
    ASSERT_EQ(string_len(tmp1), 9);
    ASSERT_EQ(memcmp(string_get_cstr(tmp1), "abcdefghi\0", string_len(tmp1) + 1), 0);

    start = utest_ns();
    gc_collect(context);
    time = utest_ns() - start;
    if (gc_print) {
        UTEST_PRINTF("gc time: %"
                             PRId64
                             " ns\n", time);
    }

    ASSERT_FALSE(is_null(obj));
    ASSERT_FALSE(is_imm(obj));
    ASSERT_FALSE(is_unique_imm(obj));
    ASSERT_FALSE(is_i64_real(obj));
    ASSERT_FALSE(is_imm_i64(obj));
    ASSERT_FALSE(is_imm_char(obj));
    ASSERT_TRUE(is_object(obj));
    ASSERT_FALSE(is_i64(obj));
    ASSERT_FALSE(is_doublenum(obj));
    ASSERT_FALSE(is_pair(obj));
    ASSERT_FALSE(is_string(obj));
    ASSERT_FALSE(is_symbol(obj));
    ASSERT_FALSE(is_vector(obj));
    ASSERT_FALSE(is_bytes(obj));
    ASSERT_TRUE(is_string_buffer(obj));
    ASSERT_FALSE(is_stack(obj));
    ASSERT_FALSE(is_string_port(obj));
    ASSERT_FALSE(is_string_port_input(obj));
    ASSERT_FALSE(is_string_port_output(obj));
    ASSERT_FALSE(is_string_port_in_out_put(obj));
    ASSERT_FALSE(is_string_port_eof(obj));
    ASSERT_FALSE(is_stdio_port(obj));
    ASSERT_FALSE(is_stdio_port_input(obj));
    ASSERT_FALSE(is_stdio_port_output(obj));
    ASSERT_FALSE(is_stdio_port_in_out_put(obj));
    ASSERT_FALSE(is_stdio_port_eof(obj));
    ASSERT_FALSE(is_port_input(obj));
    ASSERT_FALSE(is_port_output(obj));
    ASSERT_FALSE(is_port_in_out_put(obj));
    ASSERT_FALSE(is_port_eof(obj));
    ASSERT_FALSE(is_srfi6_port(obj));

    tmp1 = string_make_from_cstr_op(context, "abc");
    obj = string_buffer_make_from_string_op(context, tmp1);
    tmp1 = imm_char_to_string(context, char_imm_make('i'));
    string_buffer_append_string_op(context, obj, tmp1);
    ASSERT_GE(string_buffer_capacity(obj), string_buffer_length(obj));
    ASSERT_TRUE(memcmp(string_buffer_bytes_data(obj), "abci", 4) == 0);
    ASSERT_EQ(4, string_buffer_length(obj));
    ASSERT_FALSE(string_buffer_empty(obj));
    ASSERT_FALSE(string_buffer_full(obj));
    tmp1 = string_buffer_to_string(context, obj);
    ASSERT_EQ(string_len(tmp1), 4);
    ASSERT_EQ(memcmp(string_get_cstr(tmp1), "abci\0", string_len(tmp1) + 1), 0);

    start = utest_ns();
    gc_collect(context);
    time = utest_ns() - start;
    if (gc_print) {
        UTEST_PRINTF("gc time: %"
                             PRId64
                             " ns\n", time);
    }

    // string append
    tmp1 = string_make_from_cstr_op(context, "abc");
    tmp2 = string_make_from_cstr_op(context, "def");
    obj = string_append_op(context, tmp1, tmp2);
    ASSERT_EQ(memcmp(string_get_cstr(obj), "abcdef\0", string_len(obj) + 1), 0);
    ASSERT_EQ(6, string_len(obj));

    start = utest_ns();
    gc_collect(context);
    time = utest_ns() - start;
    if (gc_print) {
        UTEST_PRINTF("gc time: %"
                             PRId64
                             " ns\n", time);
    }

    // stack
    const size_t stack_size = 2;
    obj = stack_make_op(context, stack_size);
    ASSERT_TRUE(is_stack(obj));
    ASSERT_TRUE(stack_empty(obj));
    ASSERT_FALSE(stack_full(obj));
    ASSERT_EQ(stack_len(obj), 0);
    ASSERT_EQ(stack_peek(obj), NULL);
    ASSERT_EQ(stack_capacity(obj), stack_size);
    stack_clean(obj);
    ASSERT_TRUE(stack_empty(obj));
    ASSERT_EQ(stack_len(obj), 0);
    ASSERT_EQ(stack_peek(obj), NULL);
    ASSERT_EQ(stack_capacity(obj), stack_size);

    start = utest_ns();
    gc_collect(context);
    time = utest_ns() - start;
    if (gc_print) {
        UTEST_PRINTF("gc time: %"
                             PRId64
                             " ns\n", time);
    }

        // { a, }
    ASSERT_TRUE(stack_push(obj, char_imm_make('a')));
    ASSERT_FALSE(stack_empty(obj));
    ASSERT_FALSE(stack_full(obj));
    ASSERT_EQ(stack_len(obj), 1);
    ASSERT_EQ(stack_peek(obj), char_imm_make('a'));
    ASSERT_EQ(stack_capacity(obj), stack_size);

    start = utest_ns();
    gc_collect(context);
    time = utest_ns() - start;
    if (gc_print) {
        UTEST_PRINTF("gc time: %"
                             PRId64
                             " ns\n", time);
    }

        // { a, b }
    ASSERT_TRUE(stack_push(obj, char_imm_make('b')));
    ASSERT_FALSE(stack_empty(obj));
    ASSERT_TRUE(stack_full(obj));
    ASSERT_EQ(stack_len(obj), 2);
    ASSERT_EQ(stack_peek(obj), char_imm_make('b'));
    ASSERT_EQ(stack_capacity(obj), stack_size);

    start = utest_ns();
    gc_collect(context);
    time = utest_ns() - start;
    if (gc_print) {
        UTEST_PRINTF("gc time: %"
                             PRId64
                             " ns\n", time);
    }

        // { a, b }
    ASSERT_FALSE(stack_push(obj, char_imm_make('b')));
    ASSERT_FALSE(stack_empty(obj));
    ASSERT_TRUE(stack_full(obj));
    ASSERT_EQ(stack_len(obj), 2);
    ASSERT_EQ(stack_peek(obj), char_imm_make('b'));
    ASSERT_EQ(stack_capacity(obj), stack_size);

    start = utest_ns();
    gc_collect(context);
    time = utest_ns() - start;
    if (gc_print) {
        UTEST_PRINTF("gc time: %"
                             PRId64
                             " ns\n", time);
    }

        // { a, }
    ASSERT_TRUE(stack_pop(obj));
    ASSERT_FALSE(stack_empty(obj));
    ASSERT_FALSE(stack_full(obj));
    ASSERT_EQ(stack_len(obj), 1);
    ASSERT_EQ(stack_peek(obj), char_imm_make('a'));
    ASSERT_EQ(stack_capacity(obj), stack_size);

    start = utest_ns();
    gc_collect(context);
    time = utest_ns() - start;
    if (gc_print) {
        UTEST_PRINTF("gc time: %"
                             PRId64
                             " ns\n", time);
    }

        // { }
    ASSERT_TRUE(stack_pop(obj));
    ASSERT_TRUE(stack_empty(obj));
    ASSERT_FALSE(stack_full(obj));
    ASSERT_EQ(stack_len(obj), 0);
    ASSERT_EQ(stack_peek(obj), NULL);
    ASSERT_EQ(stack_capacity(obj), stack_size);

    start = utest_ns();
    gc_collect(context);
    time = utest_ns() - start;
    if (gc_print) {
        UTEST_PRINTF("gc time: %"
                             PRId64
                             " ns\n", time);
    }

        // { }
    ASSERT_FALSE(stack_pop(obj));
    ASSERT_TRUE(stack_empty(obj));
    ASSERT_FALSE(stack_full(obj));
    ASSERT_EQ(stack_len(obj), 0);
    ASSERT_EQ(stack_peek(obj), NULL);
    ASSERT_EQ(stack_capacity(obj), stack_size);

    start = utest_ns();
    gc_collect(context);
    time = utest_ns() - start;
    if (gc_print) {
        UTEST_PRINTF("gc time: %"
                             PRId64
                             " ns\n", time);
    }

    // { c }
    obj = stack_push_auto_increase(context, obj, char_imm_make('c'), 0);
    ASSERT_FALSE(stack_empty(obj));
    ASSERT_FALSE(stack_full(obj));
    ASSERT_EQ(stack_len(obj), 1);
    ASSERT_EQ(stack_peek(obj), char_imm_make('c'));
    ASSERT_EQ(stack_capacity(obj), stack_size);

    start = utest_ns();
    gc_collect(context);
    time = utest_ns() - start;
    if (gc_print) {
        UTEST_PRINTF("gc time: %"
                             PRId64
                             " ns\n", time);
    }

    // { c, d }
    obj = stack_push_auto_increase(context, obj, char_imm_make('d'), 0);
    ASSERT_FALSE(stack_empty(obj));
    ASSERT_TRUE(stack_full(obj));
    ASSERT_EQ(stack_len(obj), 2);
    ASSERT_EQ(stack_peek(obj), char_imm_make('d'));
    ASSERT_EQ(stack_capacity(obj), stack_size);

    start = utest_ns();
    gc_collect(context);
    time = utest_ns() - start;
    if (gc_print) {
        UTEST_PRINTF("gc time: %"
                             PRId64
                             " ns\n", time);
    }

    // { c, d, e }
    obj = stack_push_auto_increase(context, obj, char_imm_make('e'), 1);
    ASSERT_FALSE(stack_empty(obj));
    ASSERT_FALSE(stack_full(obj));
    ASSERT_EQ(stack_len(obj), 3);
    ASSERT_EQ(stack_peek(obj), char_imm_make('e'));
    ASSERT_EQ(stack_capacity(obj), stack_size + 2);

    start = utest_ns();
    gc_collect(context);
    time = utest_ns() - start;
    if (gc_print) {
        UTEST_PRINTF("gc time: %"
                             PRId64
                             " ns\n", time);
    }

    // { c, d, e, f }
    obj = stack_push_auto_increase(context, obj, char_imm_make('f'), 0);
    ASSERT_FALSE(stack_empty(obj));
    ASSERT_TRUE(stack_full(obj));
    ASSERT_EQ(stack_len(obj), 4);
    ASSERT_EQ(stack_peek(obj), char_imm_make('f'));
    ASSERT_EQ(stack_capacity(obj), stack_size + 2);

    start = utest_ns();
    gc_collect(context);
    time = utest_ns() - start;
    if (gc_print) {
        UTEST_PRINTF("gc time: %"
                             PRId64
                             " ns\n", time);
    }

    // { c, d, e, f, g }
    obj = stack_push_auto_increase(context, obj, char_imm_make('g'), 10);
    ASSERT_FALSE(stack_empty(obj));
    ASSERT_FALSE(stack_full(obj));
    ASSERT_EQ(stack_len(obj), 5);
    ASSERT_EQ(stack_peek(obj), char_imm_make('g'));
    ASSERT_EQ(stack_capacity(obj), stack_size + 2 + 11);

    start = utest_ns();
    gc_collect(context);
    time = utest_ns() - start;
    if (gc_print) {
        UTEST_PRINTF("gc time: %"
                             PRId64
                             " ns\n", time);
    }

        // { c }
    ASSERT_TRUE(stack_pop(obj));
    ASSERT_TRUE(stack_pop(obj));
    ASSERT_TRUE(stack_pop(obj));
    ASSERT_TRUE(stack_pop(obj));
    ASSERT_FALSE(stack_empty(obj));
    ASSERT_FALSE(stack_full(obj));
    ASSERT_EQ(stack_len(obj), 1);
    ASSERT_EQ(stack_peek(obj), char_imm_make('c'));
    ASSERT_EQ(stack_capacity(obj), stack_size + 2 + 11);

    start = utest_ns();
    gc_collect(context);
    time = utest_ns() - start;
    if (gc_print) {
        UTEST_PRINTF("gc time: %"
                             PRId64
                             " ns\n", time);
    }


    obj = IMM_UNIT;
    tmp1 = IMM_UNIT;
    tmp2 = IMM_UNIT;
    tmp3 = IMM_UNIT;
    gc_release_var(context);

    start = utest_ns();
    gc_collect(context);
    time = utest_ns() - start;
    UTEST_PRINTF("gc time: %"
                         PRId64
                         " ns\n", time);
    ASSERT_TRUE(context->heap->first_node->data == context->heap->first_node->free_ptr);

    ASSERT_EQ(context->saves, NULL);
    context_destroy(context);
}
