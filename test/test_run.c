// test_cases test file here

#include "test_cases/utest.h"

UTEST_STATE();

#include "test_cases/object_test.h"
#include "test_cases/gc_test.h"
#include "test_cases/context_test.h"
#include "interpreter_test.h"
#include "test_cases/value_test.h"

int main(int argc, const char *const argv[]) {
#if 0
    // for clion terminal bug
    setvbuf(stdout, NULL, _IONBF, 0);
#endif
#ifdef IS_USE_MSVC
    //wtf -> https://github.com/sheredom/utest.h/issues/44
    system(" ");
#endif
    int ret = utest_main(argc, argv);
#ifdef IS_USE_MSVC
//    system("pause");
#else
    //    getchar();
#endif
    return ret;
}