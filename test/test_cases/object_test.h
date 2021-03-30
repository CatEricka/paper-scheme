#ifndef BASE_SCHEME_OBJECT_TEST_H
#define BASE_SCHEME_OBJECT_TEST_H
#pragma once


#include "paper-scheme/object.h"


/**
 * 测试 object
 * 主要测试标记指针和内存布局是否工作正常
 * 对 object 结构进行静态断言
 */

UTEST(object_test, i64_arithmetic_right_shift_test) {
    // ASSERT_TRUE((-1 >> 2) < 0);
    ASSERT_EQ(i64_arithmetic_right_shift(-2, 1u), -1);
    ASSERT_EQ(i64_arithmetic_right_shift(2, 1u), 1);
    ASSERT_EQ(i64_arithmetic_right_shift(0, 1u), 0);
}


// test case here
UTEST(object_test, hello_utest) {
    ASSERT_TRUE(1);
}


#ifdef IS_32_BIT_ARCH
UTEST(object_test, arch_32bits) {
    UTEST_PRINTF("pointer size: %zd\n", sizeof(void *));
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
    UTEST_PRINTF("full object size:\t%zd, header size:\t%zd\n", sizeof(struct object_struct_t), object_sizeof_header());
    ASSERT_TRUE(1);
}

UTEST(object_test, double_size) {
    UTEST_PRINTF("double object size:\t%zd, header size:\t%zd\n", object_sizeof_base(doublenum),
                 object_sizeof_header());
    UTEST_PRINTF("double aligned size:\t%zd\n", aligned_size(object_sizeof_base(doublenum)));
    ASSERT_TRUE(1);
}

UTEST(object_test, i64_size) {
    UTEST_PRINTF("i64 object size:\t%zd, header size:\t%zd\n", object_sizeof_base(i64), object_sizeof_header());
    UTEST_PRINTF("i64 aligned size:\t%zd\n", aligned_size(object_sizeof_base(i64)));
    ASSERT_TRUE(1);
}

UTEST(object_test, pair_size) {
    UTEST_PRINTF("pair object size:\t%zd, header size:\t%zd\n", object_sizeof_base(pair), object_sizeof_header());
    UTEST_PRINTF("pair aligned size:\t%zd\n", aligned_size(object_sizeof_base(pair)));
    ASSERT_TRUE(1);
}

UTEST(object_test, string_size) {
    UTEST_PRINTF("string object size:\t%zd, header size:\t%zd\n", object_sizeof_base(string), object_sizeof_header());
    UTEST_PRINTF("string aligned size:\t%zd\n", aligned_size(object_sizeof_base(string)));
    ASSERT_TRUE(1);
}

UTEST(object_test, symbol_size) {
    UTEST_PRINTF("symbol object size:\t%zd, header size:\t%zd\n", object_sizeof_base(symbol), object_sizeof_header());
    UTEST_PRINTF("symbol aligned size:\t%zd\n", aligned_size(object_sizeof_base(symbol)));
    ASSERT_TRUE(1);
}

UTEST(object_test, vector_size) {
    UTEST_PRINTF("vector object size:\t%zd, header size:\t%zd\n", object_sizeof_base(vector), object_sizeof_header());
    UTEST_PRINTF("vector aligned size:\t%zd\n", aligned_size(object_sizeof_base(vector)));
    ASSERT_TRUE(1);
}


UTEST(object_test, union_print) {
    struct object_struct_t s = {
            .value.doublenum = 20.0
    };
    UTEST_PRINTF("double: %f\n", s.value.doublenum);
    UTEST_PRINTF("i64: %"
                         PRId64
                         "\n", s.value.i64);
    ASSERT_TRUE(1);
}

UTEST(object_test, char_imm_test) {
    for (int i = SCHAR_MIN; i <= SCHAR_MAX; i++) {
        char ch = (char) i;
        //UTEST_PRINTF("ch = %"PRId8", ch_imm = %"PRId8"\n", ch, char_imm_getvalue(char_imm_make(ch)));
        object obj = char_imm_make(ch);
        ASSERT_TRUE(is_imm_char(obj));
        ASSERT_TRUE(is_imm(obj));
        ASSERT_FALSE(is_object(obj));
        ASSERT_EQ(ch, char_imm_getvalue(obj));
    }
}

UTEST(object_test, i64_imm_test) {
    size_t n = 20;
    srand(0);
    int64_t *tests = malloc(n * sizeof(int64_t));
    ASSERT_TRUE(tests);

    // 测试用例
    tests[0] = I64_IMM_MIN;
    tests[1] = I64_IMM_MAX;
    tests[2] = 0;

    for (size_t i = 3; i < n; i++) {
        tests[i] = llabs(rand()) % I64_IMM_MAX;
    }

    for (size_t i = 0; i < n; i++) {
        object obj = i64_imm_make(tests[i]);
        //UTEST_PRINTF("i64 = %"PRId64", i64_imm = %"PRId64"\n", tests[i], i64_getvalue(obj));
        ASSERT_TRUE(is_i64(obj));
        ASSERT_TRUE(is_imm_i64(obj));
        ASSERT_TRUE(is_imm(obj));
        ASSERT_FALSE(is_object(obj));
        ASSERT_EQ(tests[i], i64_getvalue(obj));
    }

    for (size_t i = 0; i < n; i++) {
        tests[i] = -llabs(rand()) % I64_IMM_MAX;
    }

    for (size_t i = 0; i < n; i++) {
        object obj = i64_imm_make(tests[i]);
        //UTEST_PRINTF("i64 = %"PRId64", i64_imm = %"PRId64"\n", tests[i], i64_getvalue(obj));
        ASSERT_TRUE(is_i64(obj));
        ASSERT_TRUE(is_imm_i64(obj));
        ASSERT_TRUE(is_imm(obj));
        ASSERT_FALSE(is_object(obj));
        ASSERT_EQ(tests[i], i64_getvalue(obj));
    }
}


#endif // BASE_SCHEME_OBJECT_TEST_H