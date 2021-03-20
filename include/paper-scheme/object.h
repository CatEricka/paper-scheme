#ifndef BASE_SCHEME_OBJECT_H
#define BASE_SCHEME_OBJECT_H
#pragma once


/**
 * object.h object.c
 * ����ṹ����
 * �������ݼ� test/test_cases/object_test.h test/test_cases/vm_test.h test/test_cases/value_test.h
 */


#include <paper-scheme/feature.h>


/**
                               �������ͱ��
******************************************************************************/
enum object_type_enum {
    OBJ_I64 = 0,
    OBJ_D64,
    OBJ_CHAR,
    OBJ_BOOLEAN,
    OBJ_UNIT,
    OBJ_PAIR,
    OBJ_STRING,
    OBJ_SYMBOL,
    OBJ_VECTOR,
//    OBJ_PORT,
            OBJECT_TYPE_ENUM_MAX, // ���ö�����ֵ
};
typedef enum object_type_enum object_type_tag;
compile_time_assert(((size_t) OBJECT_TYPE_ENUM_MAX) <= SIZE_MAX);

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
compile_time_assert(sizeof(uintptr_t) == 4u);
compile_time_assert(sizeof(void *) == 4u);
// 32λʱ, ָ����ڴ�����С���뵽 8�ֽ�, B0111 = 7
# define ALIGN_MASK (7u)
// ָ����ڴ�����С����� 3 bit ����Ϊ 0
# define ALIGN_BITS (3u)
// ���뵽 8u bytes
# define ALIGN_SIZE (8u)
#elif IS_64_BIT_ARCH
compile_time_assert(sizeof(uintptr_t) == 8u);
compile_time_assert(sizeof(void *) == 8u);
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


/**
                                ��������ṹ����
******************************************************************************/
struct object_struct_t;
typedef struct object_struct_t *object;


struct object_struct_t {
    /*  ����ͷ  */
    // ����ͷħ��
    uint8_t magic;
    // gc״̬, 1Ϊ���
    uint8_t marked: 1;
    // ����β����������С
    uint8_t padding_size;
    // ��������
    object_type_tag type;
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
        - ��ʵ�� object:
        - ������:
        - NULL:

    ���� NULL:
        �б�:
            - NULL:             is_null(obj)
    ����������:
        �б�:
            - IMM_UNIT:         is_imm_unit(obj)
            - IMM_TRUE:         is_imm_true(obj)
            - IMM_FALSE:        is_imm_false(obj)
            - all unit:         is_imm(obj)
            - i64:              is_imm_i64(obj), ���Ƽ�ֱ��ʹ��
            - char:             is_imm_char(obj)
        ���������췽��:
            - unique:           MAKE_UNIQUE_IMMEDIATE(), �÷�����Ӧ����ֱ��ʹ��
            - i64:              ��, �μ� i64_make_real_object_op() �� i64_make_op()
            - char:             char_imm_make()
        ������ȡֵ����:
            - unique:           ��, ֱ�ӱȽϼ���
            - i64:              ��, �μ� i64_getvalue()
            - char:             char_getvalue()

