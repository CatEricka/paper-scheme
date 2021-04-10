#include <paper-scheme/paper-scheme.h>

int main() {
    context_t context = interpreter_create(0x1000, 2, 0x3000);

    context_destroy(context);
    return 0;
}
