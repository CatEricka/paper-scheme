#include "lib/utest.h"
#include <paper-scheme/interpreter.h>

UTEST(syntax_test, keyword) {
    context_t context = interpreter_create(0x8000000, 2, 0x32000000);
    gc_var1(context, sym);

    char *syntax_list[] = {
            "lambda",
            "quote",
            "define",
            "if",
            "else",
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


    assert(context->saves == &__gc_var_dream1__);
    gc_release_var(context);
    interpreter_destroy(context);
}