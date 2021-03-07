#ifndef _BASE_SCHEME_OBJECT_HEADER_
#define _BASE_SCHEME_OBJECT_HEADER_
#pragma once

#include <stdint.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <stddef.h>

/** 基本对象结构
*/
struct object_struct_t
{
    //对象标记
    uint32_t object_tag;

    //gc标记
    uint32_t gc_tag;

    //对象体
    union value_t {
        //浮点数
        double doublenum;

        //定点32位有符号整数
        int32_t i32;

        //定点64位有符号整数
        int64_t i64;
    } value;
};

typedef struct object_struct_t *object;

/** 对象操作 API
*/
// 计算对象大小
#define object_size(value_field)\
    (offsetof(struct object_struct_t, value) + sizeof(((object)0)->value.value_field))

// 计算对象头大小
#define object_sizeof_header() (object_size(doublenum) - sizeof(double))

#endif // _BASE_SCHEME_OBJECT_HEADER_