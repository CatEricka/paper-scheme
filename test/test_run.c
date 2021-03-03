#include "utest.h"

// include test file here
#include "object_test.h"
#include "gc_test.h"

UTEST_STATE();

int main(int argc, const char *const argv[]) {
#ifdef IS_USE_MSVC
    //wtf -> https://github.com/sheredom/utest.h/issues/44
    system(" ");
#endif
    int ret = utest_main(argc, argv);
#ifdef IS_USE_MSVC
    system("pause");
#else
    getchar();
#endif
    return ret;
}