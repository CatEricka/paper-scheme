#ifndef _BASE_SCHEME_OBJECT_HEADER_
#define _BASE_SCHEME_OBJECT_HEADER_
#pragma once

#include <stdint.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <stddef.h>


#include "base-scheme/util.h"


 /******************************************************************************
     立即数
 ******************************************************************************/
 /* 标记指针系统
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
     对象类型标记
 ******************************************************************************/
enum object_type_enum {
    FOREIGN_OBJECT,
    OBJ_I64,
    OBJ_D64,
    OBJ_BOOLEAN,
    OBJ_PAIR,
    OBJ_STRING,
    OBJ_PORT,
// 不能超过 UINT8_MAX
    OBJECT_TYPE_ENUM_MAX,
};
COMPILE_TIME_ASSERT(OBJECT_TYPE_ENUM_MAX <= UINT8_MAX);


/******************************************************************************
    基本对象结构
******************************************************************************/
struct object_struct_t;
typedef struct object_struct_t* object;
struct object_struct_t
{
    /*  header  */
    //对象类型
    uint8_t type;
    //gc状态, 1为存活
    uint32_t marked : 1;

    /*  body  */
    union {
        //定点64位有符号整数
        int64_t i64;

        //浮点数
        double doublenum;

        //pair
        struct {
            object car;
            object cdr;
        } pair;
    } value;
};




/******************************************************************************
    对象操作 API
******************************************************************************/
// 计算对象大小
#define object_size(value_field)\
    (offsetof(struct object_struct_t, value) + sizeof(((object)0)->value.value_field))

// 计算对象头大小
#define object_sizeof_header() (object_size(doublenum) - sizeof(double))



#endif // _BASE_SCHEME_OBJECT_HEADER_