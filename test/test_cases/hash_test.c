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

    gc_release_var(context);
    ASSERT_EQ(context->saves, NULL);
    context_destroy(context);
}

UTEST(hash_test, symbol_hash_test) {
    context_t context = context_make(16, 2, 0x10000);
    gc_var1(context, str);

    char *cstr = "abcdefghijkl";
    str = symbol_make_from_cstr_op(context, cstr);
    object_type_info_ptr t = context_get_object_type(context, str);
    hash_code_fn hash = type_info_hash_code(t);
    ASSERT_NE((void *) hash, NULL);
    UTEST_PRINTF("%s hash: %u\n", cstr, hash(context, str));

    cstr = "123456789010";
    str = symbol_make_from_cstr_op(context, cstr);
    UTEST_PRINTF("%s hash: %u\n", cstr, hash(context, str));

    gc_release_var(context);
    ASSERT_EQ(context->saves, NULL);
    context_destroy(context);
}


UTEST(hash_test, value_hash_test) {
    context_t context = context_make(16, 2, 0x10000);;
    gc_var4(context, obj, obj1, tmp1, tmp2);
    hash_code_fn hash_fn, hash_fn_assert;
    object_type_info_ptr type;

    obj = i64_imm_make(123);
    type = context_get_object_type(context, obj);
    hash_fn_assert = type_info_hash_code(type);
    hash_fn = object_hash_helper(context, obj);
    ASSERT_EQ(hash_fn, hash_fn_assert);
    ASSERT_EQ(hash_fn, i64_hash_code);
    UTEST_PRINTF("i64 123\t\t\thash: %u\n", i64_hash_code(context, obj));

    obj = i64_make_op(context, 615171298343471923);
    type = context_get_object_type(context, obj);
    hash_fn_assert = type_info_hash_code(type);
    hash_fn = object_hash_helper(context, obj);
    ASSERT_EQ(hash_fn, hash_fn_assert);
    ASSERT_EQ(hash_fn, i64_hash_code);
    UTEST_PRINTF("i64 615171298343471923\thash: %u\n", i64_hash_code(context, obj));

    obj = char_imm_make('a');
    type = context_get_object_type(context, obj);
    hash_fn_assert = type_info_hash_code(type);
    hash_fn = object_hash_helper(context, obj);
    ASSERT_EQ(hash_fn, hash_fn_assert);
    ASSERT_EQ(hash_fn, char_hash_code);
    UTEST_PRINTF("char %c\t\t\thash: %u\n", char_imm_getvalue(obj), char_hash_code(context, obj));

    gc_release_var(context);
    ASSERT_EQ(context->saves, NULL);
    context_destroy(context);
}