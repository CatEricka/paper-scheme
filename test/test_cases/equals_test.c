#include "lib/utest.h"
#include <paper-scheme/runtime.h>

UTEST(equals_test, value_equals_test) {
    context_t context = context_make(16, 2, 0x10000);;
    gc_var4(context, obj1, obj2, tmp1, tmp2);
    equals_fn equals, equals_assert;
    object_type_info_ptr type;

#define ASSERT_FUNC_EQ(x, y) ASSERT_EQ((void*)(x), (void*)(y))

    // i64
    obj1 = i64_imm_make(123);
    obj2 = i64_imm_make(123);
    type = context_get_object_type(context, obj1);
    equals_assert = type_info_equals(type);
    equals = object_equals_helper(context, obj1);
    ASSERT_FUNC_EQ(equals, equals_assert);
    ASSERT_FUNC_EQ(equals, i64_equals);
    ASSERT_TRUE(equals(context, obj1, obj2));


    //i64
    obj1 = i64_make_op(context, 615171298343471923);
    obj2 = i64_make_op(context, INT64_MAX);
    type = context_get_object_type(context, obj1);
    equals_assert = type_info_equals(type);
    equals = object_equals_helper(context, obj1);
    ASSERT_FUNC_EQ(equals, equals_assert);
    ASSERT_FUNC_EQ(equals, i64_equals);
    ASSERT_FALSE(equals(context, obj1, obj2));


    //i64
    obj1 = i64_make_op(context, INT64_MAX);
    obj2 = obj1;
    type = context_get_object_type(context, obj1);
    equals_assert = type_info_equals(type);
    equals = object_equals_helper(context, obj1);
    ASSERT_FUNC_EQ(equals, equals_assert);
    ASSERT_FUNC_EQ(equals, i64_equals);
    ASSERT_TRUE(equals(context, obj1, obj2));


    // char
    obj1 = char_imm_make('a');
    obj2 = char_imm_make('a');
    type = context_get_object_type(context, obj1);
    equals_assert = type_info_equals(type);
    equals = object_equals_helper(context, obj1);
    ASSERT_FUNC_EQ(equals, equals_assert);
    ASSERT_FUNC_EQ(equals, char_equals);
    ASSERT_TRUE(equals(context, obj1, obj2));



    // double
    obj1 = doublenum_make_op(context, 0.3);
    obj2 = doublenum_make_op(context, 0.3);
    type = context_get_object_type(context, obj1);
    equals_assert = type_info_equals(type);
    equals = object_equals_helper(context, obj1);
    ASSERT_FUNC_EQ(equals, equals_assert);
    ASSERT_FUNC_EQ(equals, d64_equals);
    ASSERT_TRUE(equals(context, obj1, obj2));
    ASSERT_TRUE(equals(context, obj1, obj1));


    // double
    obj1 = doublenum_make_op(context, 0.3);
    obj2 = doublenum_make_op(context, 0.4);
    type = context_get_object_type(context, obj1);
    equals_assert = type_info_equals(type);
    equals = object_equals_helper(context, obj1);
    ASSERT_FUNC_EQ(equals, equals_assert);
    ASSERT_FUNC_EQ(equals, d64_equals);
    ASSERT_FALSE(equals(context, obj1, obj2));
    ASSERT_TRUE(equals(context, obj2, obj2));


    // pair1
    tmp1 = string_make_from_cstr_op(context, "pair_car");
    tmp2 = symbol_make_from_cstr_op(context, "pair_cdr");
    obj1 = pair_make_op(context, tmp1, tmp2);
    tmp1 = string_make_from_cstr_op(context, "pair_car");
    tmp2 = symbol_make_from_cstr_op(context, "pair_cdr");
    obj2 = pair_make_op(context, tmp1, tmp2);
    type = context_get_object_type(context, obj1);
    equals_assert = type_info_equals(type);
    equals = object_equals_helper(context, obj1);
    ASSERT_FUNC_EQ(equals, equals_assert);
    ASSERT_FUNC_EQ(equals, pair_equals);
    ASSERT_TRUE(equals(context, obj1, obj2));
    ASSERT_TRUE(equals(context, obj1, obj1));


    // pair2
    tmp1 = string_make_from_cstr_op(context, "pair_car");
    tmp2 = vector_make_op(context, 10);
    tmp2 = pair_make_op(context, tmp2, IMM_UNIT);
    obj1 = pair_make_op(context, tmp1, tmp2);
    tmp1 = symbol_make_from_cstr_op(context, "pair_car");
    tmp2 = vector_make_op(context, 10);
    tmp2 = pair_make_op(context, tmp2, IMM_UNIT);
    obj2 = pair_make_op(context, tmp1, tmp2);
    type = context_get_object_type(context, obj1);
    equals_assert = type_info_equals(type);
    equals = object_equals_helper(context, obj1);
    ASSERT_FUNC_EQ(equals, equals_assert);
    ASSERT_FUNC_EQ(equals, pair_equals);
    ASSERT_FALSE(equals(context, obj1, obj2));
    ASSERT_TRUE(equals(context, obj1, obj1));


    // pair3
    tmp1 = string_make_from_cstr_op(context, "pair_car");
    tmp2 = char_imm_make('x');
    obj1 = pair_make_op(context, tmp1, tmp2);
    tmp1 = string_make_from_cstr_op(context, "pair_car");
    obj2 = pair_make_op(context, tmp1, IMM_UNIT);
    type = context_get_object_type(context, obj1);
    equals_assert = type_info_equals(type);
    equals = object_equals_helper(context, obj1);
    ASSERT_FUNC_EQ(equals, equals_assert);
    ASSERT_FUNC_EQ(equals, pair_equals);
    ASSERT_FALSE(equals(context, obj1, obj2));
    ASSERT_TRUE(equals(context, obj1, obj1));

    // bytes
    obj1 = bytes_make_op(context, 20);
    obj2 = bytes_make_op(context, 20);
    type = context_get_object_type(context, obj1);
    equals_assert = type_info_equals(type);
    equals = object_equals_helper(context, obj1);
    ASSERT_FUNC_EQ(equals, equals_assert);
    ASSERT_FUNC_EQ(equals, bytes_equals);
    ASSERT_FALSE(equals(context, obj1, obj2));
    ASSERT_TRUE(equals(context, obj1, obj1));


    // string
    char string_obj[] = "string_object test";
    obj1 = string_make_from_cstr_op(context, string_obj);
    obj2 = string_make_from_cstr_op(context, string_obj);
    type = context_get_object_type(context, obj1);
    equals_assert = type_info_equals(type);
    equals = object_equals_helper(context, obj1);
    ASSERT_FUNC_EQ(equals, equals_assert);
    ASSERT_FUNC_EQ(equals, string_equals);
    ASSERT_TRUE(equals(context, obj1, obj2));
    ASSERT_TRUE(equals(context, obj1, obj1));


    // string buffer
    char string_buffer_start_obj[] = "string_buffer: ";
    obj1 = string_make_from_cstr_op(context, string_buffer_start_obj);
    obj1 = string_buffer_make_from_string_op(context, obj1);
    obj2 = string_make_from_cstr_op(context, string_buffer_start_obj);
    obj2 = string_buffer_make_from_string_op(context, obj2);
    type = context_get_object_type(context, obj1);
    equals_assert = type_info_equals(type);
    equals = object_equals_helper(context, obj1);
    ASSERT_FUNC_EQ(equals, equals_assert);
    ASSERT_FUNC_EQ(equals, string_buffer_equals);
    ASSERT_FALSE(equals(context, obj1, obj2));
    ASSERT_TRUE(equals(context, obj1, obj1));

    char string_buffer_append_obj[] = "this is a string buffer";
    tmp1 = string_make_from_cstr_op(context, string_buffer_append_obj);
    obj1 = string_buffer_append_string_op(context, obj1, tmp1);
    tmp2 = string_make_from_cstr_op(context, string_buffer_append_obj);
    obj2 = string_buffer_append_string_op(context, obj2, tmp2);
    type = context_get_object_type(context, obj1);
    equals_assert = type_info_equals(type);
    equals = object_equals_helper(context, obj1);
    ASSERT_FUNC_EQ(equals, equals_assert);
    ASSERT_FUNC_EQ(equals, string_buffer_equals);
    ASSERT_FALSE(equals(context, obj1, obj2));
    ASSERT_TRUE(equals(context, obj1, obj1));

    tmp1 = string_buffer_to_string(context, obj1);
    tmp2 = string_buffer_to_string(context, obj2);
    type = context_get_object_type(context, tmp1);
    ASSERT_TRUE(is_string(tmp1) && is_string(tmp2));
    equals_assert = type_info_equals(type);
    equals = object_equals_helper(context, tmp1);
    ASSERT_FUNC_EQ(equals, equals_assert);
    ASSERT_FUNC_EQ(equals, string_equals);
    ASSERT_TRUE(equals(context, tmp1, tmp2));
    ASSERT_TRUE(equals(context, tmp1, tmp1));

    tmp1 = string_buffer_to_symbol(context, obj1);
    tmp2 = string_buffer_to_symbol(context, obj2);
    type = context_get_object_type(context, tmp1);
    equals_assert = type_info_equals(type);
    equals = object_equals_helper(context, tmp1);
    ASSERT_FUNC_EQ(equals, equals_assert);
    ASSERT_FUNC_EQ(equals, symbol_equals);
    ASSERT_TRUE(equals(context, tmp1, tmp2));
    ASSERT_TRUE(equals(context, tmp1, tmp1));


    // string append test
    char string_start_obj[] = "string_append_test";
    obj1 = string_make_from_cstr_op(context, string_start_obj);
    tmp1 = string_make_from_cstr_op(context, "");
    obj2 = string_append_op(context, obj1, tmp1);
    type = context_get_object_type(context, obj1);
    equals_assert = type_info_equals(type);
    equals = object_equals_helper(context, obj1);
    ASSERT_FUNC_EQ(equals, equals_assert);
    ASSERT_FUNC_EQ(equals, string_equals);
    ASSERT_TRUE(equals(context, obj1, obj2));
    ASSERT_TRUE(equals(context, obj1, obj1));
    ASSERT_TRUE(equals(context, obj2, obj2));

    tmp1 = string_make_from_cstr_op(context, ": new string");
    obj1 = string_append_op(context, obj1, tmp1);
    type = context_get_object_type(context, obj1);
    equals_assert = type_info_equals(type);
    equals = object_equals_helper(context, obj1);
    ASSERT_FUNC_EQ(equals, equals_assert);
    ASSERT_FUNC_EQ(equals, string_equals);
    ASSERT_FALSE(equals(context, obj1, obj2));
    ASSERT_TRUE(equals(context, obj1, obj1));
    ASSERT_TRUE(equals(context, obj2, obj2));

    // symbol
    char symbol_obj[] = "symbol_object test";
    obj1 = symbol_make_from_cstr_op(context, symbol_obj);
    obj2 = symbol_make_from_cstr_op(context, symbol_obj);
    type = context_get_object_type(context, obj1);
    equals_assert = type_info_equals(type);
    equals = object_equals_helper(context, obj1);
    ASSERT_FUNC_EQ(equals, equals_assert);
    ASSERT_FUNC_EQ(equals, symbol_equals);
    ASSERT_TRUE(equals(context, obj1, obj2));
    ASSERT_TRUE(equals(context, obj1, obj1));
    ASSERT_TRUE(equals(context, obj2, obj2));


    // vector
    obj1 = vector_make_op(context, 20);
    obj2 = vector_capacity_increase(context, obj1, 20);
    type = context_get_object_type(context, obj1);
    equals_assert = type_info_equals(type);
    equals = object_equals_helper(context, obj1);
    ASSERT_FUNC_EQ(equals, equals_assert);
    ASSERT_FUNC_EQ(equals, vector_equals);
    vector_ref(obj1, 0) = IMM_TRUE;
    ASSERT_TRUE(equals(context, obj1, obj1));
    ASSERT_FALSE(equals(context, obj1, obj2));


    // stack
    obj1 = stack_make_op(context, 10);
    obj2 = stack_capacity_increase(context, obj1, 10);
    type = context_get_object_type(context, obj1);
    equals_assert = type_info_equals(type);
    equals = object_equals_helper(context, obj1);
    ASSERT_FUNC_EQ(equals, equals_assert);
    ASSERT_FUNC_EQ(equals, stack_equals);
    ASSERT_FALSE(equals(context, obj1, obj2));
    ASSERT_TRUE(equals(context, obj1, obj1));
    ASSERT_TRUE(equals(context, obj2, obj2));


    // string_port_input_from_string
    obj1 = string_make_from_cstr_op(context, "this is a string_port input only");
    obj1 = string_port_input_from_string(context, obj1);
    type = context_get_object_type(context, obj1);
    equals_assert = type_info_equals(type);
    equals = object_equals_helper(context, obj1);
    ASSERT_FUNC_EQ(equals, equals_assert);
    ASSERT_FUNC_EQ(equals, string_port_equals);
    ASSERT_TRUE(equals(context, obj1, obj1));


    // string_port_output_use_buffer
    obj1 = string_port_output_use_buffer(context);
    type = context_get_object_type(context, obj1);
    equals_assert = type_info_equals(type);
    equals = object_equals_helper(context, obj1);
    ASSERT_FUNC_EQ(equals, equals_assert);
    ASSERT_FUNC_EQ(equals, string_port_equals);
    ASSERT_TRUE(equals(context, obj1, obj1));

    // string_port_in_out_put_from_string_use_buffer
    obj1 = string_make_from_cstr_op(context, "this is a string_port input output with internal string_buffer");
    gc_collect(context);
    obj1 = string_port_in_out_put_from_string_use_buffer(context, obj1);
    type = context_get_object_type(context, obj1);
    equals_assert = type_info_equals(type);
    equals = object_equals_helper(context, obj1);
    ASSERT_FUNC_EQ(equals, equals_assert);
    ASSERT_FUNC_EQ(equals, string_port_equals);
    ASSERT_TRUE(equals(context, obj1, obj1));

    // stdio_port_from_file
    obj1 = stdio_port_from_file(context, stdout, PORT_OUTPUT);
    type = context_get_object_type(context, obj1);
    equals_assert = type_info_equals(type);
    equals = object_equals_helper(context, obj1);
    ASSERT_FUNC_EQ(equals, equals_assert);
    ASSERT_FUNC_EQ(equals, stdio_port_equals);
    ASSERT_TRUE(equals(context, obj1, obj1));

    // stdio_port_from_filename
    FILE *file = fopen("stdio_port_equals_test_file.txt", "w");
    char test_str[] = "stdio_port_equals_test_file from filename test";
    size_t data_len = sizeof(test_str) - 1;
    fwrite(test_str, 1, data_len, file);
    fclose(file);
    obj1 = string_make_from_cstr_op(context, "stdio_port_equals_test_file.txt");
    obj1 = stdio_port_from_filename(context, obj1, PORT_INPUT);
    type = context_get_object_type(context, obj1);
    equals_assert = type_info_equals(type);
    equals = object_equals_helper(context, obj1);
    ASSERT_FUNC_EQ(equals, equals_assert);
    ASSERT_FUNC_EQ(equals, stdio_port_equals);
    ASSERT_TRUE(equals(context, obj1, obj1));


    // weak_ref, Ë³±ã²âÊÔÈõÒýÓÃ
    tmp1 = string_make_from_cstr_op(context, "weak_ref string 1");
    tmp2 = string_make_from_cstr_op(context, "weak_ref string 2");
    gc_collect(context);
    obj1 = weak_ref_make_op(context, tmp1);
    obj2 = weak_ref_make_op(context, tmp2);
    gc_collect(context);

    type = context_get_object_type(context, obj1);
    equals_assert = type_info_equals(type);
    equals = object_equals_helper(context, obj1);
    ASSERT_FUNC_EQ(equals, equals_assert);
    ASSERT_FUNC_EQ(equals, weak_ref_equals);
    ASSERT_FALSE(equals(context, obj1, obj2));
    ASSERT_TRUE(equals(context, obj1, obj1));
    ASSERT_TRUE(equals(context, obj2, obj2));

    ASSERT_TRUE(weak_ref_is_valid(obj1));
    ASSERT_TRUE(weak_ref_is_valid(obj2));
    ASSERT_EQ(weak_ref_get(obj1), tmp1);
    ASSERT_EQ(weak_ref_get(obj2), tmp2);

    tmp1 = IMM_UNIT;
    gc_collect(context);
    ASSERT_FALSE(weak_ref_is_valid(obj1));
    ASSERT_TRUE(weak_ref_is_valid(obj2));
    ASSERT_EQ(weak_ref_get(obj1), NULL);
    ASSERT_EQ(weak_ref_get(obj2), tmp2);
    ASSERT_FALSE(equals(context, obj1, obj2));
    ASSERT_TRUE(equals(context, obj1, obj1));
    ASSERT_TRUE(equals(context, obj2, obj2));

    tmp2 = IMM_UNIT;
    gc_collect(context);
    ASSERT_FALSE(weak_ref_is_valid(obj1));
    ASSERT_FALSE(weak_ref_is_valid(obj2));
    ASSERT_EQ(weak_ref_get(obj1), NULL);
    ASSERT_EQ(weak_ref_get(obj2), NULL);
    ASSERT_FALSE(equals(context, obj1, obj2));
    ASSERT_TRUE(equals(context, obj1, obj1));
    ASSERT_TRUE(equals(context, obj2, obj2));


    obj1 = IMM_UNIT;
    obj2 = IMM_UNIT;
    tmp1 = IMM_UNIT;
    tmp2 = IMM_UNIT;
    gc_release_var(context);
    gc_collect(context);
    ASSERT_TRUE(context->heap->first_node->data == context->heap->first_node->free_ptr);
    ASSERT_EQ(context->saves, NULL);
    context_destroy(context);
}