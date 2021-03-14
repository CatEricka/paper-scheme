#ifndef BASE_SCHEME_OBJECT_TEST_H
#define BASE_SCHEME_OBJECT_TEST_H
#pragma once

#include "base-scheme/object.h"
/**
 * ≤‚ ‘ object
 */


// test case here
UTEST(object_test, hello_utest) {
    ASSERT_TRUE(1);
}


#ifdef IS_32_BIT_ARCH
UTEST(object_test, arch_32bits) {
    printf("pointer size: %zd\n", sizeof(void *));
    ASSERT_TRUE(1);
}

#elif IS_64_BIT_ARCH
UTEST(object_test, arch_64bits) {
    printf("pointer size: %zd\n", sizeof(void *));
    ASSERT_TRUE(1);
}

#else
# error("Unknown arch")
#endif // IS_64_BIT_ARCH

UTEST(object_test, full_size) {
    printf("full object size:\t%zd, header size:\t%zd\n", sizeof(struct object_struct_t), object_sizeof_header());
    ASSERT_TRUE(1);
}

UTEST(object_test, double_size) {
    printf("double object size:\t%zd, header size:\t%zd\n", object_size(doublenum), object_sizeof_header());
    printf("double aligned size:\t%zd\n", aligned_size(object_size(doublenum)));
    ASSERT_TRUE(1);
}

UTEST(object_test, i64_size) {
    printf("i64 object size:\t%zd, header size:\t%zd\n", object_size(i64), object_sizeof_header());
    printf("i64 aligned size:\t%zd\n", aligned_size(object_size(i64)));
    ASSERT_TRUE(1);
}

UTEST(object_test, pair_size) {
    printf("pair object size:\t%zd, header size:\t%zd\n", object_size(pair), object_sizeof_header());
    printf("pair aligned size:\t%zd\n", aligned_size(object_size(pair)));
    ASSERT_TRUE(1);
}

UTEST(object_test, string_size) {
    printf("string object size:\t%zd, header size:\t%zd\n", object_size(string), object_sizeof_header());
    printf("string aligned size:\t%zd\n", aligned_size(object_size(string)));
    ASSERT_TRUE(1);
}

UTEST(object_test, symbol_size) {
    printf("symbol object size:\t%zd, header size:\t%zd\n", object_size(symbol), object_sizeof_header());
    printf("symbol aligned size:\t%zd\n", aligned_size(object_size(symbol)));
    ASSERT_TRUE(1);
}

UTEST(object_test, vector_size) {
    printf("vector object size:\t%zd, header size:\t%zd\n", object_size(vector), object_sizeof_header());
    printf("vector aligned size:\t%zd\n", aligned_size(object_size(vector)));
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

UTEST(object_test, char_imm_test) {
    for (int i = SCHAR_MIN; i <= SCHAR_MAX; i++) {
        char ch = (char) i;
        //printf("ch = %"PRId8", ch_imm = %"PRId8"\n", ch, char_imm_getvalue(char_imm_make(ch)));
        object obj = char_imm_make(ch);
        ASSERT_TRUE(is_char_imm(obj));
        ASSERT_TRUE(is_imm(obj));
        ASSERT_FALSE(is_object(obj));
        ASSERT_EQ(ch, char_imm_getvalue(obj));
    }
}

UTEST(object_test, i64_imm_test) {
    size_t n = 20;
    srand(0);
    int64_t *tests = malloc(n * sizeof(int64_t));

    // ≤‚ ‘”√¿˝
    tests[0] = I64_IMM_MIN;
    tests[1] = I64_IMM_MAX;
    tests[2] = 0;

    for (size_t i = 3; i < n; i++) {
        tests[i] = llabs(rand()) % I64_IMM_MAX;
    }

    for (size_t i = 0; i < n; i++) {
        object obj = i64_imm_make(tests[i]);
        //printf("i64 = %"PRId64", i64_imm = %"PRId64"\n", tests[i], i64_getvalue(obj));
        ASSERT_TRUE(is_i64(obj));
        ASSERT_TRUE(is_i64_imm(obj));
        ASSERT_TRUE(is_imm(obj));
        ASSERT_FALSE(is_object(obj));
        ASSERT_EQ(tests[i], i64_getvalue(obj));
    }

    for (size_t i = 0; i < n; i++) {
        tests[i] = -llabs(rand()) % I64_IMM_MAX;
    }

    for (size_t i = 0; i < n; i++) {
        object obj = i64_imm_make(tests[i]);
        //printf("i64 = %"PRId64", i64_imm = %"PRId64"\n", tests[i], i64_getvalue(obj));
        ASSERT_TRUE(is_i64(obj));
        ASSERT_TRUE(is_i64_imm(obj));
        ASSERT_TRUE(is_imm(obj));
        ASSERT_FALSE(is_object(obj));
        ASSERT_EQ(tests[i], i64_getvalue(obj));
    }
}


#endif // BASE_SCHEME_OBJECT_TEST_H