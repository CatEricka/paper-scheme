#ifndef BASE_SCHEME_VALUE_TEST_H
#define BASE_SCHEME_VALUE_TEST_H
#pragma once

#include "paper-scheme/vm.h"

/**
 * 值类型测试
 * 测试基础类型是否工作正常
 */

UTEST(value_test, all_type_function_test) {
    context_t context = context_make(0x100, 2, 0x100000);
    object obj = NULL;

    object null_object = NULL;
    obj = null_object;
    ASSERT_TRUE(is_null(obj));
    ASSERT_FALSE(is_imm(obj));
    ASSERT_FALSE(is_unique_imm(obj));
    ASSERT_FALSE(is_i64_real(obj));
    ASSERT_FALSE(is_imm_i64(obj));
    ASSERT_FALSE(is_imm_char(obj));
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
    ASSERT_FALSE(is_imm_i64(obj));
    ASSERT_FALSE(is_imm_char(obj));
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
    ASSERT_FALSE(is_imm_i64(obj));
    ASSERT_FALSE(is_imm_char(obj));
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
    ASSERT_FALSE(is_imm_i64(obj));
    ASSERT_FALSE(is_imm_char(obj));
    ASSERT_FALSE(is_object(obj));
    ASSERT_FALSE(is_i64(obj));
    ASSERT_FALSE(is_doublenum(obj));
    ASSERT_FALSE(is_pair(obj));
    ASSERT_FALSE(is_string(obj));
    ASSERT_FALSE(is_symbol(obj));
    ASSERT_FALSE(is_vector(obj));

    object imm_char = char_imm_make('x');
    obj = imm_char;
    ASSERT_EQ('x', char_imm_getvalue(obj));
    ASSERT_FALSE(is_null(obj));
    ASSERT_TRUE(is_imm(obj));
    ASSERT_FALSE(is_unique_imm(obj));
    ASSERT_FALSE(is_i64_real(obj));
    ASSERT_FALSE(is_imm_i64(obj));
    ASSERT_TRUE(is_imm_char(obj));
    ASSERT_FALSE(is_object(obj));
    ASSERT_FALSE(is_i64(obj));
    ASSERT_FALSE(is_doublenum(obj));
    ASSERT_FALSE(is_pair(obj));
    ASSERT_FALSE(is_string(obj));
    ASSERT_FALSE(is_symbol(obj));
    ASSERT_FALSE(is_vector(obj));

    imm_char = char_imm_make('\0');
    obj = imm_char;
    ASSERT_EQ('\0', char_imm_getvalue(obj));
    ASSERT_FALSE(is_null(obj));
    ASSERT_TRUE(is_imm(obj));
    ASSERT_FALSE(is_unique_imm(obj));
    ASSERT_FALSE(is_i64_real(obj));
    ASSERT_FALSE(is_imm_i64(obj));
    ASSERT_TRUE(is_imm_char(obj));
    ASSERT_FALSE(is_object(obj));
    ASSERT_FALSE(is_i64(obj));
    ASSERT_FALSE(is_doublenum(obj));
    ASSERT_FALSE(is_pair(obj));
    ASSERT_FALSE(is_string(obj));
    ASSERT_FALSE(is_symbol(obj));
    ASSERT_FALSE(is_vector(obj));

    object imm_i64 = i64_imm_make(123);
    obj = imm_i64;
    ASSERT_EQ(123, i64_getvalue(obj));
    ASSERT_FALSE(is_null(obj));
    ASSERT_TRUE(is_imm(obj));
    ASSERT_FALSE(is_unique_imm(obj));
    ASSERT_FALSE(is_i64_real(obj));
    ASSERT_TRUE(is_imm_i64(obj));
    ASSERT_FALSE(is_imm_char(obj));
    ASSERT_FALSE(is_object(obj));
    ASSERT_TRUE(is_i64(obj));
    ASSERT_FALSE(is_doublenum(obj));
    ASSERT_FALSE(is_pair(obj));
    ASSERT_FALSE(is_string(obj));
    ASSERT_FALSE(is_symbol(obj));
    ASSERT_FALSE(is_vector(obj));

    object i64 = i64_make_real_object(context, 20);
    obj = i64;
    ASSERT_EQ(20, i64_getvalue(obj));
    ASSERT_FALSE(is_null(obj));
    ASSERT_FALSE(is_imm(obj));
    ASSERT_FALSE(is_unique_imm(obj));
    ASSERT_TRUE(is_i64_real(obj));
    ASSERT_FALSE(is_imm_i64(obj));
    ASSERT_FALSE(is_imm_char(obj));
    ASSERT_TRUE(is_object(obj));
    ASSERT_TRUE(is_i64(obj));
    ASSERT_FALSE(is_doublenum(obj));
    ASSERT_FALSE(is_pair(obj));
    ASSERT_FALSE(is_string(obj));
    ASSERT_FALSE(is_symbol(obj));
    ASSERT_FALSE(is_vector(obj));

    i64 = i64_make(context, INT64_MAX);
    obj = i64;
    ASSERT_EQ(INT64_MAX, i64_getvalue(obj));
    ASSERT_FALSE(is_null(obj));
    ASSERT_FALSE(is_imm(obj));
    ASSERT_FALSE(is_unique_imm(obj));
    ASSERT_TRUE(is_i64_real(obj));
    ASSERT_FALSE(is_imm_i64(obj));
    ASSERT_FALSE(is_imm_char(obj));
    ASSERT_TRUE(is_object(obj));
    ASSERT_TRUE(is_i64(obj));
    ASSERT_FALSE(is_doublenum(obj));
    ASSERT_FALSE(is_pair(obj));
    ASSERT_FALSE(is_string(obj));
    ASSERT_FALSE(is_symbol(obj));
    ASSERT_FALSE(is_vector(obj));

    i64 = i64_make_real_object(context, INT64_MIN);
    obj = i64;
    ASSERT_EQ(INT64_MIN, i64_getvalue(obj));
    ASSERT_FALSE(is_null(obj));
    ASSERT_FALSE(is_imm(obj));
    ASSERT_FALSE(is_unique_imm(obj));
    ASSERT_TRUE(is_i64_real(obj));
    ASSERT_FALSE(is_imm_i64(obj));
    ASSERT_FALSE(is_imm_char(obj));
    ASSERT_TRUE(is_object(obj));
    ASSERT_TRUE(is_i64(obj));
    ASSERT_FALSE(is_doublenum(obj));
    ASSERT_FALSE(is_pair(obj));
    ASSERT_FALSE(is_string(obj));
    ASSERT_FALSE(is_symbol(obj));
    ASSERT_FALSE(is_vector(obj));

    object doublenum = doublenum_make(context, 200.0);
    obj = doublenum;
    ASSERT_EQ(doublenum_getvalue(obj), 200.0);
    ASSERT_FALSE(is_null(obj));
    ASSERT_FALSE(is_imm(obj));
    ASSERT_FALSE(is_unique_imm(obj));
    ASSERT_FALSE(is_i64_real(obj));
    ASSERT_FALSE(is_imm_i64(obj));
    ASSERT_FALSE(is_imm_char(obj));
    ASSERT_TRUE(is_object(obj));
    ASSERT_FALSE(is_i64(obj));
    ASSERT_TRUE(is_doublenum(obj));
    ASSERT_FALSE(is_pair(obj));
    ASSERT_FALSE(is_string(obj));
    ASSERT_FALSE(is_symbol(obj));
    ASSERT_FALSE(is_vector(obj));

    object pair_carImmI64_cdrDouble = pair_make(context, imm_i64, doublenum);
    obj = pair_carImmI64_cdrDouble;
    ASSERT_EQ(pair_car(obj), imm_i64);
    ASSERT_EQ(i64_getvalue(pair_car(obj)), i64_getvalue(imm_i64));
    ASSERT_EQ(doublenum_getvalue(pair_cdr(obj)), doublenum_getvalue(doublenum));
    ASSERT_EQ(pair_cdr(obj), doublenum);
    ASSERT_FALSE(is_null(obj));
    ASSERT_FALSE(is_imm(obj));
    ASSERT_FALSE(is_unique_imm(obj));
    ASSERT_FALSE(is_i64_real(obj));
    ASSERT_FALSE(is_imm_i64(obj));
    ASSERT_FALSE(is_imm_char(obj));
    ASSERT_TRUE(is_object(obj));
    ASSERT_FALSE(is_i64(obj));
    ASSERT_FALSE(is_doublenum(obj));
    ASSERT_TRUE(is_pair(obj));
    ASSERT_FALSE(is_string(obj));
    ASSERT_FALSE(is_symbol(obj));
    ASSERT_FALSE(is_vector(obj));

    char str[] = "this is a string object";
    object string_obj = string_make_from_cstr(context, str);
    obj = string_obj;
    ASSERT_EQ(strlen(str), string_len(obj));
    for (size_t i = 0; i < strlen(str); i++) {
        ASSERT_EQ(str[i], string_index(obj, i));
    }
    for (size_t i = 0; i < strlen(str); i++) {
        ASSERT_EQ(str[i], string_get_cstr(obj)[i]);
    }
    ASSERT_EQ(string_index(obj, string_len(obj)), '\0');
    ASSERT_FALSE(is_null(obj));
    ASSERT_FALSE(is_imm(obj));
    ASSERT_FALSE(is_unique_imm(obj));
    ASSERT_FALSE(is_i64_real(obj));
    ASSERT_FALSE(is_imm_i64(obj));
    ASSERT_FALSE(is_imm_char(obj));
    ASSERT_TRUE(is_object(obj));
    ASSERT_FALSE(is_i64(obj));
    ASSERT_FALSE(is_doublenum(obj));
    ASSERT_FALSE(is_pair(obj));
    ASSERT_TRUE(is_string(obj));
    ASSERT_FALSE(is_symbol(obj));
    ASSERT_FALSE(is_vector(obj));

    object string_null_object = string_make_from_cstr(context, NULL);
    obj = string_null_object;
    ASSERT_EQ(0, string_len(obj));
    ASSERT_EQ('\0', string_index(obj, 0));
    ASSERT_FALSE(is_null(obj));
    ASSERT_FALSE(is_imm(obj));
    ASSERT_FALSE(is_unique_imm(obj));
    ASSERT_FALSE(is_i64_real(obj));
    ASSERT_FALSE(is_imm_i64(obj));
    ASSERT_FALSE(is_imm_char(obj));
    ASSERT_TRUE(is_object(obj));
    ASSERT_FALSE(is_i64(obj));
    ASSERT_FALSE(is_doublenum(obj));
    ASSERT_FALSE(is_pair(obj));
    ASSERT_TRUE(is_string(obj));
    ASSERT_FALSE(is_symbol(obj));
    ASSERT_FALSE(is_vector(obj));

    object string_empty = string_make_from_cstr(context, "");
    obj = string_empty;
    ASSERT_EQ(0, string_len(obj));
    ASSERT_EQ('\0', string_index(obj, 0));
    ASSERT_FALSE(is_null(obj));
    ASSERT_FALSE(is_imm(obj));
    ASSERT_FALSE(is_unique_imm(obj));
    ASSERT_FALSE(is_i64_real(obj));
    ASSERT_FALSE(is_imm_i64(obj));
    ASSERT_FALSE(is_imm_char(obj));
    ASSERT_TRUE(is_object(obj));
    ASSERT_FALSE(is_i64(obj));
    ASSERT_FALSE(is_doublenum(obj));
    ASSERT_FALSE(is_pair(obj));
    ASSERT_TRUE(is_string(obj));
    ASSERT_FALSE(is_symbol(obj));
    ASSERT_FALSE(is_vector(obj));

    char symbol_str[] = "this is a symbol object";
    object symbol_obj = symbol_make_from_cstr(context, symbol_str);
    obj = symbol_obj;
    ASSERT_EQ(strlen(str), symbol_len(obj));
    for (size_t i = 0; i <= strlen(symbol_str); i++) {
        ASSERT_EQ(symbol_str[i], symbol_index(obj, i));
    }
    for (size_t i = 0; i <= strlen(symbol_str); i++) {
        ASSERT_EQ(symbol_str[i], symbol_get_cstr(obj)[i]);
    }
    ASSERT_EQ(symbol_index(obj, symbol_len(obj)), '\0');
    ASSERT_FALSE(is_null(obj));
    ASSERT_FALSE(is_imm(obj));
    ASSERT_FALSE(is_unique_imm(obj));
    ASSERT_FALSE(is_i64_real(obj));
    ASSERT_FALSE(is_imm_i64(obj));
    ASSERT_FALSE(is_imm_char(obj));
    ASSERT_TRUE(is_object(obj));
    ASSERT_FALSE(is_i64(obj));
    ASSERT_FALSE(is_doublenum(obj));
    ASSERT_FALSE(is_pair(obj));
    ASSERT_FALSE(is_string(obj));
    ASSERT_TRUE(is_symbol(obj));
    ASSERT_FALSE(is_vector(obj));

    object symbol_null_object = symbol_make_from_cstr(context, NULL);
    obj = symbol_null_object;
    ASSERT_EQ(0, symbol_len(obj));
    ASSERT_EQ('\0', symbol_index(obj, 0));
    ASSERT_FALSE(is_null(obj));
    ASSERT_FALSE(is_imm(obj));
    ASSERT_FALSE(is_unique_imm(obj));
    ASSERT_FALSE(is_i64_real(obj));
    ASSERT_FALSE(is_imm_i64(obj));
    ASSERT_FALSE(is_imm_char(obj));
    ASSERT_TRUE(is_object(obj));
    ASSERT_FALSE(is_i64(obj));
    ASSERT_FALSE(is_doublenum(obj));
    ASSERT_FALSE(is_pair(obj));
    ASSERT_FALSE(is_string(obj));
    ASSERT_TRUE(is_symbol(obj));
    ASSERT_FALSE(is_vector(obj));

    object symbol_empty = symbol_make_from_cstr(context, "");
    obj = symbol_empty;
    ASSERT_EQ(0, symbol_len(obj));
    ASSERT_EQ('\0', symbol_index(obj, 0));
    ASSERT_FALSE(is_null(obj));
    ASSERT_FALSE(is_imm(obj));
    ASSERT_FALSE(is_unique_imm(obj));
    ASSERT_FALSE(is_i64_real(obj));
    ASSERT_FALSE(is_imm_i64(obj));
    ASSERT_FALSE(is_imm_char(obj));
    ASSERT_TRUE(is_object(obj));
    ASSERT_FALSE(is_i64(obj));
    ASSERT_FALSE(is_doublenum(obj));
    ASSERT_FALSE(is_pair(obj));
    ASSERT_FALSE(is_string(obj));
    ASSERT_TRUE(is_symbol(obj));
    ASSERT_FALSE(is_vector(obj));

    object vector10 = vector_make(context, 10);
    obj = vector10;
    for (size_t i = 0; i < vector_len(obj); i++) {
        ASSERT_EQ(IMM_UNIT, vector_ref(obj, i));
    }
    vector_ref(obj, 1) = imm_i64;
    ASSERT_EQ(vector_ref(obj, 1), imm_i64);
    ASSERT_EQ(10, vector_len(obj));
    ASSERT_FALSE(is_null(obj));
    ASSERT_FALSE(is_imm(obj));
    ASSERT_FALSE(is_unique_imm(obj));
    ASSERT_FALSE(is_i64_real(obj));
    ASSERT_FALSE(is_imm_i64(obj));
    ASSERT_FALSE(is_imm_char(obj));
    ASSERT_TRUE(is_object(obj));
    ASSERT_FALSE(is_i64(obj));
    ASSERT_FALSE(is_doublenum(obj));
    ASSERT_FALSE(is_pair(obj));
    ASSERT_FALSE(is_string(obj));
    ASSERT_FALSE(is_symbol(obj));
    ASSERT_TRUE(is_vector(obj));

    object vector20 = vector_make(context, 20);
    obj = vector20;
    for (size_t i = 0; i < vector_len(obj); i++) {
        ASSERT_EQ(IMM_UNIT, vector_ref(obj, i));
    }
    ASSERT_EQ(20, vector_len(obj));
    ASSERT_FALSE(is_null(obj));
    ASSERT_FALSE(is_imm(obj));
    ASSERT_FALSE(is_unique_imm(obj));
    ASSERT_FALSE(is_i64_real(obj));
    ASSERT_FALSE(is_imm_i64(obj));
    ASSERT_FALSE(is_imm_char(obj));
    ASSERT_TRUE(is_object(obj));
    ASSERT_FALSE(is_i64(obj));
    ASSERT_FALSE(is_doublenum(obj));
    ASSERT_FALSE(is_pair(obj));
    ASSERT_FALSE(is_string(obj));
    ASSERT_FALSE(is_symbol(obj));
    ASSERT_TRUE(is_vector(obj));

    object vector0 = vector_make(context, 0);
    obj = vector0;
    for (size_t i = 0; i < vector_len(obj); i++) {
        ASSERT_EQ(IMM_UNIT, vector_ref(obj, i));
    }
    ASSERT_EQ(0, vector_len(obj));
    ASSERT_FALSE(is_null(obj));
    ASSERT_FALSE(is_imm(obj));
    ASSERT_FALSE(is_unique_imm(obj));
    ASSERT_FALSE(is_i64_real(obj));
    ASSERT_FALSE(is_imm_i64(obj));
    ASSERT_FALSE(is_imm_char(obj));
    ASSERT_TRUE(is_object(obj));
    ASSERT_FALSE(is_i64(obj));
    ASSERT_FALSE(is_doublenum(obj));
    ASSERT_FALSE(is_pair(obj));
    ASSERT_FALSE(is_string(obj));
    ASSERT_FALSE(is_symbol(obj));
    ASSERT_TRUE(is_vector(obj));
}

#endif //BASE_SCHEME_VALUE_TEST_H
