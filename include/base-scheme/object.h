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
    OBJ_PAIR,
    OBJ_STRING,
    OBJ_SYMBOL,
    OBJ_VECTOR,
    OBJ_PORT,
// ���ܳ��� UINT8_MAX
            OBJECT_TYPE_ENUM_MAX,
};
COMPILE_TIME_ASSERT(OBJECT_TYPE_ENUM_MAX <= UINT8_MAX);

// ����ͷħ��, uint8_t, B1010 1010
#define OBJECT_HEADER_MAGIC (0xAAu)
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
    // ��������
    // ����ͷħ��
    uint8_t magic;
    uint8_t type;
    // gc״̬, 1Ϊ���
    uint8_t marked: 1;
    // ����β����������С
    uint8_t padding_size;
    // �ƶ�������ת����ַ
    object forwarding;

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
        //�ַ���
        struct value_string_t {
            // char data[] ��С, ע����ָ char ����
            size_t len;
            char data[0];
        } string;
        //symbol
        struct value_symbol_t {
            // char data[] ��С, ע����ָ char ����
            size_t len;
            char data[0];
        } symbol;
        //����
        struct value_vector_t {
            // object data[0] ��С, ע����ָ object ����, �����ֽڳ���!
            size_t len;
            object data[0];
        } vector;

        /*  ����ʱ�ṹ  */
    } value;
    /*  �������, ���뵽 ALIGN_SIZE, �� sizeof(void *)  */
};



/**
                                ���ӽṹ����
******************************************************************************/


/**
                               ��������Ƕ���
    `*` ���Ϊ�Ѿ�ʵ��


     bits end in
             *         1:  i64 number
                      00:  pointer
             * 0000 0010:  ����
             * 0000 0110:  ����
             * 0000 1110:  ����
             * 0001 1110:  char, 30u
             * 0011 1110:  unique immediate (IMM_UNIT, IMM_TRUE, IMM_FALSE ������), 62u

    ���� object, �����ֿ�������:
        - ��ʵ�� object:     is_object(obj)
        - ������:            is_imm(obj)
        - NULL:             is_null(obj)    // ((void*)0)

    ����������:
        ���������췽��:
            - unique:           MAKE_UNIQUE_IMMEDIATE(), �÷�����Ӧ����ֱ��ʹ��
            - i64:              ��, �μ� i64_make_real_object() �� i64_make()
            - char:             char_imm_make()
        ������ȡֵ����:
            - unique:           ��, ֱ�ӱȽϼ���
            - i64:              ��, �μ� i64_getvalue()
            - char:             char_getvalue()

    ��ʵ�� object:
        �б�:
            - i64:              assert(is_i64(obj))
            - double number:    assert(is_doublenum(obj)
            - pair:             assert(is_pair(obj))
            - string:           assert(is_string(obj))
            - symbol:           assert(is_symbol(obj))
            - vector:           assert(is_vector(obj))
            - port: TODO port �ж� ��δʵ��
        ����:
            - i64:              i64_make()
            - double number:    doublenum_make()
            - pair:             pair_make()
            - string:           string_make_from_cstr()
            - symbol:           symbol_make_from_cstr()
            - vector:           vector_make()
            - port: TODO port ���� ��δʵ��
        ȡֵ:
            - i64:              i64_getvalue()
            - double number:    doublenum_getvalue()
            - pair:             TODO pair_getcar(), pair_getcdr()
            - string:           TODO string_get_cstr(), string_len(), string_index()
            - symbol:           TODO symbol_make_get_cstr(), symbol_len(), symbol_index()
            - vector:           TODO vector_make(), vector_len(), vector_ref()
            - port: TODO port ȡֵ ��δʵ��
******************************************************************************/


/**
                               ��ͨ���������
******************************************************************************/
#define USE_IMMEDIATE

// i64 ���������1λ����
#define I64_EXTENDED_BITS (1u)
// i64 ���������� B01
#define I64_IMM_MASK (1u)
// i64 ��������� B01
#define I64_IMM_TAG (1u)

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
#define POINTER_MASK (3u)
#define POINTER_TAG (0u)

// �޷��� char (8bits), ��������� 8 λ����
#define CHAR_EXTENDED_BITS (8u)
// char ���������� B1111 1111
#define CHAR_IMM_MASK (255u)
// char ��������� B0001 1110
#define CHAR_IMM_TAG (30u)
// char ֵ�� MASK
#define CHAR_VALUE_MASK (255u)



/**
                               �������������
******************************************************************************/
/**
 * ��� 8 λ���ڳ������������
 */
#define UNIQUE_IMMEDIATE_EXTENDED_BITS (8u)
/**
 * ����������������� B1111 1111
 */
#define UNIQUE_IMMEDIATE_MASK (255u)
/**
 * ������������� B0011 1110
 */
#define UNIQUE_IMMEDIATE_TAG (62u)
/**
 * ��������������
 */
