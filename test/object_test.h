#ifndef _BASE_SCHEME_OBJECT_TEST_HEADER_
#define _BASE_SCHEME_OBJECT_TEST_HEADER_
#pragma once

#include "base-scheme/object.h"

#define UT(test_name) UTEST(object_test, test_name)

// test case here
UT(hello_utest) {
    ASSERT_TRUE(1);
}

UT(struct_offset) {
    printf("i32 object size: %zd, header size: %zd\n", object_size(doublenum), object_sizeof_header());
    ASSERT_TRUE(1);
}

UT(union_print) {
    struct object_struct_t s = {
        .value.doublenum = 20.0
    };
    printf("double: %f\n", s.value.doublenum);
    printf("i64: %"PRId64"\n", s.value.i64);
    ASSERT_TRUE(1);
}

#endif // _BASE_SCHEME_OBJECT_TEST_HEADER_