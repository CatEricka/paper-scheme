// base-scheme.cpp: 定义应用程序的入口点。
//

#include "base-scheme/base-scheme.h"

int main()
{
#ifdef IS_64_BIT_ARCH
    printf("Hello CMake at 64bits.\n");
#elif IS_X86_BIT_ARCH
    printf("Hello CMake at 32bits.\n");
#endif
	return 0;
}
