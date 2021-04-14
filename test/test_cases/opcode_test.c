#include "lib/utest.h"
#include <paper-scheme/interpreter.h>

UTEST(opcode_test, op_exec_repl_test) {
    context_t context = interpreter_create(0x8000000, 2, 0x32000000);
    gc_var5(context, var, val, slot, env, obj);

//    interpreter_load_file(context, stdin);
    ASSERT_EQ(interpreter_load_cstr(context, "(test (list (reader)))"), 0);

    assert(context->saves == &__gc_var_dream5__);
    gc_release_var(context);
    interpreter_destroy(context);
}