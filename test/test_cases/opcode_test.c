#include "lib/utest.h"
#include <paper-scheme/interpreter.h>

UTEST(opcode_test, op_exec_repl_test) {
    context_t context = interpreter_create(0x8000000, 2, 0x32000000);
    gc_var5(context, var, val, slot, env, obj);

    FILE *file = fopen(TEST_RESOURCE_DIR"/test_1.scm", "r");
    ASSERT_NE(file, NULL);
    int ret = interpreter_load_file_with_name(context, file, "test_1.scm");
    fclose(file);
    UTEST_PRINTF("return code: %d\n", ret);
    ASSERT_EQ(ret, 0);
    ASSERT_EQ(interpreter_load_cstr(context, "(test (list (reader)))"), 0);

    assert(context->saves == &__gc_var_dream5__);
    gc_release_var(context);
    interpreter_destroy(context);
}