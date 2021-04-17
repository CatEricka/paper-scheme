#include "lib/utest.h"
#include <paper-scheme/interpreter.h>

UTEST(syntax_test, keyword) {
    size_t heap = 64 * 1024;
    context_t context = interpreter_create(heap, 2, heap);
    gc_var1(context, sym);

    char *syntax_list[] = {
            "lambda",
            "quote",
            "define",
            "if",
            "begin",
            "set!",
            "let",
            "let*",
            "letrec",
            "cond",
            "delay",
            "and",
            "or",
            "cons-stream",
            "macro",
            "case",
    };

    for (size_t i = 0; i < sizeof(syntax_list) / sizeof(syntax_list[0]); i++) {
        sym = symbol_make_from_cstr_op(context, syntax_list[i]);
        ASSERT_EQ(symbol_is_syntax(context, sym), IMM_TRUE);
    }

    sym = symbol_make_from_cstr_op(context, "else");
    ASSERT_TRUE(find_slot_in_current_env(context, sym, 1) != IMM_UNIT);
    sym = symbol_make_from_cstr_op(context, "lambda");
    ASSERT_TRUE(is_imm_true(global_symbol_found(context, sym)));
    sym = symbol_make_from_cstr_op(context, "quote");
    ASSERT_TRUE(is_imm_true(global_symbol_found(context, sym)));
    sym = symbol_make_from_cstr_op(context, "quasiquote");
    ASSERT_TRUE(is_imm_true(global_symbol_found(context, sym)));
    sym = symbol_make_from_cstr_op(context, "unquote");
    ASSERT_TRUE(is_imm_true(global_symbol_found(context, sym)));
    sym = symbol_make_from_cstr_op(context, "unquote-splicing");
    ASSERT_TRUE(is_imm_true(global_symbol_found(context, sym)));
    sym = symbol_make_from_cstr_op(context, "=>");
    ASSERT_TRUE(is_imm_true(global_symbol_found(context, sym)));
    sym = symbol_make_from_cstr_op(context, "*error-hook*");
    ASSERT_TRUE(is_imm_true(global_symbol_found(context, sym)));
    sym = symbol_make_from_cstr_op(context, "*compile-hook*");
    ASSERT_TRUE(is_imm_true(global_symbol_found(context, sym)));

    assert(context->saves == &__gc_var_dream1__);
    gc_release_var(context);
    interpreter_destroy(context);
}