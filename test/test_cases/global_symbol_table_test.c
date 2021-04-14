#include "lib/utest.h"
#include <paper-scheme/interpreter.h>

UTEST(global_symbol_table, test) {
    context_t context = interpreter_create(0x8000000, 2, 0x32000000);
    gc_var2(context, sym, obj);

    sym = symbol_make_from_cstr_op(context, "sym");
    ASSERT_EQ(IMM_TRUE, global_symbol_found(context, sym));

    obj = string_buffer_make_op(context, 16);
    string_buffer_append_cstr_op(context, obj, "string_buffer");
    obj = string_buffer_to_symbol_op(context, obj);
    ASSERT_EQ(0, memcmp(string_get_cstr(obj), "string_buffer", string_len(obj)));
    ASSERT_EQ(IMM_TRUE, global_symbol_found(context, sym));
    ASSERT_EQ(IMM_TRUE, global_symbol_found(context, obj));

    sym = IMM_UNIT;
    obj = IMM_UNIT;
    gc_collect(context);
    sym = symbol_make_from_cstr_untracked_op(context, "sym");
    obj = symbol_make_from_cstr_untracked_op(context, "string_buffer");
    ASSERT_EQ(IMM_FALSE, global_symbol_found(context, sym));
    ASSERT_EQ(IMM_FALSE, global_symbol_found(context, obj));
    // 不再有效, 因为初始环境中存在关键字和默认过程
    //    obj = global_symbol_all_symbol(context);
    //    ASSERT_EQ(0, vector_len(obj));

    obj = string_make_from_cstr_op(context, "string");
    obj = string_to_symbol_op(context, obj);
    ASSERT_EQ(0, memcmp(string_get_cstr(obj), "string", string_len(obj)));
    ASSERT_EQ(IMM_FALSE, global_symbol_found(context, sym));
    ASSERT_EQ(IMM_TRUE, global_symbol_found(context, obj));

    sym = IMM_UNIT;
    obj = IMM_UNIT;
    gc_collect(context);
    sym = symbol_make_from_cstr_untracked_op(context, "sym");
    obj = symbol_make_from_cstr_untracked_op(context, "string");
    ASSERT_EQ(IMM_FALSE, global_symbol_found(context, sym));
    ASSERT_EQ(IMM_FALSE, global_symbol_found(context, obj));
    //    obj = global_symbol_all_symbol(context);
    //    ASSERT_EQ(0, vector_len(obj));

    // 测试 gensym
    obj = symbol_make_from_cstr_op(context, "gensym_0x0000000000000000");
    sym = gensym(context);
    UTEST_PRINTF("gensym: %s\n", symbol_get_cstr(sym));
    sym = gensym(context);
    UTEST_PRINTF("gensym: %s\n", symbol_get_cstr(sym));
    sym = gensym(context);
    UTEST_PRINTF("gensym: %s\n", symbol_get_cstr(sym));

    ASSERT_EQ(context->saves, &__gc_var_dream2__);
    gc_release_var(context);
    interpreter_destroy(context);
}