#ifndef BASE_SCHEME_OBJECT_H
#define BASE_SCHEME_OBJECT_H
#pragma once


/**
 * object.h object.c
 * ����ṹ����
 */


#include "base-scheme/util.h"


/**
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


/**
    �����ڴ沼��
******************************************************************************/

/**
 * �ڴ��������
 * B0111 = 7
 * ���뵽 8 �ֽ�������
 */
#ifdef IS_32_BIT_ARCH
COMPILE_TIME_ASSERT(sizeof(uintptr_t) == 4u);
COMPILE_TIME_ASSERT(sizeof(void *) == 4u);
// 32λʱ, ָ����ڴ�����С���뵽 8�ֽ�, B0111 = 7
# define ALIGN_MASK (7u)
// ָ����ڴ�����С����� 3 bit ����Ϊ 0
# define ALIGN_BITS (3u)
// ���뵽 8u bytes
# define ALIGN_SIZE (8u)
#elif IS_64_BIT_ARCH
COMPILE_TIME_ASSERT(sizeof(uintptr_t) == 8u);
COMPILE_TIME_ASSERT(sizeof(void *) == 8u);
// 64λʱ, ָ����ڴ�����С���뵽 8�ֽ�, B0111 = 7
# define ALIGN_MASK (7u)
// ָ����ڴ�����С����� 3 bit ����Ϊ 0
# define ALIGN_BITS (3u)
// ���뵽 8u bytes ��������
# define ALIGN_SIZE (8u)
#else
# error("Unknown arch")
#endif

/**
 * �ڴ������
 */
#define assert_aligned_ptr_check(x) assert(((((uintptr_t) (x)) & ((uintptr_t) ALIGN_MASK)) == 0u))
#define assert_aligned_size_check(x) assert(((((size_t) (x)) & ((uintptr_t) ALIGN_MASK)) == 0u))

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
    /*  �������, ���뵽 ALIGN_SIZE, �� sizeof(void *)  */
};



/**
    ���ӽṹ����
******************************************************************************/




/**
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
EXPORT_API OUT size_t aligned_object_size(IN size_t un_aligned_object_size);

/**
 * �������ͷ��С
 * @return ����ͷ��С
 */
#define object_sizeof_header() (object_size(doublenum) - sizeof(double))


/**
    malloc & free ��װ
******************************************************************************/

/**
 * malloc() �ķ�װ
 * @param size �ֽ���
 * @return ������ڴ��, Ϊ�������ʧ��
 */
EXPORT_API OUT NULLABLE void *raw_alloc(IN size_t size);

/**
 * free() �ķ�װ
 * @param obj raw_alloc() ������ڴ�
 */
EXPORT_API void raw_free(IN NOTNULL void *obj);


/**
    ��������Ƕ���
    `*` ���Ϊ�Ѿ�ʵ��
******************************************************************************/
/**
 *   bits end in
 *           *         1:  i64 number
 *                    00:  pointer
 *   imm:
 *                  0 10:  string cursor (optional)
 *                 01 10:  immediate symbol (optional)
 *            0000 11 10:  immediate flonum (optional)
 *           *0001 11 10:  char, 30u
 *            0010 11 10:  reader label (optional)
 *           *0011 11 10:  unique immediate (NULL, TRUE, FALSE or else), 62u
 */


/**
    ��ͨ���������
******************************************************************************/
#define USE_IMMEDIATE

// i64 ���������1λ����
#define I64_EXTENDED_BITS (1u)
// i64 ���������� B01
#define I64_TAG_MASK (1u)
// i64 ��������� B01
#define I64_TAG (1u)
#ifdef IS_64_BIT_ARCH
// i64 ��������Сֵ: - 2^(63-1)
# define I64_IMM_MIN (-4611686018427387904i64)
// i64 ���������ֵ:   2^(63-1)-1
# define I64_IMM_MAX (4611686018427387903i64)
#elif IS_32_BIT_ARCH
// i64 ��������Сֵ: - 2^(31-1)
# define I64_IMM_MIN (-1073741824i32)
// i64 ���������ֵ:   2^(31-1)-1
# define I64_IMM_MAX (1073741823i32)
#else
# error("Unknown arch")
#endif

// ָ������ B0011
#define POINTER_TAG_MASK (3u)

// �޷��� char (8bits), ��������� 8 λ����
#define CHAR_EXTENDED_BITS (8u)
// char ���������� B0001 1110
#define CHAR_TAG_MASK (30u)
// char ��������� B0001 1110
#define CHAR_TAG (30u)
// char ������ȡֵMASK
#define CHAR_VALUE_MASK (255u)



/**
    �������������
******************************************************************************/
/**
 * ��� 8 λ���ڳ������������
 */
#define UNIQUE_IMMEDIATE_EXTENDED_BITS (8u)
/**
 * �����������������
 */
#define UNIQUE_IMMEDIATE_MASK (62u)
/**
 * ��������������
 */
#define MAKE_UNIQUE_IMMEDIATE(n)  ((object) (((n)<<UNIQUE_IMMEDIATE_EXTENDED_BITS) \
                                          + UNIQUE_IMMEDIATE_MASK))


/**
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


/**
    ����������
******************************************************************************/

// ����Ƿ�Ϊ i64 ������
#define is_i64_imm(x) ((ptr_to_uintptr(x) & I64_TAG_MASK) != 0)
// ���� i64 ������
#define i64_imm_make(x) ((object) ((ptr_to_uintptr(x) << I64_EXTENDED_BITS) | I64_TAG))

// ����Ƿ�Ϊָ��
#define is_pointer(x) ((ptr_to_uintptr(x) & POINTER_TAG_MASK) == 0)

// ����Ƿ�Ϊ char ������, ��������Ϊ object
#define is_char_imm(x) ((ptr_to_uintptr(x) & CHAR_TAG_MASK) != 0)
// ���� char ����/������, ����Ϊ char, ע������
#define char_imm_make(x) ((object) (((ptr_to_uintptr(x) & CHAR_VALUE_MASK) << CHAR_EXTENDED_BITS) | CHAR_TAG))
// char ������ȡֵ, ��������Ϊ object, ����ֵΪ char
#define char_imm_getvalue(x) ((char) ((ptr_to_uintptr(x) >> CHAR_EXTENDED_BITS) & CHAR_VALUE_MASK))

// ����Ƿ�Ϊ������
#define is_imm(x) (!is_pointer((x)))


/**
    ����ֵ����
******************************************************************************/

/**
 * ��ȡ i64 �����ֵ
 * @param i64
 * @return
 */
EXPORT_API OUT int64_t i64_getvalue(REF NOTNULL object i64);

/**
 * �ж� object �Ƿ�Ϊ i64
 * @param i64
 * @return !0 -> true, 0 -> false
 */
EXPORT_API OUT int is_i64(REF NOTNULL object i64);


#endif // _BASE_SCHEME_OBJECT_HEADER_