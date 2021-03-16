#ifndef BASE_SCHEME_VM_TEST_H
#define BASE_SCHEME_VM_TEST_H
#pragma once

#include "paper-scheme/vm.h"


/**
 * 测试 vm
 * 对上线纹结构相关和虚拟机相关 API, 以及值类型在托管堆上的分配进行测试
 */

UTEST(vm_test, hello) {
    ASSERT_TRUE(1);
}

UTEST(vm_test, auto_imm_test) {
    size_t n = 20;
    srand(0);
    int64_t *tests = malloc(n * sizeof(int64_t));
    context_t context = context_make(0x100, 2, 0x10000);

    // 测试用例
    tests[0] = I64_IMM_MIN;
    tests[1] = I64_IMM_MAX;
    tests[2] = 0;

    for (size_t i = 3; i < n; i++) {
        tests[i] = llabs(rand()) % I64_IMM_MAX;
    }

    for (size_t i = 0; i < n; i++) {
        object obj = i64_make_op(context, tests[i]);
        //printf("i64 = %"PRId64", i64_imm = %"PRId64"\n", tests[i], i64_getvalue(obj));

        if (tests[i] >= I64_IMM_MIN && tests[i] <= I64_IMM_MAX) {
            // imm 范围
            ASSERT_TRUE(is_i64(obj));
            ASSERT_TRUE(is_imm_i64(obj));
            ASSERT_TRUE(is_imm(obj));
            ASSERT_FALSE(is_object(obj));
            ASSERT_EQ(tests[i], i64_getvalue(obj));
        } else {
            // 非 imm 范围
            ASSERT_TRUE(is_i64(obj));
            ASSERT_FALSE(is_imm_i64(obj));
            ASSERT_FALSE(is_imm(obj));
            ASSERT_TRUE(is_object(obj));
            ASSERT_EQ(tests[i], i64_getvalue(obj));
        }
    }

    for (size_t i = 0; i < n; i++) {
        tests[i] = -llabs(rand()) % I64_IMM_MAX;
    }

    tests[0] = INT64_MAX;
    tests[1] = INT64_MIN;
    tests[2] = INT64_MAX - llabs(rand());
    tests[3] = INT64_MIN + llabs(rand());

    for (size_t i = 0; i < n; i++) {
        object obj = i64_make_op(context, tests[i]);
        //UTEST_PRINTF("i64 = %"PRId64", i64_imm = %"PRId64"\n", tests[i], i64_getvalue(obj));

        if (tests[i] >= I64_IMM_MIN && tests[i] <= I64_IMM_MAX) {
            // imm 范围
            ASSERT_TRUE(is_i64(obj));
            ASSERT_TRUE(is_imm_i64(obj));
            ASSERT_TRUE(is_imm(obj));
            ASSERT_FALSE(is_object(obj));
            ASSERT_EQ(tests[i], i64_getvalue(obj));
        } else {
            // 非 imm 范围
            ASSERT_TRUE(is_i64(obj));
            ASSERT_FALSE(is_imm_i64(obj));
            ASSERT_FALSE(is_imm(obj));
            ASSERT_TRUE(is_object(obj));
            ASSERT_EQ(tests[i], i64_getvalue(obj));
        }
    }
}

#endif //BASE_SCHEME_VM_TEST_H
