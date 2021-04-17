#include "lib/utest.h"
#include <paper-scheme/interpreter.h>

UTEST(opcode_test, op_exec_repl_test) {
    size_t heap = 512 * 1024;
    context_t context = interpreter_create(heap, 2, heap);
    gc_var1(context, var);

    UTEST_PRINTF("opcode: total %d\n", MAX_OP_CODE);
    FILE *file = fopen(TEST_RESOURCE_DIR"/test_1.scm", "r");
    ASSERT_NE(file, NULL);
    int ret = interpreter_load_file_with_name(context, file, "test_1.scm");
    fclose(file);
    UTEST_PRINTF("return code: %d\n", ret);
    ASSERT_EQ(ret, 0);
    ASSERT_EQ(interpreter_load_cstr(context, "(test (list (reader)))"), 0);

    assert(context->saves == &__gc_var_dream1__);
    gc_release_var(context);
    interpreter_destroy(context);
}