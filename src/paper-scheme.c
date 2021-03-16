#include "paper-scheme/paper-scheme.h"

int main() {
#ifdef IS_64_BIT_ARCH
    printf("Hello CMake at 64bits.\n");
#elif IS_32_BIT_ARCH
    printf("Hello CMake at 32bits.\n");
#endif
    return 0;
}
