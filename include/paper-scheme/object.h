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
// todo  ������������д object_type_enum ö��
enum object_type_enum {
    OBJ_I64 = 0,
    OBJ_D64,
    OBJ_CHAR,
    OBJ_BOOLEAN,
    OBJ_UNIT,
    OBJ_EOF,
    OBJ_PAIR,
    OBJ_BYTES,
    OBJ_STRING,
    OBJ_STRING_BUFFER,
    OBJ_SYMBOL,
    OBJ_VECTOR,
    OBJ_STACK,
    OBJ_STRING_PORT,
    OBJ_STDIO_PORT,
    OBJ_HASH_SET,
    OBJ_HASH_MAP,
    OBJ_WEAK_REF,

    OBJECT_TYPE_ENUM_MAX, // ���ö�����ֵ
};
typedef enum object_type_enum object_type_tag;
//compile_time_assert(((size_t) OBJECT_TYPE_ENUM_MAX) <= SIZE_MAX);

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
compile_time_assert(sizeof(void *) == sizeof(uint32_t));
compile_time_assert(sizeof(double) == 8u);
compile_time_assert(sizeof(double) == sizeof(uint64_t));
// 32λʱ, ָ����ڴ�����С���뵽 8�ֽ�, B0111 = 7
# define ALIGN_MASK (7u)
// ָ����ڴ�����С����� 3 bit ����Ϊ 0
# define ALIGN_BITS (3u)
// ���뵽 8u bytes
# define ALIGN_SIZE (8u)
#elif IS_64_BIT_ARCH
compile_time_assert(sizeof(uintptr_t) == 8u);
compile_time_assert(sizeof(void *) == 8u);
compile_time_assert(sizeof(void *) == sizeof(uint64_t));
compile_time_assert(sizeof(double) == 8u);
compile_time_assert(sizeof(double) == sizeof(uint64_t));
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
// port ����
enum port_kind {
    // �����ڲ����� https://srfi.schemers.org/srfi-6/srfi-6.html
            PORT_SRFI6 = 1,
    // ���� port
            PORT_INPUT = 2,
    // ��� port
            PORT_OUTPUT = 4,
    // port �����β
            PORT_EOF = 8,
};

struct object_struct_t;
typedef struct object_struct_t *object;


struct object_struct_t {
    /*  ����ͷ  */
    // ����ͷħ��
    uint8_t magic;
    // gc״̬, 1Ϊ���
    uint8_t marked: 1;
    // �Ƿ�Ϊ���ɱ����
    uint8_t immutable: 1;
    // ����β����������С
    uint8_t padding_size;
    // ��������
    object_type_tag type;
    // �ƶ�������ת����ַ
    object forwarding;

    union object_value_u {
        /*  ��������  */
        //����64λ�з�������, ���ɱ�
        int64_t i64;

        //������, ���ɱ�
        double doublenum;

        //pair, �ɱ�
        struct value_pair_t {
            object car;
            object cdr;
        } pair;

        // bytes, �ɱ�
        struct value_bytes_t {
            // ���� hash ����, �̶�����
            uint32_t hash;
            size_t capacity;
            char data[0];
        } bytes;

        //�ַ���, ���ɱ�
        struct value_string_t {
            // ���� hash ����, �̶�����
            uint32_t hash;
            // char data[] ��С, ע����ָ char ����, ���� '\0'
            size_t len;
            char data[0];
        } string;

        // �ַ�������, �ɱ�
        struct value_string_buffer_t {
            // ���� hash ����, �̶�����
            uint32_t hash;
            // �����ܳ���
            size_t buffer_size;
            // ��ǰ����, ���� '\0'
            size_t buffer_length;
            object bytes_buffer;
        } string_buffer;

        //symbol, ���ɱ�
        struct value_symbol_t {
            // ���� hash ����, �̶�����
            uint32_t hash;
            // char data[] ��С, ע����ָ char ����
            size_t len;
            char data[0];
        } symbol;

        //����, �ɱ�
        struct value_vector_t {
            // ���� hash ����, �̶�����
            uint32_t hash;
            // object data[0] ��С, ע����ָ object ����, �����ֽڳ���!
            size_t len;
            object data[0];
        } vector;

        //ջ, �ɱ�
        struct value_stack_t {
            // ���� hash ����, �̶�����
            uint32_t hash;
            // ջ��С
            size_t size;
            // ջ�� object ����, ��ջΪ 0
            size_t length;
            object data[0];
        } stack;

        // �ַ��������������, �ɱ�
        struct value_string_port_t {
            // ���� hash ����, �����̶�����
            uint32_t hash;
            enum port_kind kind;
            object string_buffer_data;
            size_t length;
            size_t current;
        } string_port;

