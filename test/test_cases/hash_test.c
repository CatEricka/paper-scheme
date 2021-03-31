#include "lib/utest.h"
#include <paper-scheme/runtime.h>

UTEST(hash_test, string_hash_test) {
    context_t context = context_make(16, 2, 0x10000);
    gc_var1(context, str);

    char *cstr = "abcdefghijkl";
    str = string_make_from_cstr_op(context, cstr);
    object_type_info_ptr t = context_get_object_type(context, str);
    hash_code_fn hash = type_info_hash_code(t);
    ASSERT_NE(hash, NULL);
    UTEST_PRINTF("%s hash: 0x%08"
                         PRIx32
                         "\n", cstr, hash(context, str));

    cstr = "123456789010";
    str = string_make_from_cstr_op(context, cstr);
    UTEST_PRINTF("%s hash: 0x%08"
                         PRIx32
                         "\n", cstr, hash(context, str));

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
    ASSERT_NE(hash, NULL);
    UTEST_PRINTF("%s hash: 0x%08"
                         PRIx32
                         "\n", cstr, hash(context, str));

    cstr = "123456789010";
    str = symbol_make_from_cstr_op(context, cstr);
    UTEST_PRINTF("%s hash: 0x%08"
                         PRIx32
                         "\n", cstr, hash(context, str));

    gc_release_var(context);
    ASSERT_EQ(context->saves, NULL);
    context_destroy(context);
}