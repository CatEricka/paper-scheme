#ifndef _BASE_SCHEME_OBJECT_HEADER_
#define _BASE_SCHEME_OBJECT_HEADER_
#pragma once

#include <stdint.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <stddef.h>


#include "base-scheme/util.h"


 /******************************************************************************
     ������
 ******************************************************************************/
 /* ���ָ��ϵͳ
  *   bits end in     1:  fixnum
  *                  00:  pointer
  *                 010:  string cursor (optional)
  *                0110:  immediate symbol (optional)
  *            00001110:  immediate flonum (optional)
  *            00011110:  char
  *            00101110:  reader label (optional)
  *            00111110:  unique immediate (NULL, TRUE, FALSE)
  */




 /******************************************************************************
     �������ͱ��
 ******************************************************************************/
enum object_type_enum {
    FOREIGN_OBJECT,
    OBJ_I64,
    OBJ_D64,
    OBJ_BOOLEAN,
    OBJ_PAIR,
    OBJ_STRING,
    OBJ_PORT,
// ���ܳ��� UINT8_MAX
    OBJECT_TYPE_ENUM_MAX,
};
COMPILE_TIME_ASSERT(OBJECT_TYPE_ENUM_MAX <= UINT8_MAX);


/******************************************************************************
    ��������ṹ
******************************************************************************/
struct object_struct_t;
typedef struct object_struct_t* object;
struct object_struct_t
{
    /*  header  */
    //��������
    uint8_t type;
    //gc״̬, 1Ϊ���
    uint32_t marked : 1;

    /*  body  */
    union {
        //����64λ�з�������
        int64_t i64;

        //������
        double doublenum;

        //pair
        struct {
            object car;
            object cdr;
        } pair;
    } value;
};




/******************************************************************************
    ������� API
******************************************************************************/
// ��������С
#define object_size(value_field)\
    (offsetof(struct object_struct_t, value) + sizeof(((object)0)->value.value_field))

// �������ͷ��С
#define object_sizeof_header() (object_size(doublenum) - sizeof(double))



#endif // _BASE_SCHEME_OBJECT_HEADER_