#ifndef BASE_SCHEME_VM_TEST_H
#define BASE_SCHEME_VM_TEST_H
#pragma once

#include "base-scheme/vm.h"


/**
 * 测试 vm
 */

UTEST(vm_test, hello) {
    ASSERT_TRUE(1);
}


UTEST(gc_test, alloc_test) {
    size_t init_size = 16u;
    size_t scale = 2u;
    size_t max_size = 500u;
    size_t n = 8;
    context_t context = context_make(init_size, scale, max_size);
    printf("heap init total size: %zd\n", context->heap->total_size);

    // 增长测试
    heap_grow(context->heap);
    object *objs = malloc(n * (sizeof(object)));

    // 分配测试
    for (size_t i = 0; i < n; i++) {
        objs[i] = i64_make(context, 200);
        ASSERT_TRUE(objs[i]);
    }

    // 容量限制
    ASSERT_LE(context->heap->total_size, max_size);

    // 堆信息
    printf("heap data: 0x%p, free_ptr: 0x%p\n", context->heap->first_node->data,
           context->heap->first_node->free_ptr);

    // 对象信息
    for (size_t i = 0; i < n; i++) {
        printf("object %zd: address=%p, value=%"PRId64"\n", i, objs[i], i64_getvalue(objs[i]));
    }

    // 分配容量计算
    size_t allocated = 0u;
    for (heap_node_t node = context->heap->first_node; node != NULL; node = node->next) {
        allocated += node->free_ptr - node->data;
    }
    printf("allocated: %td\n", allocated);
    printf("heap total size: %zd\n", context->heap->total_size);

    // 对象值变更测试
    for (size_t i = 0; i < n; i++) {
        objs[i]->value.i64 = 3000;
    }
    for (size_t i = 0; i < n; i++) {
        printf("object %zd: address=%p, value=%"PRId64"\n", i, objs[i], objs[i]->value.i64);
        ASSERT_EQ(objs[i]->value.i64, 3000);
    }

    // 释放堆结构
    context_destroy(context);
}

UTEST(vm_test, make_context_test) {
    context_t context = context_make(0x100, 2, 0x10000);
    heap_grow(context->heap);
    ASSERT_EQ(context->heap->total_size, context->heap->init_size + context->heap->init_size * 2);
    printf("heap total size: %zd\n", context->heap->total_size);
    context_destroy(context);
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
        object obj = i64_imm_auto_make(context, tests[i]);
        printf("i64 = %"PRId64", i64_imm = %"PRId64"\n", tests[i], i64_getvalue(obj));

        if (tests[i] >= I64_IMM_MIN && tests[i] <= I64_IMM_MAX) {
            // imm 范围
            ASSERT_TRUE(is_i64(obj));
            ASSERT_TRUE(is_i64_imm(obj));
            ASSERT_TRUE(is_imm(obj));
            ASSERT_FALSE(is_pointer(obj));
            ASSERT_EQ(tests[i], i64_getvalue(obj));
        } else {
            // 非 imm 范围
            ASSERT_TRUE(is_i64(obj));
            ASSERT_FALSE(is_i64_imm(obj));
            ASSERT_FALSE(is_imm(obj));
            ASSERT_TRUE(is_pointer(obj));
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
        object obj = i64_imm_auto_make(context, tests[i]);
        printf("i64 = %"PRId64", i64_imm = %"PRId64"\n", tests[i], i64_getvalue(obj));

        if (tests[i] >= I64_IMM_MIN && tests[i] <= I64_IMM_MAX) {
            // imm 范围
            ASSERT_TRUE(is_i64(obj));
            ASSERT_TRUE(is_i64_imm(obj));
            ASSERT_TRUE(is_imm(obj));
            ASSERT_FALSE(is_pointer(obj));
            ASSERT_EQ(tests[i], i64_getvalue(obj));
        } else {
            // 非 imm 范围
            ASSERT_TRUE(is_i64(obj));
            ASSERT_FALSE(is_i64_imm(obj));
            ASSERT_FALSE(is_imm(obj));
            ASSERT_TRUE(is_pointer(obj));
            ASSERT_EQ(tests[i], i64_getvalue(obj));
        }
    }
}


#endif //BASE_SCHEME_VM_TEST_H
