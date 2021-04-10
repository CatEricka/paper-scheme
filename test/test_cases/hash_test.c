#include "lib/utest.h"
#include <paper-scheme/runtime.h>

UTEST(hash_test, string_hash_test) {
    context_t context = context_make(16, 2, 0x10000);
    gc_var1(context, str);

    char *cstr = "abcdefghijkl";
    str = string_make_from_cstr_op(context, cstr);
    object_type_info_ptr t = context_get_object_type(context, str);
    hash_code_fn hash = type_info_hash_code(t);
    ASSERT_NE((void *) hash, NULL);
    UTEST_PRINTF("%s hash: %u\n", cstr, hash(context, str));

    cstr = "123456789010";
    str = string_make_from_cstr_op(context, cstr);
    UTEST_PRINTF("%s hash: %u\n", cstr, hash(context, str));

    ASSERT_EQ(context->saves, &__gc_var_dream1__);
    gc_release_var(context);
    ASSERT_EQ(context->saves, NULL);
    context_destroy(context);
}

UTEST(hash_test, symbol_hash_test) {
    context_t context = context_make(16, 2, 0x10000);
    gc_var1(context, str);

    char *cstr = "abcdefghijkl";
    str = symbol_make_from_cstr_untracked_op(context, cstr);
    object_type_info_ptr t = context_get_object_type(context, str);
    hash_code_fn hash = type_info_hash_code(t);
    ASSERT_NE((void *) hash, NULL);
    UTEST_PRINTF("%s hash: %u\n", cstr, hash(context, str));

    cstr = "123456789010";
    str = symbol_make_from_cstr_untracked_op(context, cstr);
    UTEST_PRINTF("%s hash: %u\n", cstr, hash(context, str));

    ASSERT_EQ(context->saves, &__gc_var_dream1__);
    gc_release_var(context);
    ASSERT_EQ(context->saves, NULL);
    context_destroy(context);
}


