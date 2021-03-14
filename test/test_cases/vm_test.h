#ifndef BASE_SCHEME_VM_TEST_H
#define BASE_SCHEME_VM_TEST_H
#pragma once

#include "base-scheme/vm.h"


/**
 * ���� vm
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

    // ��������
    heap_grow(context->heap);
    object *objs = malloc(n * (sizeof(object)));

    // �������
    for (size_t i = 0; i < n; i++) {
        objs[i] = i64_make_real_object(context, 200);
        ASSERT_TRUE(objs[i]);
    }

    // ��������
    ASSERT_LE(context->heap->total_size, max_size);

    // ����Ϣ
    printf("heap data: 0x%p, free_ptr: 0x%p\n", context->heap->first_node->data,
           context->heap->first_node->free_ptr);

    // ������Ϣ
    for (size_t i = 0; i < n; i++) {
        ASSERT_EQ(i64_getvalue(objs[i]), 200);
        //printf("object %zd: address=%p, value=%"PRId64"\n", i, objs[i], i64_getvalue(objs[i]));
    }

    // ������������
    size_t allocated = 0u;
    for (heap_node_t node = context->heap->first_node; node != NULL; node = node->next) {
        allocated += node->free_ptr - node->data;
    }
    printf("allocated: %td\n", allocated);
    printf("heap total size: %zd\n", context->heap->total_size);

    // ����ֵ�������
    for (size_t i = 0; i < n; i++) {
        objs[i]->value.i64 = 3000;
    }
    for (size_t i = 0; i < n; i++) {
        //printf("object %zd: address=%p, value=%"PRId64"\n", i, objs[i], objs[i]->value.i64);
        ASSERT_EQ(objs[i]->value.i64, 3000);
    }

    // �ͷŶѽṹ
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

    // ��������
    tests[0] = I64_IMM_MIN;
    tests[1] = I64_IMM_MAX;
    tests[2] = 0;

    for (size_t i = 3; i < n; i++) {
        tests[i] = llabs(rand()) % I64_IMM_MAX;
    }

    for (size_t i = 0; i < n; i++) {
        object obj = i64_make(context, tests[i]);
        //printf("i64 = %"PRId64", i64_imm = %"PRId64"\n", tests[i], i64_getvalue(obj));

        if (tests[i] >= I64_IMM_MIN && tests[i] <= I64_IMM_MAX) {
            // imm ��Χ
            ASSERT_TRUE(is_i64(obj));
            ASSERT_TRUE(is_i64_imm(obj));
            ASSERT_TRUE(is_imm(obj));
            ASSERT_FALSE(is_object(obj));
            ASSERT_EQ(tests[i], i64_getvalue(obj));
        } else {
            // �� imm ��Χ
            ASSERT_TRUE(is_i64(obj));
            ASSERT_FALSE(is_i64_imm(obj));
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
        object obj = i64_make(context, tests[i]);
        //printf("i64 = %"PRId64", i64_imm = %"PRId64"\n", tests[i], i64_getvalue(obj));

        if (tests[i] >= I64_IMM_MIN && tests[i] <= I64_IMM_MAX) {
            // imm ��Χ
            ASSERT_TRUE(is_i64(obj));
            ASSERT_TRUE(is_i64_imm(obj));
            ASSERT_TRUE(is_imm(obj));
            ASSERT_FALSE(is_object(obj));
            ASSERT_EQ(tests[i], i64_getvalue(obj));
        } else {
            // �� imm ��Χ
            ASSERT_TRUE(is_i64(obj));
            ASSERT_FALSE(is_i64_imm(obj));
            ASSERT_FALSE(is_imm(obj));
            ASSERT_TRUE(is_object(obj));
            ASSERT_EQ(tests[i], i64_getvalue(obj));
        }
    }
}


UTEST(vm_test, is_a_test) {
    context_t context = context_make(0x100, 2, 0x100000);
    object obj = NULL;
    // TODO ʵ�ֲ���

    object null_object = NULL;
    obj = null_object;
    ASSERT_TRUE(is_null(obj));
    ASSERT_FALSE(is_imm(obj));
    ASSERT_FALSE(is_unique_imm(obj));
    ASSERT_FALSE(is_i64_real(obj));
    ASSERT_FALSE(is_i64_imm(obj));
    ASSERT_FALSE(is_char_imm(obj));
    ASSERT_FALSE(is_object(obj));
    ASSERT_FALSE(is_i64(obj));
    ASSERT_FALSE(is_doublenum(obj));
    ASSERT_FALSE(is_pair(obj));
    ASSERT_FALSE(is_string(obj));
    ASSERT_FALSE(is_symbol(obj));
    ASSERT_FALSE(is_vector(obj));

    object imm_true = IMM_TRUE;
    obj = imm_true;
    ASSERT_FALSE(is_null(obj));
    ASSERT_FALSE(is_null(obj));
    ASSERT_TRUE(is_imm(obj));
    ASSERT_TRUE(is_unique_imm(obj));
    ASSERT_FALSE(is_i64_real(obj));
    ASSERT_FALSE(is_i64_imm(obj));
    ASSERT_FALSE(is_char_imm(obj));
    ASSERT_FALSE(is_object(obj));
    ASSERT_FALSE(is_i64(obj));
    ASSERT_FALSE(is_doublenum(obj));
    ASSERT_FALSE(is_pair(obj));
    ASSERT_FALSE(is_string(obj));
    ASSERT_FALSE(is_symbol(obj));
    ASSERT_FALSE(is_vector(obj));

    object imm_false = IMM_FALSE;
    obj = imm_false;
    ASSERT_FALSE(is_null(obj));
    ASSERT_TRUE(is_imm(obj));
    ASSERT_TRUE(is_unique_imm(obj));
    ASSERT_FALSE(is_i64_real(obj));
    ASSERT_FALSE(is_i64_imm(obj));
    ASSERT_FALSE(is_char_imm(obj));
    ASSERT_FALSE(is_object(obj));
    ASSERT_FALSE(is_i64(obj));
    ASSERT_FALSE(is_doublenum(obj));
    ASSERT_FALSE(is_pair(obj));
    ASSERT_FALSE(is_string(obj));
    ASSERT_FALSE(is_symbol(obj));
    ASSERT_FALSE(is_vector(obj));

    object imm_unit = IMM_UNIT;
    obj = imm_unit;
    ASSERT_FALSE(is_null(obj));
    ASSERT_TRUE(is_imm(obj));
    ASSERT_TRUE(is_unique_imm(obj));
    ASSERT_FALSE(is_i64_real(obj));
    ASSERT_FALSE(is_i64_imm(obj));
    ASSERT_FALSE(is_char_imm(obj));
    ASSERT_FALSE(is_object(obj));
    ASSERT_FALSE(is_i64(obj));
    ASSERT_FALSE(is_doublenum(obj));
    ASSERT_FALSE(is_pair(obj));
    ASSERT_FALSE(is_string(obj));
    ASSERT_FALSE(is_symbol(obj));
    ASSERT_FALSE(is_vector(obj));

    object imm_char = char_imm_make('x');
    obj = imm_char;
    ASSERT_FALSE(is_null(obj));
    ASSERT_TRUE(is_imm(obj));
    ASSERT_FALSE(is_unique_imm(obj));
    ASSERT_FALSE(is_i64_real(obj));
    ASSERT_FALSE(is_i64_imm(obj));
    ASSERT_TRUE(is_char_imm(obj));
    ASSERT_FALSE(is_object(obj));
    ASSERT_FALSE(is_i64(obj));
    ASSERT_FALSE(is_doublenum(obj));
    ASSERT_FALSE(is_pair(obj));
    ASSERT_FALSE(is_string(obj));
    ASSERT_FALSE(is_symbol(obj));
    ASSERT_FALSE(is_vector(obj));

    object imm_i64 = i64_imm_make(123);
    obj = imm_i64;
    ASSERT_FALSE(is_null(obj));
    ASSERT_TRUE(is_imm(obj));
    ASSERT_FALSE(is_unique_imm(obj));
    ASSERT_FALSE(is_i64_real(obj));
    ASSERT_TRUE(is_i64_imm(obj));
    ASSERT_FALSE(is_char_imm(obj));
    ASSERT_FALSE(is_object(obj));
    ASSERT_TRUE(is_i64(obj));
    ASSERT_FALSE(is_doublenum(obj));
    ASSERT_FALSE(is_pair(obj));
    ASSERT_FALSE(is_string(obj));
    ASSERT_FALSE(is_symbol(obj));
    ASSERT_FALSE(is_vector(obj));

    object i64 = i64_make_real_object(context, 20);
    obj = i64;
    ASSERT_FALSE(is_null(obj));
    ASSERT_FALSE(is_imm(obj));
    ASSERT_FALSE(is_unique_imm(obj));
    ASSERT_TRUE(is_i64_real(obj));
    ASSERT_FALSE(is_i64_imm(obj));
    ASSERT_FALSE(is_char_imm(obj));
    ASSERT_TRUE(is_object(obj));
    ASSERT_TRUE(is_i64(obj));
    ASSERT_FALSE(is_doublenum(obj));
    ASSERT_FALSE(is_pair(obj));
    ASSERT_FALSE(is_string(obj));
    ASSERT_FALSE(is_symbol(obj));
    ASSERT_FALSE(is_vector(obj));

    object doublenum = doublenum_make(context, 200.0);
    obj = doublenum;
    ASSERT_FALSE(is_null(obj));
    ASSERT_FALSE(is_imm(obj));
    ASSERT_FALSE(is_unique_imm(obj));
    ASSERT_FALSE(is_i64_real(obj));
    ASSERT_FALSE(is_i64_imm(obj));
    ASSERT_FALSE(is_char_imm(obj));
    ASSERT_TRUE(is_object(obj));
    ASSERT_FALSE(is_i64(obj));
    ASSERT_TRUE(is_doublenum(obj));
    ASSERT_FALSE(is_pair(obj));
    ASSERT_FALSE(is_string(obj));
    ASSERT_FALSE(is_symbol(obj));
    ASSERT_FALSE(is_vector(obj));

    object pair_carImmI64_cdrDouble = pair_make(context, imm_i64, doublenum);
    obj = pair_carImmI64_cdrDouble;
    ASSERT_FALSE(is_null(obj));
    ASSERT_FALSE(is_imm(obj));
    ASSERT_FALSE(is_unique_imm(obj));
    ASSERT_FALSE(is_i64_real(obj));
    ASSERT_FALSE(is_i64_imm(obj));
    ASSERT_FALSE(is_char_imm(obj));
    ASSERT_TRUE(is_object(obj));
    ASSERT_FALSE(is_i64(obj));
    ASSERT_FALSE(is_doublenum(obj));
    ASSERT_TRUE(is_pair(obj));
    ASSERT_FALSE(is_string(obj));
    ASSERT_FALSE(is_symbol(obj));
    ASSERT_FALSE(is_vector(obj));

    object string_obj = string_make_from_cstr(context, "this is a string object");
    obj = string_obj;
    ASSERT_FALSE(is_null(obj));
    ASSERT_FALSE(is_imm(obj));
    ASSERT_FALSE(is_unique_imm(obj));
    ASSERT_FALSE(is_i64_real(obj));
    ASSERT_FALSE(is_i64_imm(obj));
    ASSERT_FALSE(is_char_imm(obj));
    ASSERT_TRUE(is_object(obj));
    ASSERT_FALSE(is_i64(obj));
    ASSERT_FALSE(is_doublenum(obj));
    ASSERT_FALSE(is_pair(obj));
    ASSERT_TRUE(is_string(obj));
    ASSERT_FALSE(is_symbol(obj));
    ASSERT_FALSE(is_vector(obj));

    object string_null_object = string_make_from_cstr(context, NULL);
    obj = string_null_object;
    ASSERT_FALSE(is_null(obj));
    ASSERT_FALSE(is_imm(obj));
    ASSERT_FALSE(is_unique_imm(obj));
    ASSERT_FALSE(is_i64_real(obj));
    ASSERT_FALSE(is_i64_imm(obj));
    ASSERT_FALSE(is_char_imm(obj));
    ASSERT_TRUE(is_object(obj));
    ASSERT_FALSE(is_i64(obj));
    ASSERT_FALSE(is_doublenum(obj));
    ASSERT_FALSE(is_pair(obj));
    ASSERT_TRUE(is_string(obj));
    ASSERT_FALSE(is_symbol(obj));
    ASSERT_FALSE(is_vector(obj));

    object string_empty = string_make_from_cstr(context, "");
    obj = string_empty;
    ASSERT_FALSE(is_null(obj));
    ASSERT_FALSE(is_imm(obj));
    ASSERT_FALSE(is_unique_imm(obj));
    ASSERT_FALSE(is_i64_real(obj));
    ASSERT_FALSE(is_i64_imm(obj));
    ASSERT_FALSE(is_char_imm(obj));
    ASSERT_TRUE(is_object(obj));
    ASSERT_FALSE(is_i64(obj));
    ASSERT_FALSE(is_doublenum(obj));
    ASSERT_FALSE(is_pair(obj));
    ASSERT_TRUE(is_string(obj));
    ASSERT_FALSE(is_symbol(obj));
    ASSERT_FALSE(is_vector(obj));

    object symbol_obj = symbol_make_from_cstr(context, "this is a symbol object");
    obj = symbol_obj;
    ASSERT_FALSE(is_null(obj));
    ASSERT_FALSE(is_imm(obj));
    ASSERT_FALSE(is_unique_imm(obj));
    ASSERT_FALSE(is_i64_real(obj));
    ASSERT_FALSE(is_i64_imm(obj));
    ASSERT_FALSE(is_char_imm(obj));
    ASSERT_TRUE(is_object(obj));
    ASSERT_FALSE(is_i64(obj));
    ASSERT_FALSE(is_doublenum(obj));
    ASSERT_FALSE(is_pair(obj));
    ASSERT_FALSE(is_string(obj));
    ASSERT_TRUE(is_symbol(obj));
    ASSERT_FALSE(is_vector(obj));

    object symbol_null_object = symbol_make_from_cstr(context, NULL);
    obj = symbol_null_object;
    ASSERT_FALSE(is_null(obj));
    ASSERT_FALSE(is_imm(obj));
    ASSERT_FALSE(is_unique_imm(obj));
    ASSERT_FALSE(is_i64_real(obj));
    ASSERT_FALSE(is_i64_imm(obj));
    ASSERT_FALSE(is_char_imm(obj));
    ASSERT_TRUE(is_object(obj));
    ASSERT_FALSE(is_i64(obj));
    ASSERT_FALSE(is_doublenum(obj));
    ASSERT_FALSE(is_pair(obj));
    ASSERT_FALSE(is_string(obj));
    ASSERT_TRUE(is_symbol(obj));
    ASSERT_FALSE(is_vector(obj));

    object symbol_empty = symbol_make_from_cstr(context, "");
    obj = symbol_empty;
    ASSERT_FALSE(is_null(obj));
    ASSERT_FALSE(is_imm(obj));
    ASSERT_FALSE(is_unique_imm(obj));
    ASSERT_FALSE(is_i64_real(obj));
    ASSERT_FALSE(is_i64_imm(obj));
    ASSERT_FALSE(is_char_imm(obj));
    ASSERT_TRUE(is_object(obj));
    ASSERT_FALSE(is_i64(obj));
    ASSERT_FALSE(is_doublenum(obj));
    ASSERT_FALSE(is_pair(obj));
    ASSERT_FALSE(is_string(obj));
    ASSERT_TRUE(is_symbol(obj));
    ASSERT_FALSE(is_vector(obj));

    object vector10 = vector_make(context, 10);
    obj = vector10;
    ASSERT_FALSE(is_null(obj));
    ASSERT_FALSE(is_imm(obj));
    ASSERT_FALSE(is_unique_imm(obj));
    ASSERT_FALSE(is_i64_real(obj));
    ASSERT_FALSE(is_i64_imm(obj));
    ASSERT_FALSE(is_char_imm(obj));
    ASSERT_TRUE(is_object(obj));
    ASSERT_FALSE(is_i64(obj));
    ASSERT_FALSE(is_doublenum(obj));
    ASSERT_FALSE(is_pair(obj));
    ASSERT_FALSE(is_string(obj));
    ASSERT_FALSE(is_symbol(obj));
    ASSERT_TRUE(is_vector(obj));

    object vector20 = vector_make(context, 20);
    obj = vector20;
    ASSERT_FALSE(is_null(obj));
    ASSERT_FALSE(is_imm(obj));
    ASSERT_FALSE(is_unique_imm(obj));
    ASSERT_FALSE(is_i64_real(obj));
    ASSERT_FALSE(is_i64_imm(obj));
    ASSERT_FALSE(is_char_imm(obj));
    ASSERT_TRUE(is_object(obj));
    ASSERT_FALSE(is_i64(obj));
    ASSERT_FALSE(is_doublenum(obj));
    ASSERT_FALSE(is_pair(obj));
    ASSERT_FALSE(is_string(obj));
    ASSERT_FALSE(is_symbol(obj));
    ASSERT_TRUE(is_vector(obj));

    object vector0 = vector_make(context, 0);
    obj = vector0;
    ASSERT_FALSE(is_null(obj));
    ASSERT_FALSE(is_imm(obj));
    ASSERT_FALSE(is_unique_imm(obj));
    ASSERT_FALSE(is_i64_real(obj));
    ASSERT_FALSE(is_i64_imm(obj));
    ASSERT_FALSE(is_char_imm(obj));
    ASSERT_TRUE(is_object(obj));
    ASSERT_FALSE(is_i64(obj));
    ASSERT_FALSE(is_doublenum(obj));
    ASSERT_FALSE(is_pair(obj));
    ASSERT_FALSE(is_string(obj));
    ASSERT_FALSE(is_symbol(obj));
    ASSERT_TRUE(is_vector(obj));
}

#endif //BASE_SCHEME_VM_TEST_H
