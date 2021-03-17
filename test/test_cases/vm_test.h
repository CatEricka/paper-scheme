#ifndef BASE_SCHEME_VM_TEST_H
#define BASE_SCHEME_VM_TEST_H
#pragma once

#include "paper-scheme/vm.h"


/**
 * 测试 vm
 * 对上下文结构相关和虚拟机相关 API, 以及值类型在托管堆上的分配进行测试
 * 测试运行时类型系统和上下文全局信息表
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

UTEST(vm_test, context_runtime_type_test) {
    context_t context = context_make(16, 2, 0x10000);
    size_t size1, size2;

    // 完成测试
    object i64 = i64_make_real_object_op(context, 20);
    size1 = object_bootstrap_sizeof(i64);
    size2 = context_object_sizeof(context, i64);
    ASSERT_EQ(size1, size2);

    object i64max = i64_make_op(context, INT64_MAX);
    size1 = object_bootstrap_sizeof(i64max);
    size2 = context_object_sizeof(context, i64max);
    ASSERT_EQ(size1, size2);

    object i64min = i64_make_real_object_op(context, INT64_MIN);
    size1 = object_bootstrap_sizeof(i64min);
    size2 = context_object_sizeof(context, i64min);
    ASSERT_EQ(size1, size2);

    object doublenum = doublenum_make_op(context, 200.0);
    size1 = object_bootstrap_sizeof(doublenum);
    size2 = context_object_sizeof(context, doublenum);
    ASSERT_EQ(size1, size2);

    object pair_carImmI64_cdrDouble = pair_make_op(context, i64, doublenum);
    size1 = object_bootstrap_sizeof(pair_carImmI64_cdrDouble);
    size2 = context_object_sizeof(context, pair_carImmI64_cdrDouble);
    ASSERT_EQ(size1, size2);
    size_t off = object_offsetof(pair, car);
    object car = *type_info_get_object_of_first_member(context_get_object_type(context, pair_carImmI64_cdrDouble),
                                                       pair_carImmI64_cdrDouble);
    object cdr = *(type_info_get_object_of_first_member(context_get_object_type(context, pair_carImmI64_cdrDouble),
                                                        pair_carImmI64_cdrDouble) + 1);
    ASSERT_EQ(car, i64);
    ASSERT_EQ(cdr, doublenum);

    object string_obj = string_make_from_cstr_op(context, "this is a string object");
    size1 = object_bootstrap_sizeof(string_obj);
    size2 = context_object_sizeof(context, string_obj);
    ASSERT_EQ(size1, size2);

    object string_null_object = string_make_from_cstr_op(context, NULL);
    size1 = object_bootstrap_sizeof(string_null_object);
    size2 = context_object_sizeof(context, string_null_object);
    ASSERT_EQ(size1, size2);

    object string_empty = string_make_from_cstr_op(context, "");
    size1 = object_bootstrap_sizeof(string_empty);
    size2 = context_object_sizeof(context, string_empty);
    ASSERT_EQ(size1, size2);

    object symbol_obj = symbol_make_from_cstr_op(context, "this is a symbol object");
    size1 = object_bootstrap_sizeof(symbol_obj);
    size2 = context_object_sizeof(context, symbol_obj);
    ASSERT_EQ(size1, size2);

    object vector10 = vector_make_op(context, 10);
    size1 = object_bootstrap_sizeof(vector10);
    size2 = context_object_sizeof(context, vector10);
    ASSERT_EQ(size1, size2);
    object_type_info type = context_get_object_type(context, vector10);
    ASSERT_EQ(object_type_info_member_slots_of(type, vector10), vector10->value.vector.len);
    ASSERT_EQ(object_type_info_member_eq_slots_of(type, vector10), vector10->value.vector.len);
    object *fp = type_info_get_object_of_first_member(type, vector10);
    for (size_t i = 0; i < object_type_info_member_slots_of(type, vector10); i++) {
        ASSERT_EQ(vector_ref(vector10, i), *fp);
    }
}

#endif //BASE_SCHEME_VM_TEST_H