    ��ʵ�� object:
        �б�:
            - i64:              is_i64(obj)
            - double number:    is_doublenum(obj
            - pair:             is_pair(obj)
            - string:           is_string(obj)
            - symbol:           is_symbol(obj)
            - vector:           is_vector(obj)
            - port: TODO port �ж� ��δʵ��
        ����:
            - i64:              i64_make_op()
            - double number:    doublenum_make_op()
            - pair:             pair_make_op()
            - string:           string_make_from_cstr_op()
            - symbol:           symbol_make_from_cstr_op()
            - vector:           vector_make_op()
            - port: TODO port ���� ��δʵ��
        ȡֵ:
            - i64:              i64_getvalue()
            - double number:    doublenum_getvalue()
            - pair:             pair_getcar(), pair_getcdr()
            - string:           string_get_cstr(), string_len(), string_index()
            - symbol:           symbol_make_get_cstr(), symbol_len(), symbol_index()
            - vector:           vector_make_op(), vector_len(), vector_ref()
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
# define I64_IMM_MIN (-4611686018427387904)
// i64 ���������ֵ:   2^(63-1)-1
# define I64_IMM_MAX (4611686018427387903)
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
#define MAKE_UNIQUE_IMMEDIATE(n) \
    ((object) (((n)<<UNIQUE_IMMEDIATE_EXTENDED_BITS) | UNIQUE_IMMEDIATE_TAG))


/**
                               ��������������
******************************************************************************/
/**
 * �������� false
 */
#define IMM_FALSE MAKE_UNIQUE_IMMEDIATE(0u)
/**
 * �������� true
 */
#define IMM_TRUE MAKE_UNIQUE_IMMEDIATE(1u)
/**
 * �������� Unit, �ȼ��� '()
 * <p>�����ڲ��ṹ, Ӧ��ͳһʹ�� NULL; �� List Ӧ���� Unit ���������� NULL; �� vector Ӧ���� Unit ���</p>
 */
#define IMM_UNIT MAKE_UNIQUE_IMMEDIATE(2u)

// ������ڲ�����
// TODO ����������ڲ�����

/**
                               ����������
******************************************************************************/

/**
 * ���� i64 ������
 * @param object
 */
#define i64_imm_make(x) ((object) ((ptr_to_uintptr(x) << I64_EXTENDED_BITS) | I64_IMM_TAG))
/**
 * ���� char ����/������, ����Ϊ char, ע������
 * @param object
 */
#define char_imm_make(x) ((object) (((ptr_to_uintptr(x) & CHAR_VALUE_MASK) << CHAR_EXTENDED_BITS) | CHAR_IMM_TAG))



/**
                               ������� API
******************************************************************************/

/**
 * ��������С
 * @param value_field object->value->value_field
 * @return �����С
 */
#define object_sizeof_base(value_field)\
    (offsetof(struct object_struct_t, value) + sizeof(((object)0)->value.value_field))
/**
* �������ṹ���Ա����ƫ����
* @param value_field object->value->value_field
* @return �����С
*/
#define object_offsetof(_value, _value_field) \
    ((size_t)&(((object)0)->value._value._value_field))
/**
 * �������Ĵ�С
 * �÷�: aligned_size(object_sizeof_base(value_field))
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
 * <p>todo ���� context_t->global_type_table ����ǰʹ��</p>
 * <p>��� context.h: struct object_runtime_type_info_t, macro object_type_info_sizeof()</p>
 * @param object NOTNULL ���������������ָ��
 * @param object
 */
EXPORT_API OUT OUT size_t object_bootstrap_sizeof(REF NOTNULL object obj);


/**
                           ����ֵ����: is_a
******************************************************************************/
/**
 * ����Ƿ�Ϊ����������, ע�� NULL ��������������
 * @param object
 */
#define is_unique_imm(x) \
    ((ptr_to_uintptr(x) & UNIQUE_IMMEDIATE_MASK) == UNIQUE_IMMEDIATE_TAG)
/**
 * ����Ƿ�Ϊ i64 ������
 * @param object
 */
#define is_imm_i64(x) ((ptr_to_uintptr(x) & I64_IMM_MASK) == I64_IMM_TAG)
/**
 * ����Ƿ�Ϊ����ָ��
 * <p>����ζ��������ʽ����, �����Ǹ���Ч�� object ָ��</p>
 * @param object
 */
#define is_object(obj) ((!is_null(obj)) \
    && ((ptr_to_uintptr(obj) & POINTER_MASK) == POINTER_TAG) \
    && (((object) (obj))->magic == OBJECT_HEADER_MAGIC))
/**
 * ����Ƿ��������ձ�Ǵ��
 */
#define is_marked(obj) ((obj)->marked)
/**
 * ����Ƿ�Ϊ char ������, ��������Ϊ object
 * @param object
 */
#define is_imm_char(x) ((ptr_to_uintptr(x) & CHAR_IMM_MASK) == CHAR_IMM_TAG)
/**
 * ����Ƿ�Ϊ������
 * @param object
 */
#define is_imm(x) (!is_null(x) && ((ptr_to_uintptr(x) & POINTER_MASK) != POINTER_TAG))

// �Ƿ�Ϊ IMM_UNIT
#define is_imm_unit(obj) ((obj) == IMM_UNIT)
// �Ƿ�Ϊ IMM_TRUE
#define is_imm_true(obj) ((obj) == IMM_TRUE)
// �Ƿ�Ϊ IMM_FALSE
#define is_imm_false(obj) ((obj) == IMM_FALSE)
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
                           ����ֵ����: get value
******************************************************************************/
/**
 * char ������ȡֵ, ��������Ϊ object, ����ֵΪ char
 * @param object
 */
#define char_imm_getvalue(x) \
    ((char) ((ptr_to_uintptr(x) >> CHAR_EXTENDED_BITS) & CHAR_VALUE_MASK))
/**
 * ��ȡ i64 �����ֵ
 * @param i64: object
 * @return
 */
EXPORT_API OUT int64_t i64_getvalue(REF NOTNULL object i64);
/**
 * ��ȡ doublenum �����ֵ
 * @param object
 * @return
 */
#define doublenum_getvalue(obj) ((obj)->value.doublenum)
/**
 * ��ȡ pair ����� car
 * @param object
 * @return
 */
#define pair_car(obj) ((obj)->value.pair.car)
/**
 * ��ȡ pair ����� cdr
 * @param object
 * @return
 */
#define pair_cdr(obj) ((obj)->value.pair.cdr)

#define pair_caar(x)      (pair_car(pair_car(x)))
#define pair_cadr(x)      (pair_car(pair_cdr(x)))
#define pair_cdar(x)      (pair_cdr(pair_car(x)))
#define pair_cddr(x)      (pair_cdr(pair_cdr(x)))
#define pair_caaar(x)     (pair_car(pair_caar(x)))
#define pair_caadr(x)     (pair_car(pair_cadr(x)))
#define pair_cadar(x)     (pair_car(pair_cdar(x)))
#define pair_caddr(x)     (pair_car(pair_cddr(x)))
#define pair_cdaar(x)     (pair_cdr(pair_caar(x)))
#define pair_cdadr(x)     (pair_cdr(pair_cadr(x)))
#define pair_cddar(x)     (pair_cdr(pair_cdar(x)))
#define pair_cdddr(x)     (pair_cdr(pair_cddr(x)))
#define pair_cadddr(x)    (pair_cadr(pair_cddr(x)))
/**
 * ��ȡ string ����� cstr
 * @param object
 * @return
 */
#define string_get_cstr(obj) ((obj)->value.string.data)
/**
 * ��ȡ string ����� char ����, ע�ⲻ���� '\0'
 * @param object
 * @return
 */
#define string_len(obj) ((obj)->value.string.len - 1u)
/**
 * ʹ���������� string �� cstr �Ķ�Ӧ�ַ�, ��Χ [ 0, symbol_len(obj) )
 * @param object
 * @param i: ����ֵ
 * @return char ����
 */
#define string_index(obj, i) ((obj)->value.string.data[(i)])
/**
 * ��ȡ symbol ����� cstr
 * @param object
 * @return
 */
#define symbol_get_cstr(obj) ((obj)->value.symbol.data)
/**
 * ��ȡ symbol ����� char ����, ע�ⲻ���� '\0'
 * @param object
 * @return
 */
#define symbol_len(obj) ((obj)->value.symbol.len - 1u)
/**
 * ʹ���������� symbol �� cstr �Ķ�Ӧ�ַ�, ��Χ [0, len)
 * @param object
 * @param i: ����ֵ
 * @return ����
 */
#define symbol_index(obj, i) ((obj)->value.symbol.data[(i)])
/**
 * ��ȡ vector ���������
 * @param object
 * @return
 */
#define vector_len(obj) ((obj)->value.vector.len)
/**
 * ʹ���������� vector ������, ��Χ [0, len)
 * @param object
 * @param i: ����ֵ
 * @return object ����
 */
#define vector_ref(obj, i) ((obj)->value.vector.data[(i)])
/**
 * ʹ�������޸� vector ������, ��Χ [0, len)
 * @param object
 * @param i: ����ֵ
 * @return ��Ҫʹ�÷���ֵ
 */
#define vector_set(obj, i, v) ((obj)->value.vector.data[(i)] = (v))

/**
                           ����ֵ����: compare
******************************************************************************/
// TODO ʵ�ֻ�������Ƚ�


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
 * realloc() �ķ�װ
 * @param obj raw_alloc() ������ڴ�
 */
EXPORT_API void *raw_realloc(IN NOTNULL void *obj, size_t new_size);


#endif // _BASE_SCHEME_OBJECT_HEADER_