        // stdio �����������, �ɱ�
        struct value_stdio_port_t {
            // ���� hash ����, �̶�����
            uint32_t hash;
            enum port_kind kind;
            FILE *file;
            object filename;
            // �Ƿ��Ѿ��ͷ�
            int is_released;
            // �Ƿ���Ҫ�ر�, stdin, stdout, stderr ����Ҫ�ر�
            int need_close;
        } stdio_port;

        // ʵ�� hash set, �ɱ�
        // ���ֻ����ԭ������, �޷���֤����������������
        struct value_hashset_t {
            // ���� hash ����, �̶�����
            uint32_t hash;
            // hashmap ʵ��
            object map;
        } hashset;

        // hash map, �ɱ�
        // key ���Ϊԭ������
        struct value_hashmap_t {
            // ���� hash ����, �̶�����
            uint32_t hash;
            // ��ǰ hashmap ��ֵ������
            size_t size;
            double load_factor;
            // ��������
            size_t threshold;
            // #( ((k v) '())
            //    ((k v) '())
            //    ((k v) '()))
            // vector
            object table;
        } hashmap;

        /*  ����ʱ�ṹ  */
        // ������, �ɱ�
        // ע��, �������޷�������Ƿ�����, ����������������������ǲ��ɿ���
        struct value_weak_ref_t {
            // ���� hash ����, �̶�����
            uint32_t hash;
            // GC ʱʹ�õ��ڲ��ṹ
            object _internal_next_ref;
            // ������
            object ref;
        } weak_ref;

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
            - IMM_EOF:          is_imm_eof(obj)
            - all unit:         is_imm(obj)
            - i64:              is_imm_i64(obj), ���Ƽ�ֱ��ʹ��
            - char:             is_imm_char(obj)
        ���������췽��:
            - unique:           make_unique_immediate(), �÷�����Ӧ����ֱ��ʹ��
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
            - bytes:            is_bytes(obj)
            - string:           is_string(obj)
            - string_buffer:    is_string_buffer(obj)
            - symbol:           is_symbol(obj)
            - vector:           is_vector(obj)
            - stack:            is_stack(obj)
            - string_port:      TODO is_string_port(obj), is_string_port_input(), is_string_port_output(),
                                TODO is_string_port_in_out_put(obj), is_string_port_eof(obj)
            - stdio_port:       TODO is_stdio_port(obj), is_stdio_port_input(obj), is_stdio_port_output(obj),
                                TODO is_stdio_port_in_out_put(obj), is_stdio_port_eof(obj)
            - string_port & stdio_port:
                                TODO is_port_input(obj), is_port_output(obj), is_port_in_out_put(obj), is_port_eof(obj)
            - srfi6 string_port:
                                TODO is_srfi6_port(obj)
            - hashset:          is_hashset(obj)
            - hashmap:          is_hashmap(obj)
            - weak_ref:         is_weak_ref(obj)
        ����:
            - i64:              i64_make_op()
            - double number:    doublenum_make_op()
            - pair:             pair_make_op()
            - bytes:            bytes_make_op()
            - string:           string_make_from_cstr_op()
            - string_buffer:    string_buffer_make_op(), string_buffer_make_from_string_op()
            - symbol:           symbol_make_from_cstr_op()
            - vector:           vector_make_op()
            - stack:            stack_make_op()
            - string_port:      TODO string_port_input_from_string(), string_port_output_use_buffer(),
                                TODO string_port_in_out_put_from_string_use_buffer()
            - stdio_port:       TODO stdio_port_from_filename(), stdio_port_from_file()
            - hashset:          TODO hashset_make_op()
            - hashmap:          TODO hashmap_make_op()
            - weak_ref:         TODO weak_ref_make_op()
        ȡֵ:
            - i64:              i64_getvalue()
            - double number:    doublenum_getvalue()
            - pair:             pair_getcar(), pair_getcdr()
            - bytes:            bytes_capacity(), bytes_index(), bytes_data()
            - string:           string_get_cstr(), string_len(), string_index()
            - string_buffer:    string_buffer_empty(), string_buffer_full(),
                                string_buffer_length(), string_buffer_index(), string_buffer_bytes_data(),
                                string_buffer_capacity(), string_buffer_bytes_obj
            - symbol:           symbol_make_get_cstr(), symbol_len(), symbol_index()
            - vector:           vector_len(), vector_ref()
            - stack:            stack_clean(), stack_capacity(), stack_len()
                                stack_full(), stack_empty()
                                stack_push, stack_pop(),  stack_peek(),
            - string_port:      TODO string_port_kind()
            - stdio_port:       TODO stdio_port_kind()
            - hashset:          hashset_size()
            - hashmap:          hashmap_size()
            - weak_ref:         weak_ref_is_valid()
        ����:
            - string:           string_append_op()
            - string_buffer:    string_buffer_append_string_op(), string_buffer_append_imm_char_op(),
                                string_buffer_append_char_op()
            - hashset:          TODO hashset_contains_op(), hashset_put_op(), hashset_put_all_op()
                                TODO hashset_clear_op(), hashset_remove_op()
            - hashmap:          TODO hashmap_contains_key_op(), hashmap_put_op(), hashmap_get_op()
                                TODO hashmap_put_all_op(), hashmap_clear_op(), hashmap_remove_op()
            - weak_ref:         weak_ref_get()
        ����:
            - bytes:            bytes_capacity_increase()
            - string_buffer:    string_buffer_capacity_increase()
            - vector:           vector_capacity_increase()
            - stack:            stack_capacity_increase(),  stack_push_auto_increase()
        ����ת��:
            - char (������):     imm_char_to_string()
            - char (C ԭʼ����): char_to_string()
            - symbol:           TODO symbol_to_string()
            - string:           TODO string_to_symbol()
            - string_buffer:    string_buffer_to_string(), TODO string_buffer_to_symbol()
        ��ϣ�㷨 & equals �㷨:
            - i64:
            - double number:
            - pair:
            - bytes:
            - string:           string_hash_code(), string_equals()
            - string_buffer:
            - symbol:           symbol_hash_code(), symbol_equals()
            - vector:
            - stack:
            - string_port:
            - stdio_port:
            - string_port & stdio_port:
            - srfi6 string_port:
            - hashset:
            - hashmap:
            - weak_ref:
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
#define make_unique_immediate(n) \
    ((object) (((n)<<UNIQUE_IMMEDIATE_EXTENDED_BITS) | UNIQUE_IMMEDIATE_TAG))


/**
                               ��������������
******************************************************************************/
/**
 * �������� false
 */
#define IMM_FALSE make_unique_immediate(0u)
/**
 * �������� true
 */
#define IMM_TRUE make_unique_immediate(1u)
/**
 * �������� Unit, �ȼ��� '()
 * <p>�����ڲ��ṹ, Ӧ��ͳһʹ�� NULL; �� List Ӧ���� Unit ���������� NULL; �� vector Ӧ���� Unit ���</p>
 */
#define IMM_UNIT make_unique_immediate(2u)
/**
 * EOF ����
 */
#define IMM_EOF make_unique_immediate(3u)

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
#define is_imm_i64(x)                   ((ptr_to_uintptr(x) & I64_IMM_MASK) == I64_IMM_TAG)
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
#define is_marked(obj)                  ((obj)->marked)
/**
 * ����Ƿ�Ϊ char ������, ��������Ϊ object
 * @param object
 */
#define is_imm_char(x)                  ((ptr_to_uintptr(x) & CHAR_IMM_MASK) == CHAR_IMM_TAG)
/**
 * ����Ƿ�Ϊ������
 * @param object
 */
#define is_imm(x)                       (!is_null(x) && ((ptr_to_uintptr(x) & POINTER_MASK) != POINTER_TAG))

// �Ƿ�Ϊ IMM_UNIT
#define is_imm_unit(obj)                ((obj) == IMM_UNIT)
// �Ƿ�Ϊ IMM_TRUE
#define is_imm_true(obj)                ((obj) == IMM_TRUE)
// �Ƿ�Ϊ IMM_FALSE
#define is_imm_false(obj)               ((obj) == IMM_FALSE)
// �Ƿ�Ϊ IMM_EOF
#define is_imm_eof(obj)                 ((obj) == IMM_EOF)
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
#define is_i64_real(object)             (is_object(object) && ((object)->type == OBJ_I64))
/**
 * �ж� object �Ƿ�Ϊ doublenum
 * @param object NULLABLE
 * @return !0 -> true, 0 -> false
 */
#define is_doublenum(object)            (is_object(object) && ((object)->type == OBJ_D64))
/**
 * �ж� object �Ƿ�Ϊ pair
 * @param object NULLABLE
 * @return !0 -> true, 0 -> false
 */
#define is_pair(object)                 (is_object(object) && ((object)->type == OBJ_PAIR))
// bytes
#define is_bytes(obj)                   (is_object(obj) && ((obj)->type == OBJ_BYTES))
/**
 * �ж� object �Ƿ�Ϊ string
 * @param object NULLABLE
 * @return !0 -> true, 0 -> false
 */
#define is_string(object)               (is_object(object) && ((object)->type == OBJ_STRING))
// string buffer
#define is_string_buffer(obj)           (is_object(obj) && ((obj)->type == OBJ_STRING_BUFFER))
/**
 * �ж� object �Ƿ�Ϊ symbol
 * @param object NULLABLE
 * @return !0 -> true, 0 -> false
 */
#define is_symbol(object)               (is_object(object) && ((object)->type == OBJ_SYMBOL))
/**
 * �ж� object �Ƿ�Ϊ vector
 * @param object NULLABLE
 * @return !0 -> true, 0 -> false
 */
#define is_vector(object)               (is_object(object) && ((object)->type == OBJ_VECTOR))
// stack
#define is_stack(object)                (is_object(object) && ((object)->type == OBJ_STACK))

//port
#define is_port(obj)                    (is_object(obj) && (((obj)->type == OBJ_STRING_PORT) || ((obj)->type == OBJ_STDIO_PORT)))

// string_port
#define string_port_kind(obj)           ((obj)->value.string_port.kind)
#define is_string_port(obj)             (is_port(obj) && ((obj)->type == OBJ_STRING_PORT))
#define is_string_port_input(obj)       (is_port(obj) && ((unsigned)string_port_kind(obj) & (unsigned)PORT_INPUT))
#define is_string_port_output(obj)      (is_port(obj) && ((unsigned)string_port_kind(obj) & (unsigned)PORT_OUTPUT))
#define is_string_port_in_out_put(obj)  (is_port(obj) && ((unsigned)string_port_kind(obj) & ((unsigned)PORT_INPUT | (unsigned)PORT_OUTPUT)))
#define is_string_port_eof(obj)         (is_port(obj) && ((unsigned)string_port_kind(obj) & (unsigned)PORT_EOF))

// stdio_port
#define stdio_port_kind(obj)            ((obj)->value.stdio_port.kind)
#define is_stdio_port(obj)              (is_port(obj) && ((obj)->type == OBJ_STDIO_PORT))
#define is_stdio_port_input(obj)        (is_port(obj) && ((unsigned)stdio_port_kind(obj) & (unsigned)PORT_INPUT))
#define is_stdio_port_output(obj)       (is_port(obj) && ((unsigned)stdio_port_kind(obj) & (unsigned)PORT_OUTPUT))
#define is_stdio_port_in_out_put(obj)   (is_port(obj) && ((unsigned)stdio_port_kind(obj) & ((unsigned)PORT_INPUT | (unsigned)PORT_OUTPUT)))
#define is_stdio_port_eof(obj)          (is_port(obj) && ((unsigned)stdio_port_kind(obj) & (unsigned)PORT_EOF))

// srfi6
#define is_srfi6_port(obj)              ((is_string_port(obj)) && ((unsigned)string_port_kind(obj) & (unsigned)PORT_SRFI6))

// port
#define is_port_input(obj)              (is_string_port(obj) ? is_string_port_input(obj) : is_stdio_port_input(obj))
#define is_port_output(obj)             (is_string_port(obj) ? is_string_port_output(obj) : is_stdio_port_output(obj))
#define is_port_in_out_put(obj)         (is_string_port(obj) ? is_string_port_in_out_put(obj) : is_stdio_port_in_out_put(obj))

#define is_port_eof(obj)                (is_string_port(obj) ? is_string_port_eof(obj) : is_stdio_port_eof(obj))

// hash set
#define is_hashset(obj)                 (is_object(obj) && (obj)->type == OBJ_HASH_SET)
// hash map
#define is_hashmap(obj)                 (is_object(obj) && (obj)->type == OBJ_HASH_MAP)

// ������
#define is_weak_ref(obj)                (is_object(obj) && (obj)->type == OBJ_WEAK_REF)
/**
                                ����ֵ����
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

#define pair_caar(x)        (pair_car(pair_car(x)))
#define pair_cadr(x)        (pair_car(pair_cdr(x)))
#define pair_cdar(x)        (pair_cdr(pair_car(x)))
#define pair_cddr(x)        (pair_cdr(pair_cdr(x)))
#define pair_caaar(x)       (pair_car(pair_caar(x)))
#define pair_caadr(x)       (pair_car(pair_cadr(x)))
#define pair_cadar(x)       (pair_car(pair_cdar(x)))
#define pair_caddr(x)       (pair_car(pair_cddr(x)))
#define pair_cdaar(x)       (pair_cdr(pair_caar(x)))
#define pair_cdadr(x)       (pair_cdr(pair_cadr(x)))
#define pair_cddar(x)       (pair_cdr(pair_cdar(x)))
#define pair_cdddr(x)       (pair_cdr(pair_cddr(x)))
#define pair_cadddr(x)      (pair_cadr(pair_cddr(x)))

/**
 * bytes �����ܴ�С
 */
#define bytes_capacity(obj)     ((obj)->value.bytes.capacity)

/**
 * bytes ����
 */
#define bytes_index(obj, i)     ((obj)->value.bytes.data[(i)])

/**
 * get bytes' data
 * @return
 */
#define bytes_data(obj)         ((obj)->value.bytes.data)

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
#define string_len(obj)     ((obj)->value.string.len - 1u)

/**
 * ʹ���������� string �� cstr �Ķ�Ӧ�ַ�, ��Χ [ 0, symbol_len(obj) )
 * @param object
 * @param i: ����ֵ
 * @return char ����
 */
#define string_index(obj, i) ((obj)->value.string.data[(i)])

/**
 * ��� string buffer �Ƿ�Ϊ��
 */
#define string_buffer_empty(obj)    ((obj)->value.string_buffer.buffer_length == 0)

/**
 * string buffer �ַ�����
 */
#define string_buffer_capacity(obj)     ((obj)->value.string_buffer.buffer_size)

/**
 * string buffer ��ǰ����
 */
#define string_buffer_length(obj)   ((obj)->value.string_buffer.buffer_length)

/**
 * string buffer �Ƿ�����
 */
#define string_buffer_full(obj)     (string_buffer_capacity(obj) == string_buffer_length(obj))

/**
 * string buffer ��������
 */
#define string_buffer_index(obj, i)    (bytes_index((obj)->value.string_buffer.bytes_buffer, (i)))

/**
 * ��ȡ string_buffer �ڲ��ֽ�����
 * @return
 */
#define string_buffer_bytes_data(obj)    (bytes_data((obj)->value.string_buffer.bytes_buffer))

/**
 * ��ȡ string_buffer �ڲ� bytes ����
 * @return
 */
#define string_buffer_bytes_obj(obj)    ((obj)->value.string_buffer.bytes_buffer)


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
 * ���ջ�Ƿ�Ϊ��
 * @param obj object
 * @return int 0 => ��; !0 => �ǿ�
 */
#define stack_empty(obj) (((obj)->value.stack.length) == 0)

/**
 * ���ջ�Ƿ�Ϊ��
 * @param obj object
 * @return int 0 => ��; !0 => ����
 */
#define stack_full(obj) (((obj)->value.stack.length) == ((obj)->value.stack.size))

/**
 * ���ջ
 */
#define stack_clean(obj) ((obj)->value.stack.length = 0)

/**
 * ջ����
 */
#define stack_capacity(obj) ((obj)->value.stack.size)

/**
 * ����ջ����, ����ջ��Ԫ������
 */
#define stack_len(obj) ((obj)->value.stack.length)

/**
 * ����ջ��Ԫ��
 * @param stack
 * @return object ���ջΪ��, ���� NULL
 */
NULLABLE CHECKED REF object stack_peek(object stack);

/**
 * ��ջ
 * @param stack
 * @param obj
 * @return ���ջ��, ���� 0, ���򷵻� 1
 */
NULLABLE OUT int stack_push(REF object stack, REF object obj);

/**
 * ��ջ
 * @param stack
 * @return ���ջ��, ���� 0; ���򷵻� 1
 */
CHECKED OUT int stack_pop(REF object stack);

/**
 * port �Ƿ���Ҫ�ر� (stdin stdout stderr ����Ҫ�ر�)
 */
#define stdio_port_need_close(obj) ((obj)->value.stdio_port.need_close)

/**
 * port �Ƿ��Ѿ��ͷ�
 */
#define stdio_port_is_released(obj) ((obj)->value.stdio_port.is_released)

/**
 * hashset Ԫ������
 */
#define hashset_size(obj)   ((obj)->value.hashset.size)

/**
 *  hashmap Ԫ������
 */
#define hashmap_size(obj)   ((obj)->value.hashmap.size)

/**
 * �������Ƿ����
 * @return 0: ������; 1: ����
 */
#define weak_ref_is_valid(obj)   ((obj)->value.weak_ref.ref != NULL)

/**
 * ��ȡ�����ö�Ӧ������
 * ���������, ���� NULL
 * ע��, �������޷�������Ƿ�����, ����������������������ǲ��ɿ���
 */
#define weak_ref_get(obj)   ((obj)->value.weak_ref.ref)


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