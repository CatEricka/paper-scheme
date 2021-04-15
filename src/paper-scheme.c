#include <paper-scheme/paper-scheme.h>

int main() {
    context_t context = interpreter_create(0x800000, 2, 0x80000000);
    printf("paper-scheme version 0.1 alpha");
    int ret = interpreter_load_file(context, stdin);

    assert(context->saves == NULL);
    interpreter_destroy(context);
    return ret;
}
