#ifndef BASE_SCHEME_GC_TEST_H
#define BASE_SCHEME_GC_TEST_H
#pragma once

#include "base-scheme/object.h"
#include "base-scheme/heap.h"
#include "base-scheme/context.h"
#include "base-scheme/gc.h"


/**
 * ≤‚ ‘ context, heap, gc
 */

// test case here
UTEST(gc_test, hello_utest) {
    ASSERT_TRUE(1);
}


UTEST(gc_test, heap_make_free_test) {

}

#endif // BASE_SCHEME_GC_TEST_H