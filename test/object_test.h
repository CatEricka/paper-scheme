#ifndef _BASE_SCHEME_OBJECT_TEST_HEADER_
#define _BASE_SCHEME_OBJECT_TEST_HEADER_
#pragma once

#include "base-scheme/object.h"

// test case here
UTEST(object_test, hello_utest) {
    ASSERT_TRUE(1);
}

#ifdef IS_64_BIT_ARCH
UTEST(object_test, arch_64bits) {
    ASSERT_TRUE(1);
}
#else
UTEST(object_test, arch_32bits) {
    ASSERT_TRUE(1);
}
#endif // IS_64_BIT_ARCH

UTEST(object_test, full_size) {
    printf("full object size:\t%zd, header size:\t%zd\n", sizeof(struct object_struct_t), object_sizeof_header());
    ASSERT_TRUE(1);
}

UTEST(object_test, double_size) {
    printf("double object size:\t%zd, header size:\t%zd\n", object_size(doublenum), object_sizeof_header());
    ASSERT_TRUE(1);
}

UTEST(object_test, i64_size) {
    printf("i64 object size:\t%zd, header size:\t%zd\n", object_size(i64), object_sizeof_header());
    ASSERT_TRUE(1);
}

UTEST(object_test, union_print) {
    struct object_struct_t s = {
        .value.doublenum = 20.0
    };
    printf("double: %f\n", s.value.doublenum);
    printf("i64: %"PRId64"\n", s.value.i64);
    ASSERT_TRUE(1);
}

#endif // _BASE_SCHEME_OBJECT_TEST_HEADER_