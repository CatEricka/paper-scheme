#ifndef _BASE_SCHEME_OBJECT_HEADER_
#define _BASE_SCHEME_OBJECT_HEADER_
#pragma once

#include <stdint.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <stddef.h>

/** ��������ṹ
*/
struct object_struct_t
{
    //������
    uint32_t object_tag;

    //gc���
    uint32_t gc_tag;

    //������
    union value_t {
        //������
        double doublenum;

        //����32λ�з�������
        int32_t i32;

        //����64λ�з�������
        int64_t i64;
    } value;
};

typedef struct object_struct_t *object;

/** ������� API
*/
// ��������С
#define object_size(value_field)\
    (offsetof(struct object_struct_t, value) + sizeof(((object)0)->value.value_field))

// �������ͷ��С
#define object_sizeof_header() (object_size(doublenum) - sizeof(double))

#endif // _BASE_SCHEME_OBJECT_HEADER_