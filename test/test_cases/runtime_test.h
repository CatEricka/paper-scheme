#ifndef BASE_SCHEME_RUNTIME_TEST_H
#define BASE_SCHEME_RUNTIME_TEST_H
#pragma once


#include "paper-scheme/runtime.h"


/**
 * 测试 解释器
 * 对上下文结构相关和解释器相关 API, 以及值类型在托管堆上的分配进行测试
 * 测试运行时类型系统和上下文全局信息表
 */

UTEST(runtime_test, hello) {
    ASSERT_TRUE(1);
}

UTEST(runtime_test, auto_imm_test) {
    size_t n = 20;
    srand(0);
    int64_t *tests = malloc(n * sizeof(int64_t));
    ASSERT_TRUE(tests);
    context_t context = context_make(0x100, 2, 0x10000);
    gc_collect_disable(context);

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

UTEST(runtime_test, context_runtime_type_test) {
    context_t context = context_make(16, 2, 0x10000);
    gc_collect_disable(context);
    object_type_info_ptr t;
    size_t size1, size2;
    object obj;

    // todo 增加初始类型后, 修改此测试测试
    object i64 = i64_make_real_object_op(context, 20);
    t = context_get_object_type(context, i64);
    obj = i64;
    ASSERT_EQ(object_bootstrap_sizeof(i64), context_object_sizeof(context, i64));
    ASSERT_EQ(type_info_member_base(t), 0);
    ASSERT_EQ(type_info_member_eq_len_base(t), 0);
    ASSERT_EQ(type_info_member_len_base(t), 0);
    ASSERT_EQ(type_info_member_meta_len_offset(t), 0);
    ASSERT_EQ(type_info_member_meta_len_scale(t), 0);
    ASSERT_EQ(type_info_size_base(t), object_sizeof_base(i64));
    ASSERT_EQ(type_info_size_meta_size_offset(t), 0);
    ASSERT_EQ(type_info_size_meta_size_scale(t), 0);
    ASSERT_EQ(object_type_info_member_slots_of(t, obj), 0);
    ASSERT_EQ(object_type_info_member_eq_slots_of(t, obj), 0);

    object i64max = i64_make_op(context, INT64_MAX);
    t = context_get_object_type(context, i64max);
    obj = i64max;
    ASSERT_EQ(object_bootstrap_sizeof(i64), context_object_sizeof(context, i64));
    ASSERT_EQ(type_info_member_base(t), 0);
    ASSERT_EQ(type_info_member_eq_len_base(t), 0);
    ASSERT_EQ(type_info_member_len_base(t), 0);
    ASSERT_EQ(type_info_member_meta_len_offset(t), 0);
    ASSERT_EQ(type_info_member_meta_len_scale(t), 0);
    ASSERT_EQ(type_info_size_base(t), object_sizeof_base(i64));
    ASSERT_EQ(type_info_size_meta_size_offset(t), 0);
    ASSERT_EQ(type_info_size_meta_size_scale(t), 0);
    ASSERT_EQ(object_type_info_member_slots_of(t, obj), 0);
    ASSERT_EQ(object_type_info_member_eq_slots_of(t, obj), 0);

    object i64min = i64_make_real_object_op(context, INT64_MIN);
    t = context_get_object_type(context, i64min);
    obj = i64min;
    ASSERT_EQ(object_bootstrap_sizeof(i64), context_object_sizeof(context, i64));
    ASSERT_EQ(type_info_member_base(t), 0);
    ASSERT_EQ(type_info_member_eq_len_base(t), 0);
    ASSERT_EQ(type_info_member_len_base(t), 0);
    ASSERT_EQ(type_info_member_meta_len_offset(t), 0);
    ASSERT_EQ(type_info_member_meta_len_scale(t), 0);
    ASSERT_EQ(type_info_size_base(t), object_sizeof_base(i64));
    ASSERT_EQ(type_info_size_meta_size_offset(t), 0);
    ASSERT_EQ(type_info_size_meta_size_scale(t), 0);
    ASSERT_EQ(object_type_info_member_slots_of(t, obj), 0);
    ASSERT_EQ(object_type_info_member_eq_slots_of(t, obj), 0);

    object doublenum = doublenum_make_op(context, 200.0);
    t = context_get_object_type(context, doublenum);
    obj = doublenum;
    ASSERT_EQ(object_bootstrap_sizeof(doublenum), context_object_sizeof(context, i64));
    ASSERT_EQ(type_info_member_base(t), 0);
    ASSERT_EQ(type_info_member_eq_len_base(t), 0);
    ASSERT_EQ(type_info_member_len_base(t), 0);
    ASSERT_EQ(type_info_member_meta_len_offset(t), 0);
    ASSERT_EQ(type_info_member_meta_len_scale(t), 0);
    ASSERT_EQ(type_info_size_base(t), object_sizeof_base(doublenum));
    ASSERT_EQ(type_info_size_meta_size_offset(t), 0);
    ASSERT_EQ(type_info_size_meta_size_scale(t), 0);
    ASSERT_EQ(object_type_info_member_slots_of(t, obj), 0);
    ASSERT_EQ(object_type_info_member_eq_slots_of(t, obj), 0);

    object pair_carI64_cdrDouble = pair_make_op(context, i64, doublenum);
    size1 = object_bootstrap_sizeof(pair_carI64_cdrDouble);
    size2 = context_object_sizeof(context, pair_carI64_cdrDouble);
    ASSERT_EQ(size1, size2);
    object car = *type_info_get_object_ptr_of_first_member(context_get_object_type(context, pair_carI64_cdrDouble),
                                                           pair_carI64_cdrDouble);
    object cdr = *(type_info_get_object_ptr_of_first_member(context_get_object_type(context, pair_carI64_cdrDouble),
                                                            pair_carI64_cdrDouble) + 1);
    ASSERT_EQ(car, i64);
    ASSERT_EQ(cdr, doublenum);
    t = context_get_object_type(context, pair_carI64_cdrDouble);
    obj = pair_carI64_cdrDouble;
    ASSERT_EQ(object_bootstrap_sizeof(pair_carI64_cdrDouble), context_object_sizeof(context, pair_carI64_cdrDouble));
    ASSERT_EQ(type_info_member_base(t), object_offsetof(pair, car));
    ASSERT_EQ(type_info_member_eq_len_base(t), 2);
    ASSERT_EQ(type_info_member_len_base(t), 2);
    ASSERT_EQ(type_info_member_meta_len_offset(t), 0);
    ASSERT_EQ(type_info_member_meta_len_scale(t), 0);
    ASSERT_EQ(type_info_size_base(t), object_sizeof_base(pair));
    ASSERT_EQ(type_info_size_meta_size_offset(t), 0);
    ASSERT_EQ(type_info_size_meta_size_scale(t), 0);
    ASSERT_EQ(object_type_info_member_slots_of(t, obj), 2);
    ASSERT_EQ(object_type_info_member_eq_slots_of(t, obj), 2);

    object string_obj = string_make_from_cstr_op(context, "this is a string object");
    size1 = object_bootstrap_sizeof(string_obj);
    size2 = context_object_sizeof(context, string_obj);
    ASSERT_EQ(size1, size2);
    t = context_get_object_type(context, string_obj);
    obj = string_obj;
    ASSERT_EQ(object_bootstrap_sizeof(string_obj), context_object_sizeof(context, string_obj));
    ASSERT_EQ(type_info_member_base(t), 0);
    ASSERT_EQ(type_info_member_eq_len_base(t), 0);
    ASSERT_EQ(type_info_member_len_base(t), 0);
    ASSERT_EQ(type_info_member_meta_len_offset(t), 0);
    ASSERT_EQ(type_info_member_meta_len_scale(t), 0);
    ASSERT_EQ(type_info_size_base(t), object_sizeof_base(string));
    ASSERT_EQ(type_info_size_meta_size_offset(t), object_offsetof(string, len));
    ASSERT_EQ(type_info_size_meta_size_scale(t), sizeof(char));
    ASSERT_EQ(object_type_info_member_slots_of(t, obj), 0);
    ASSERT_EQ(object_type_info_member_eq_slots_of(t, obj), 0);

    object string_null_object = string_make_from_cstr_op(context, NULL);
    size1 = object_bootstrap_sizeof(string_null_object);
    size2 = context_object_sizeof(context, string_null_object);
    ASSERT_EQ(size1, size2);
    t = context_get_object_type(context, string_null_object);
    obj = string_null_object;
    ASSERT_EQ(object_bootstrap_sizeof(string_null_object), context_object_sizeof(context, string_null_object));
    ASSERT_EQ(type_info_member_base(t), 0);
    ASSERT_EQ(type_info_member_eq_len_base(t), 0);
    ASSERT_EQ(type_info_member_len_base(t), 0);
    ASSERT_EQ(type_info_member_meta_len_offset(t), 0);
    ASSERT_EQ(type_info_member_meta_len_scale(t), 0);
    ASSERT_EQ(type_info_size_base(t), object_sizeof_base(string));
    ASSERT_EQ(type_info_size_meta_size_offset(t), object_offsetof(string, len));
    ASSERT_EQ(type_info_size_meta_size_scale(t), sizeof(char));
    ASSERT_EQ(object_type_info_member_slots_of(t, obj), 0);
    ASSERT_EQ(object_type_info_member_eq_slots_of(t, obj), 0);

    object string_empty = string_make_from_cstr_op(context, "");
    size1 = object_bootstrap_sizeof(string_empty);
    size2 = context_object_sizeof(context, string_empty);
    ASSERT_EQ(size1, size2);
    t = context_get_object_type(context, string_empty);
    obj = string_empty;
    ASSERT_EQ(object_bootstrap_sizeof(string_empty), context_object_sizeof(context, string_empty));
    ASSERT_EQ(type_info_member_base(t), 0);
    ASSERT_EQ(type_info_member_eq_len_base(t), 0);
    ASSERT_EQ(type_info_member_len_base(t), 0);
    ASSERT_EQ(type_info_member_meta_len_offset(t), 0);
    ASSERT_EQ(type_info_member_meta_len_scale(t), 0);
    ASSERT_EQ(type_info_size_base(t), object_sizeof_base(string));
    ASSERT_EQ(type_info_size_meta_size_offset(t), object_offsetof(string, len));
    ASSERT_EQ(type_info_size_meta_size_scale(t), sizeof(char));
    ASSERT_EQ(object_type_info_member_slots_of(t, obj), 0);
    ASSERT_EQ(object_type_info_member_eq_slots_of(t, obj), 0);

    object symbol_obj = symbol_make_from_cstr_op(context, "this is a symbol object");
    size1 = object_bootstrap_sizeof(symbol_obj);
    size2 = context_object_sizeof(context, symbol_obj);
    ASSERT_EQ(size1, size2);
    t = context_get_object_type(context, symbol_obj);
    obj = symbol_obj;
    ASSERT_EQ(object_bootstrap_sizeof(symbol_obj), context_object_sizeof(context, symbol_obj));
    ASSERT_EQ(type_info_member_base(t), 0);
    ASSERT_EQ(type_info_member_eq_len_base(t), 0);
    ASSERT_EQ(type_info_member_len_base(t), 0);
    ASSERT_EQ(type_info_member_meta_len_offset(t), 0);
    ASSERT_EQ(type_info_member_meta_len_scale(t), 0);
    ASSERT_EQ(type_info_size_base(t), object_sizeof_base(symbol));
    ASSERT_EQ(type_info_size_meta_size_offset(t), object_offsetof(symbol, len));
    ASSERT_EQ(type_info_size_meta_size_scale(t), sizeof(char));
    ASSERT_EQ(object_type_info_member_slots_of(t, obj), 0);
    ASSERT_EQ(object_type_info_member_eq_slots_of(t, obj), 0);

    object vector10 = vector_make_op(context, 10);
    size1 = object_bootstrap_sizeof(vector10);
    size2 = context_object_sizeof(context, vector10);
    ASSERT_EQ(size1, size2);
    object_type_info_ptr type = context_get_object_type(context, vector10);
    ASSERT_EQ(object_type_info_member_slots_of(type, vector10), vector10->value.vector.len);
    ASSERT_EQ(object_type_info_member_eq_slots_of(type, vector10), vector10->value.vector.len);
    object *fp = type_info_get_object_ptr_of_first_member(type, vector10);
    for (size_t i = 0; i < object_type_info_member_slots_of(type, vector10); i++) {
        ASSERT_EQ(vector_ref(vector10, i), *fp);
    }
    t = context_get_object_type(context, vector10);
    obj = vector10;
    ASSERT_EQ(object_bootstrap_sizeof(vector10), context_object_sizeof(context, vector10));
    ASSERT_EQ(type_info_member_base(t), object_offsetof(vector, data));
    ASSERT_EQ(type_info_member_eq_len_base(t), 0);
    ASSERT_EQ(type_info_member_len_base(t), 0);
    ASSERT_EQ(type_info_member_meta_len_offset(t), object_offsetof(vector, len));
    ASSERT_EQ(type_info_member_meta_len_scale(t), 1);
    ASSERT_EQ(type_info_size_base(t), object_sizeof_base(vector));
    ASSERT_EQ(type_info_size_meta_size_offset(t), object_offsetof(vector, len));
    ASSERT_EQ(type_info_size_meta_size_scale(t), sizeof(object));
    ASSERT_EQ(object_type_info_member_slots_of(t, obj), vector_len(obj));
    ASSERT_EQ(object_type_info_member_eq_slots_of(t, obj), vector_len(obj));

    gc_collect_enable(context);
    int64_t start = utest_ns();
    gc_collect(context);
    int64_t time = utest_ns() - start;
    UTEST_PRINTF("gc time: %"
                         PRId64
                         " ns\n", time);
    ASSERT_TRUE(context->heap->first_node->data == context->heap->first_node->free_ptr);
    context_destroy(context);
}

#endif //BASE_SCHEME_RUNTIME_TEST_H
