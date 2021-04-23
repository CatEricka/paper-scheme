// test_cases test file here
#include "lib/utest.h"
// #include <vld.h>

UTEST_STATE();

int main(int argc, const char *const argv[]) {
#if 0
    // for clion terminal bug
    setvbuf(stdout, NULL, _IONBF, 0);
#endif
#ifdef IS_USE_MSVC
    // wtf -> https://github.com/sheredom/utest.h/issues/44
    system(" ");
#endif
    int ret = utest_main(argc, argv);
#ifdef IS_USE_MSVC
    // system("pause");
#else
    // getchar();
#endif
    return ret;
}