#include <paper-scheme/paper-scheme.h>

int main() {
    context_t context = interpreter_create(0x8000000, 2, 0x32000000);
    printf("paper-scheme version 0.1 alpha");
    interpreter_load_file(context, stdin);

    assert(context->saves == NULL);
    interpreter_destroy(context);
    return 0;
}