UTEST(hash_test, all_value_hash_test) {
    context_t context = context_make(16, 2, 0x10000);;
    gc_var4(context, obj, obj1, tmp1, tmp2);
    hash_code_fn hash_fn, hash_fn_assert;
    object_type_info_ptr type;

#define ASSERT_FUNC_EQ(x, y) ASSERT_EQ((void*)(x), (void*)(y))

    // i64
    obj = i64_imm_make(123);
    type = context_get_object_type(context, obj);
    hash_fn_assert = type_info_hash_code(type);
    hash_fn = object_hash_helper(context, obj);
    ASSERT_FUNC_EQ(hash_fn, hash_fn_assert);
    ASSERT_FUNC_EQ(hash_fn, i64_hash_code);
    UTEST_PRINTF("i64 123\t\t\thash: %u\n", i64_hash_code(context, obj));


    //i64
    obj = i64_make_op(context, 615171298343471923);
    type = context_get_object_type(context, obj);
    hash_fn_assert = type_info_hash_code(type);
    hash_fn = object_hash_helper(context, obj);
    ASSERT_FUNC_EQ(hash_fn, hash_fn_assert);
    ASSERT_FUNC_EQ(hash_fn, i64_hash_code);
    UTEST_PRINTF("i64 615171298343471923\thash: %u\n", i64_hash_code(context, obj));


    // char
    obj = char_imm_make('a');
    type = context_get_object_type(context, obj);
    hash_fn_assert = type_info_hash_code(type);
    hash_fn = object_hash_helper(context, obj);
    ASSERT_FUNC_EQ(hash_fn, hash_fn_assert);
    ASSERT_FUNC_EQ(hash_fn, char_hash_code);
    UTEST_PRINTF("char %c\t\t\thash: %u\n", char_imm_getvalue(obj), char_hash_code(context, obj));


    // double
    obj = doublenum_make_op(context, 0.3);
    type = context_get_object_type(context, obj);
    hash_fn_assert = type_info_hash_code(type);
    hash_fn = object_hash_helper(context, obj);
    ASSERT_FUNC_EQ(hash_fn, hash_fn_assert);
    ASSERT_FUNC_EQ(hash_fn, d64_hash_code);
    UTEST_PRINTF("double %f\t\thash: %u\n", doublenum_getvalue(obj), d64_hash_code(context, obj));


    // double
    obj = doublenum_make_op(context, 0.4);
    type = context_get_object_type(context, obj);
    hash_fn_assert = type_info_hash_code(type);
    hash_fn = object_hash_helper(context, obj);
    ASSERT_FUNC_EQ(hash_fn, hash_fn_assert);
    ASSERT_FUNC_EQ(hash_fn, d64_hash_code);
    UTEST_PRINTF("double %f\t\thash: %u\n", doublenum_getvalue(obj), d64_hash_code(context, obj));


    // pair1
    tmp1 = string_make_from_cstr_op(context, "pair_car");
    tmp2 = symbol_make_from_cstr_untracked_op(context, "pair_cdr");
    obj = pair_make_op(context, tmp1, tmp2);
    type = context_get_object_type(context, obj);
    hash_fn_assert = type_info_hash_code(type);
    hash_fn = object_hash_helper(context, obj);
    ASSERT_FUNC_EQ(hash_fn, hash_fn_assert);
    ASSERT_FUNC_EQ(hash_fn, pair_hash_code);
    UTEST_PRINTF("pair1\t\t\thash: %u\n", pair_hash_code(context, obj));


    // pair2
    tmp1 = string_make_from_cstr_op(context, "pair_car");
    tmp2 = vector_make_op(context, 10);
    tmp2 = pair_make_op(context, tmp2, IMM_UNIT);
    obj = pair_make_op(context, tmp1, tmp2);
    type = context_get_object_type(context, obj);
    hash_fn_assert = type_info_hash_code(type);
    hash_fn = object_hash_helper(context, obj);
    ASSERT_FUNC_EQ(hash_fn, hash_fn_assert);
    ASSERT_FUNC_EQ(hash_fn, pair_hash_code);
    UTEST_PRINTF("pair2\t\t\thash: %u\n", pair_hash_code(context, obj));


    // pair3, ASSERT(car(pair3) == cdr (pair3))
    tmp1 = string_make_from_cstr_op(context, "pair_car");
    tmp2 = string_make_from_cstr_op(context, "pair_car");
    obj = pair_make_op(context, tmp1, tmp2);
    type = context_get_object_type(context, obj);
    hash_fn_assert = type_info_hash_code(type);
    hash_fn = object_hash_helper(context, obj);
    ASSERT_FUNC_EQ(hash_fn, hash_fn_assert);
    ASSERT_FUNC_EQ(hash_fn, pair_hash_code);
    UTEST_PRINTF("pair3\t\t\thash: %u\n", pair_hash_code(context, obj));


    // bytes
    obj = bytes_make_op(context, 20);
    type = context_get_object_type(context, obj);
    hash_fn_assert = type_info_hash_code(type);
    hash_fn = object_hash_helper(context, obj);
    ASSERT_FUNC_EQ(hash_fn, hash_fn_assert);
    ASSERT_FUNC_EQ(hash_fn, bytes_hash_code);
    UTEST_PRINTF("bytes1\t\t\thash: %u\n", bytes_hash_code(context, obj));


    // string
    char string_obj[] = "string_object test";
    obj = string_make_from_cstr_op(context, string_obj);
    type = context_get_object_type(context, obj);
    hash_fn_assert = type_info_hash_code(type);
    hash_fn = object_hash_helper(context, obj);
    ASSERT_FUNC_EQ(hash_fn, hash_fn_assert);
    ASSERT_FUNC_EQ(hash_fn, string_hash_code);
    UTEST_PRINTF("\"%s\"\thash: %u\n", string_obj, string_hash_code(context, obj));


    // string buffer
    char string_buffer_start_obj[] = "string_buffer: ";
    obj = string_make_from_cstr_op(context, string_buffer_start_obj);
    obj = string_buffer_make_from_string_op(context, obj);
    type = context_get_object_type(context, obj);
    hash_fn_assert = type_info_hash_code(type);
    hash_fn = object_hash_helper(context, obj);
    ASSERT_FUNC_EQ(hash_fn, hash_fn_assert);
    ASSERT_FUNC_EQ(hash_fn, string_buffer_hash_code);
    uint32_t hash1 = hash_fn(context, obj);
    tmp1 = string_buffer_to_string_op(context, obj);
    UTEST_PRINTF("\"%s\"\thash: %u\n", string_get_cstr(tmp1), string_buffer_hash_code(context, obj));

    char string_buffer_append_obj[] = "this is a string buffer";
    tmp1 = string_make_from_cstr_op(context, string_buffer_append_obj);
    obj = string_buffer_append_string_op(context, obj, tmp1);
    type = context_get_object_type(context, obj);
    hash_fn_assert = type_info_hash_code(type);
    hash_fn = object_hash_helper(context, obj);
    ASSERT_FUNC_EQ(hash_fn, hash_fn_assert);
    ASSERT_FUNC_EQ(hash_fn, string_buffer_hash_code);
    uint32_t hash2 = hash_fn(context, obj);
    tmp1 = string_buffer_to_string_op(context, obj);
    UTEST_PRINTF("\"%s\"\n\t\t\thash: %u\n", string_get_cstr(tmp1), string_buffer_hash_code(context, obj));
    ASSERT_EQ(hash1, hash2);

    tmp1 = string_buffer_to_string_op(context, obj);
    type = context_get_object_type(context, tmp1);
    hash_fn_assert = type_info_hash_code(type);
    hash_fn = object_hash_helper(context, tmp1);
    ASSERT_FUNC_EQ(hash_fn, hash_fn_assert);
    ASSERT_FUNC_EQ(hash_fn, string_hash_code);
    UTEST_PRINTF("string_buffer->string: \"%s\"\n\t\t\thash: %u\n", string_get_cstr(tmp1),
                 string_hash_code(context, tmp1));

    tmp1 = string_buffer_to_symbol_untracked_op(context, obj);
    type = context_get_object_type(context, tmp1);
    hash_fn_assert = type_info_hash_code(type);
    hash_fn = object_hash_helper(context, tmp1);
    ASSERT_FUNC_EQ(hash_fn, hash_fn_assert);
    ASSERT_FUNC_EQ(hash_fn, symbol_hash_code);
    UTEST_PRINTF("string_buffer->symbol: \"%s\"\n\t\t\thash: %u\n", symbol_get_cstr(tmp1),
                 symbol_hash_code(context, tmp1));


    // string append test
    char string_start_obj[] = "string_append_test";
    obj = string_make_from_cstr_op(context, string_start_obj);
    type = context_get_object_type(context, obj);
    hash_fn_assert = type_info_hash_code(type);
    hash_fn = object_hash_helper(context, obj);
    ASSERT_FUNC_EQ(hash_fn, hash_fn_assert);
    ASSERT_FUNC_EQ(hash_fn, string_hash_code);
    hash1 = hash_fn(context, obj);
    UTEST_PRINTF("\"%s\"\thash: %u\n", string_get_cstr(obj), string_hash_code(context, obj));

    tmp1 = string_make_from_cstr_op(context, "");
    obj = string_append_op(context, obj, tmp1);
    type = context_get_object_type(context, obj);
    hash_fn_assert = type_info_hash_code(type);
    hash_fn = object_hash_helper(context, obj);
    ASSERT_FUNC_EQ(hash_fn, hash_fn_assert);
    ASSERT_FUNC_EQ(hash_fn, string_hash_code);
    hash2 = hash_fn(context, obj);
    UTEST_PRINTF("\"%s\"\thash: %u\n", string_get_cstr(obj), string_hash_code(context, obj));
    ASSERT_EQ(hash1, hash2);

    tmp1 = string_make_from_cstr_op(context, ": new string");
    obj = string_append_op(context, obj, tmp1);
    type = context_get_object_type(context, obj);
    hash_fn_assert = type_info_hash_code(type);
    hash_fn = object_hash_helper(context, obj);
    ASSERT_FUNC_EQ(hash_fn, hash_fn_assert);
    ASSERT_FUNC_EQ(hash_fn, string_hash_code);
    hash2 = hash_fn(context, obj);
    UTEST_PRINTF("\"%s\"\n\t\t\thash: %u\n", string_get_cstr(obj), string_hash_code(context, obj));
    EXPECT_NE(hash1, hash2);
    if (hash1 == hash2) {
        UTEST_PRINTF("hash clash: %d, not test failed", hash1);
    }


    // symbol
    char symbol_obj[] = "symbol_object test";
    obj = symbol_make_from_cstr_untracked_op(context, symbol_obj);
    type = context_get_object_type(context, obj);
    hash_fn_assert = type_info_hash_code(type);
    hash_fn = object_hash_helper(context, obj);
    ASSERT_FUNC_EQ(hash_fn, hash_fn_assert);
    ASSERT_FUNC_EQ(hash_fn, symbol_hash_code);
    UTEST_PRINTF("\"%s\"\thash: %u\n", symbol_obj, symbol_hash_code(context, obj));


    // vector
    obj = vector_make_op(context, 20);
    type = context_get_object_type(context, obj);
    hash_fn_assert = type_info_hash_code(type);
    hash_fn = object_hash_helper(context, obj);
    ASSERT_FUNC_EQ(hash_fn, hash_fn_assert);
    ASSERT_FUNC_EQ(hash_fn, vector_hash_code);
    hash1 = hash_fn(context, obj);
    UTEST_PRINTF("vector\t\t\thash: %u\n", vector_hash_code(context, obj));

    vector_ref(obj, 0) = IMM_TRUE;
    type = context_get_object_type(context, obj);
    hash_fn_assert = type_info_hash_code(type);
    hash_fn = object_hash_helper(context, obj);
    ASSERT_FUNC_EQ(hash_fn, hash_fn_assert);
    ASSERT_FUNC_EQ(hash_fn, vector_hash_code);
    hash2 = hash_fn(context, obj);
    UTEST_PRINTF("vector\t\t\thash: %u\n", vector_hash_code(context, obj));
    ASSERT_EQ(hash1, hash2);

    tmp1 = vector_capacity_increase_op(context, obj, 20);
    type = context_get_object_type(context, tmp1);
    hash_fn_assert = type_info_hash_code(type);
    hash_fn = object_hash_helper(context, tmp1);
    ASSERT_FUNC_EQ(hash_fn, hash_fn_assert);
    ASSERT_FUNC_EQ(hash_fn, vector_hash_code);
    hash2 = hash_fn(context, tmp1);
    UTEST_PRINTF("vector\t\t\thash: %u\n", vector_hash_code(context, tmp1));
    EXPECT_NE(hash1, hash2);
    if (hash1 == hash2) {
        UTEST_PRINTF("hash clash: %d, not test failed", hash1);
    }


    // stack
    obj = stack_make_op(context, 10);
    type = context_get_object_type(context, obj);
    hash_fn_assert = type_info_hash_code(type);
    hash_fn = object_hash_helper(context, obj);
    ASSERT_FUNC_EQ(hash_fn, hash_fn_assert);
    ASSERT_FUNC_EQ(hash_fn, stack_hash_code);
    hash1 = hash_fn(context, obj);
    UTEST_PRINTF("stack\t\t\thash: %u\n", stack_hash_code(context, obj));

    tmp1 = stack_capacity_increase_op(context, obj, 10);
    type = context_get_object_type(context, tmp1);
    hash_fn_assert = type_info_hash_code(type);
    hash_fn = object_hash_helper(context, tmp1);
    ASSERT_FUNC_EQ(hash_fn, hash_fn_assert);
    ASSERT_FUNC_EQ(hash_fn, stack_hash_code);
    hash2 = hash_fn(context, tmp1);
    UTEST_PRINTF("stack\t\t\thash: %u\n", stack_hash_code(context, tmp1));
    EXPECT_NE(hash1, hash2);
    if (hash1 == hash2) {
        UTEST_PRINTF("hash clash: %d, not test failed", hash1);
    }


    // string_port_input_from_string_op
    obj = string_make_from_cstr_op(context, "this is a string_port input only");
    obj = string_port_input_from_string_op(context, obj);
    type = context_get_object_type(context, obj);
    hash_fn_assert = type_info_hash_code(type);
    hash_fn = object_hash_helper(context, obj);
    ASSERT_FUNC_EQ(hash_fn, hash_fn_assert);
    ASSERT_FUNC_EQ(hash_fn, string_port_hash_code);
    UTEST_PRINTF("string_port_input\thash: %u\n", string_port_hash_code(context, obj));


    // string_port_output_use_buffer_op
    obj = string_port_output_use_buffer_op(context);
    type = context_get_object_type(context, obj);
    hash_fn_assert = type_info_hash_code(type);
    hash_fn = object_hash_helper(context, obj);
    ASSERT_FUNC_EQ(hash_fn, hash_fn_assert);
    ASSERT_FUNC_EQ(hash_fn, string_port_hash_code);
    UTEST_PRINTF("string_port_output\thash: %u\n", string_port_hash_code(context, obj));

    // string_port_in_out_put_from_string_use_buffer_op
    obj = string_make_from_cstr_op(context, "this is a string_port input output with internal string_buffer");
    gc_collect(context);
    obj = string_port_in_out_put_from_string_use_buffer_op(context, obj);
    type = context_get_object_type(context, obj);
    hash_fn_assert = type_info_hash_code(type);
    hash_fn = object_hash_helper(context, obj);
    ASSERT_FUNC_EQ(hash_fn, hash_fn_assert);
    ASSERT_FUNC_EQ(hash_fn, string_port_hash_code);
    UTEST_PRINTF("string_port_output from string\n\t\t\thash: %u\n", string_port_hash_code(context, obj));

    // stdio_port_from_file_op
    obj = stdio_port_from_file_op(context, stdout, PORT_OUTPUT);
    type = context_get_object_type(context, obj);
    hash_fn_assert = type_info_hash_code(type);
    hash_fn = object_hash_helper(context, obj);
    ASSERT_FUNC_EQ(hash_fn, hash_fn_assert);
    ASSERT_FUNC_EQ(hash_fn, stdio_port_hash_code);
    UTEST_PRINTF("stdio_port output\thash: %u\n", stdio_port_hash_code(context, obj));

    // stdio_port_from_filename_op
    FILE *file = fopen("stdio_port_hash_test_file.txt", "w");
    char test_str[] = "stdio_port_hash_test_file from filename test";
    size_t data_len = sizeof(test_str) - 1;
    fwrite(test_str, 1, data_len, file);
    fclose(file);
    obj = string_make_from_cstr_op(context, "stdio_port_hash_test_file.txt");
    obj = stdio_port_from_filename_op(context, obj, PORT_INPUT);
    type = context_get_object_type(context, obj);
    hash_fn_assert = type_info_hash_code(type);
    hash_fn = object_hash_helper(context, obj);
    ASSERT_FUNC_EQ(hash_fn, hash_fn_assert);
    ASSERT_FUNC_EQ(hash_fn, stdio_port_hash_code);
    UTEST_PRINTF("stdio_port by filename\thash: %u\n", stdio_port_hash_code(context, obj));
    obj = IMM_UNIT;
    gc_collect(context);


    // weak_ref, Ë³±ã²âÊÔÈõÒýÓÃ
    tmp1 = string_make_from_cstr_op(context, "weak_ref string");
    gc_collect(context);
    obj = weak_ref_make_op(context, tmp1);
    gc_collect(context);
    ASSERT_TRUE(weak_ref_is_valid(obj));
    ASSERT_EQ(weak_ref_get(obj), tmp1);
    type = context_get_object_type(context, obj);
    hash_fn_assert = type_info_hash_code(type);
    hash_fn = object_hash_helper(context, obj);
    ASSERT_FUNC_EQ(hash_fn, hash_fn_assert);
    ASSERT_FUNC_EQ(hash_fn, weak_ref_hash_code);
    hash1 = weak_ref_hash_code(context, obj);
    UTEST_PRINTF("weak-ref\t\thash: %u\n", weak_ref_hash_code(context, obj));

    tmp1 = IMM_UNIT;
    gc_collect(context);
    ASSERT_FALSE(weak_ref_is_valid(obj));
    ASSERT_EQ(NULL, weak_ref_get(obj));
    type = context_get_object_type(context, obj);
    hash_fn_assert = type_info_hash_code(type);
    hash_fn = object_hash_helper(context, obj);
    ASSERT_FUNC_EQ(hash_fn, hash_fn_assert);
    ASSERT_FUNC_EQ(hash_fn, weak_ref_hash_code);
    hash2 = weak_ref_hash_code(context, obj);
    UTEST_PRINTF("weak-ref\t\thash: %u\n", weak_ref_hash_code(context, obj));

    ASSERT_EQ(hash1, hash2);

    // hashset
    obj = hashset_make_op(context, 2, 0.75);
    type = context_get_object_type(context, obj);
    hash_fn_assert = type_info_hash_code(type);
    hash_fn = object_hash_helper(context, obj);
    ASSERT_FUNC_EQ(hash_fn, hash_fn_assert);
    ASSERT_FUNC_EQ(hash_fn, hash_set_hash_code);
    UTEST_PRINTF("hashset\t\t\thash: %u\n", hash_set_hash_code(context, obj));

    // hashmap
    obj = hashmap_make_op(context, 2, 0.75);
    type = context_get_object_type(context, obj);
    hash_fn_assert = type_info_hash_code(type);
    hash_fn = object_hash_helper(context, obj);
    ASSERT_FUNC_EQ(hash_fn, hash_fn_assert);
    ASSERT_FUNC_EQ(hash_fn, hash_map_hash_code);
    UTEST_PRINTF("hashmap\t\t\thash: %u\n", hash_map_hash_code(context, obj));

    // weak_hashset
    obj = weak_hashset_make_op(context, 2, 0.75);
    type = context_get_object_type(context, obj);
    hash_fn_assert = type_info_hash_code(type);
    hash_fn = object_hash_helper(context, obj);
    ASSERT_FUNC_EQ(hash_fn, hash_fn_assert);
    ASSERT_FUNC_EQ(hash_fn, weak_hashset_hash_code);
    UTEST_PRINTF("weak_hashset\t\thash: %u\n", weak_hashset_hash_code(context, obj));


    obj1 = IMM_UNIT;
    tmp1 = IMM_UNIT;
    tmp2 = IMM_UNIT;
    obj = IMM_UNIT;
    ASSERT_EQ(context->saves, &__gc_var_dream4__);
    gc_release_var(context);
    gc_collect(context);
    ASSERT_TRUE(context->heap->first_node->data == context->heap->first_node->free_ptr);
    ASSERT_EQ(context->saves, NULL);
    context_destroy(context);
}