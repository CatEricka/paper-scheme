#ifndef BASE_SCHEME_OBJECT_H
#define BASE_SCHEME_OBJECT_H
#pragma once


/**
 * object.h object.c
 * ����ṹ����
 */


#include "base-scheme/util.h"



/******************************************************************************
    �������ͱ��
******************************************************************************/
enum object_type_enum {
    FOREIGN_OBJECT = 0,
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
    �����ڴ沼��
******************************************************************************/
/**
 *  �ڴ���뵽 8 �ֽ�:
 *  1 << 3 = 8
 */
#define ALIGN_BITS (3u)
#define ALIGN_SIZE (8u)

/**
 * �ڴ��������
 * B0111 = 7
 */
#define ALIGN_MASK (7u)

/**
 * �ڴ������
 */
#define assert_aligned_ptr_check(x) assert(((((uintptr_t) (x)) & ((uintptr_t) ALIGN_MASK)) == 0u));
#define assert_aligned_size_check(x) assert(((((size_t) (x)) & ((uintptr_t) ALIGN_MASK)) == 0u));

struct object_struct_t;
typedef struct object_struct_t *object;
struct object_struct_t {
    /*  ����ͷ  */
    //��������
    uint8_t type;
    //gc״̬, 1Ϊ���
    uint32_t marked : 1;

    union object_value_u {
        /*  ��������  */

        //����64λ�з�������
        int64_t i64;

        //������
        double doublenum;

        //pair
        struct value_pair_t {
            object car;
            object cdr;
        } pair;


        /*  ����ʱ�ṹ  */

    } value;

    /*  �������, ���뵽 sizeof(void *)  */
};



/******************************************************************************
    ���ӽṹ����
******************************************************************************/




/******************************************************************************
    ������� API
******************************************************************************/

/**
 * ��������С
 * @param value_field object->value->value_field
 * @return �����С
 */
#define object_size(value_field)\
    (offsetof(struct object_struct_t, value) + sizeof(((object)0)->value.value_field))

/**
 * �����������Ĵ�С
 * �÷�: aligned_object_size(object_size(value_field))
 * ����һ����С, ���ض��뵽 1<<ALIGN_BITS �Ĵ�С
 * @param un_aligned_object_size
 * @return
 */
EXPORT_API size_t aligned_object_size(size_t un_aligned_object_size) {
    size_t align = un_aligned_object_size & (size_t) ALIGN_MASK;
    return un_aligned_object_size + (align == 0 ? 0 : 8);
}

/**
 * �������ͷ��С
 * @return ����ͷ��С
 */
#define object_sizeof_header() (object_size(doublenum) - sizeof(double))


/******************************************************************************
    malloc & free ��װ
******************************************************************************/
/**
 * malloc() �ķ�װ
 * @param size �ֽ���
 * @return ������ڴ��, Ϊ�������ʧ��
 */
EXPORT_API void *raw_alloc(size_t size) {
    void *obj = malloc(size);
    memset(obj, 0, size);
    // ��ַ���뵽 8bytes
    assert_aligned_ptr_check(obj);
    return obj;
}
/**
 * free() �ķ�װ
 * @param obj raw_alloc() ������ڴ�
 */
EXPORT_API void raw_free(void *obj) {
    assert_aligned_ptr_check(obj);
    free(obj);
}


/******************************************************************************
    ��������Ƕ���
******************************************************************************/
/**
 *   bits end in       1:  double number
 *                    00:  pointer
 *                  0 10:  string cursor (optional)
 *                 01 10:  immediate symbol (optional)
 *            0000 11 10:  immediate flonum (optional)
 *            0001 11 10:  char
 *            0010 11 10:  reader label (optional)
 *            0011 11 10:  unique immediate (NULL, TRUE, FALSE or else)
 */



/******************************************************************************
    ��ͨ���������
******************************************************************************/
/**
 * ָ������
 * 00
 */
#define POINTER_TAG (3u)



/******************************************************************************
    �������������
******************************************************************************/
/**
 * ��� 8 λ���ڳ������������
 */
#define UNIQUE_IMMEDIATE_EXTENDED_BITS (8u)
/**
 * ��������������
 */
#define UNIQUE_IMMEDIATE_TAG (62u)
/**
 * ��������������
 */
#define MAKE_UNIQUE_IMMEDIATE(n)  ((object) (((n)<<UNIQUE_IMMEDIATE_EXTENDED_BITS) \
                                          + UNIQUE_IMMEDIATE_TAG))



/******************************************************************************
    ��������������
******************************************************************************/
/**
 * ��������
 * false
 */
#define IMM_FALSE MAKE_UNIQUE_IMMEDIATE(0u)
/**
 * ��������
 * true
 */
#define IMM_TRUE MAKE_UNIQUE_IMMEDIATE(1u)
/**
 * ��������
 * nils
 */
#define IMM_NIL MAKE_UNIQUE_IMMEDIATE(2u)
// ������ڲ�����



/******************************************************************************
    ����������
******************************************************************************/
/**
 * ����Ƿ�Ϊָ��
 */
#define is_pointer(x) ((((uintptr_t)(x)) & POINTER_TAG) == 0)


#endif // _BASE_SCHEME_OBJECT_HEADER_