#define MAKE_UNIQUE_IMMEDIATE(n)  ((object) (((n)<<UNIQUE_IMMEDIATE_EXTENDED_BITS) \
                                          | UNIQUE_IMMEDIATE_TAG))


// ����Ƿ�Ϊ����������, ע�� NULL ��������������
#define is_unique_imm(x) ((ptr_to_uintptr(x) & UNIQUE_IMMEDIATE_MASK) == UNIQUE_IMMEDIATE_TAG)

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
 * <li>Unit, �ȼ��� '()</li>
 * <li>�����ڲ��ṹ, Ӧ��ͳһʹ�� NULL; �� List Ӧ���� Unit ���������� NULL</li>
 */
#define IMM_UNIT MAKE_UNIQUE_IMMEDIATE(2u)
// ������ڲ�����


/**
                               ����������
******************************************************************************/

// ����Ƿ�Ϊ i64 ������
#define is_i64_imm(x) ((ptr_to_uintptr(x) & I64_IMM_MASK) == I64_IMM_TAG)
// ���� i64 ������
#define i64_imm_make(x) ((object) ((ptr_to_uintptr(x) << I64_EXTENDED_BITS) | I64_IMM_TAG))

// ����Ƿ�Ϊ����ָ��
// ����ζ��������ʽ����, �����Ǹ���Ч�� object ָ��
#define is_object(obj) ((!is_null(obj)) \
    && ((ptr_to_uintptr(obj) & POINTER_MASK) == POINTER_TAG) \
    && (((object) (obj))->magic == OBJECT_HEADER_MAGIC))

// ����Ƿ�Ϊ char ������, ��������Ϊ object
#define is_char_imm(x) ((ptr_to_uintptr(x) & CHAR_IMM_MASK) == CHAR_IMM_TAG)
// ���� char ����/������, ����Ϊ char, ע������
#define char_imm_make(x) ((object) (((ptr_to_uintptr(x) & CHAR_VALUE_MASK) << CHAR_EXTENDED_BITS) | CHAR_IMM_TAG))
// char ������ȡֵ, ��������Ϊ object, ����ֵΪ char
#define char_imm_getvalue(x) ((char) ((ptr_to_uintptr(x) >> CHAR_EXTENDED_BITS) & CHAR_VALUE_MASK))

// ����Ƿ�Ϊ������
#define is_imm(x) (!is_null(x) && ((ptr_to_uintptr(x) & POINTER_MASK) != POINTER_TAG))



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
 * �������Ĵ�С
 * �÷�: aligned_size(object_size(value_field))
 * ����һ����С, ���ض��뵽 ALIGN_SIZE �Ĵ�С
 * @param unaligned_size
 * @return
 */
EXPORT_API OUT size_t aligned_size(IN size_t unaligned_size);

/**
 * �������ͷ��С
 * @return ����ͷ��С
 */
#define object_sizeof_header() (offsetof(struct object_struct_t, value))

/**
 * ����ʱ��������С
 * @param object
 */
EXPORT_API OUT OUT size_t object_size_runtime(REF NOTNULL object obj);


/**
                               ����ֵ����
******************************************************************************/

/**
 * �ж� object �Ƿ�Ϊ i64, ������������, �μ� is_i64_real()
 * @param i64 NULLABLE
 * @return !0 -> true, 0 -> false
 */
EXPORT_API OUT int is_i64(REF NULLABLE object i64);
/**
 * �ж� object �Ƿ�Ϊ������ i64 ����, ������һ��������
 * @param i64
 * @return !0 -> true, 0 -> false
 */
#define is_i64_real(object) (is_object(object) && ((object)->type == OBJ_I64))
/**
 * �ж� object �Ƿ�Ϊ doublenum
 * @param object NULLABLE
 * @return !0 -> true, 0 -> false
 */
#define is_doublenum(object) (is_object(object) && ((object)->type == OBJ_D64))
/**
 * �ж� object �Ƿ�Ϊ pair
 * @param object NULLABLE
 * @return !0 -> true, 0 -> false
 */
#define is_pair(object) (is_object(object) && ((object)->type == OBJ_PAIR))
/**
 * �ж� object �Ƿ�Ϊ string
 * @param object NULLABLE
 * @return !0 -> true, 0 -> false
 */
#define is_string(object) (is_object(object) && ((object)->type == OBJ_STRING))
/**
 * �ж� object �Ƿ�Ϊ symbol
 * @param object NULLABLE
 * @return !0 -> true, 0 -> false
 */
#define is_symbol(object) (is_object(object) && ((object)->type == OBJ_SYMBOL))
/**
 * �ж� object �Ƿ�Ϊ vector
 * @param object NULLABLE
 * @return !0 -> true, 0 -> false
 */
#define is_vector(object) (is_object(object) && ((object)->type == OBJ_VECTOR))

/**
 * ��ȡ i64 �����ֵ
 * @param i64
 * @return
 */
EXPORT_API OUT int64_t i64_getvalue(REF NOTNULL object i64);


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


#endif // _BASE_SCHEME_OBJECT_HEADER_