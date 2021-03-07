#ifndef _BASE_SCHEME_GC_TEST_HEADER_
#define _BASE_SCHEME_GC_TEST_HEADER_
#pragma once

#include "base-scheme/gc.h"

// test case here
UTEST(gc_test, hello_utest) {
    ASSERT_TRUE(1);
}

UTEST(gc_test, alloc_test) {
    object obj = mk_i64(200);
    ASSERT_EQ(obj->value.i64, 200);
}

#endif // _BASE_SCHEME_GC_TEST_HEADER_