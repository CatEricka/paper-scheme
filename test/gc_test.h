#ifndef BASE_SCHEME_GC_TEST_H
#define BASE_SCHEME_GC_TEST_H
#pragma once

#include "base-scheme/gc.h"
#include "utest.h"


// test case here
UTEST(gc_test, hello_utest) {
    ASSERT_TRUE(1);
}

UTEST(gc_test, alloc_test) {
    object obj = mk_i64(0, 200);
    ASSERT_EQ(obj->value.i64, 200);
}

#endif // BASE_SCHEME_GC_TEST_H