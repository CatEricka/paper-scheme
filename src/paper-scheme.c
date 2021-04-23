#include <paper-scheme/paper-scheme.h>
// #include <vld.h>

int main() {
    context_t context = interpreter_create(0x800000, 2, 0x80000000);
    printf("paper-scheme version 1.0\n");
    int ret = 0;

    // load ps_init.scm
    FILE *init = fopen(PAPER_SCHEME_INIT_DEFAULT_FILENAME, "r");
    if (init == NULL) {
        printf("Error: cannot open "PAPER_SCHEME_INIT_DEFAULT_FILENAME"\n");
    } else {
        ret = interpreter_load_file_with_name(context, init, PAPER_SCHEME_INIT_DEFAULT_FILENAME);
        fclose(init);
        if (ret != NO_ERROR) {
            printf("Error: "PAPER_SCHEME_INIT_DEFAULT_FILENAME" load failed\n");
        }
    }

    // load stdin
    setvbuf(stdout, NULL, _IONBF, 1024);
    ret = interpreter_load_file_with_name(context, stdin, "<stdin>");

    assert(context->saves == NULL);
    interpreter_destroy(context);
    return ret;
}
