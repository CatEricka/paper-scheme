#ifndef BASE_SCHEME_RUNTIME_H
#define BASE_SCHEME_RUNTIME_H
#pragma once


/**
 * runtime.h runtime.c
 * ���������������Ľṹ���������ʱ֧��, �Լ�����ֵ���͹��캯��
 *
 * �������ݼ� test/test_cases/repl_test.h test/test_cases/value_test.h
 *
 */


#include <paper-scheme/opcodes.h>
#include <paper-scheme/gc.h>


/******************************************************************************
                                ������ API
******************************************************************************/

/**
 * ���� i64 ���Ͷ���, ������ֱ��ʹ��, ��ΪҪ���ǵ����ӵı߽�����, �μ� i64_make_op()
 * @param heap
 * @param v i64 ֵ
 * @return
 */
EXPORT_API OUT NOTNULL GC object i64_make_real_object_op(REF NOTNULL context_t context, IN int64_t v);

/**
 * ���� i64 ���Ͷ���, ���ֵ��Χ���� [- 2^(63-1), 2^(63-1)-1] ����������
 * @param heap
 * @param v i64 ֵ
 * @return object ��������
 */
EXPORT_API OUT NOTNULL GC object i64_make_op(REF NOTNULL context_t context, IN int64_t v);

/**
 * ���� doublenum ���Ͷ���
 * @param context
 * @param v
 * @return
 */
EXPORT_API OUT NOTNULL GC object doublenum_make_op(REF NOTNULL context_t context, IN double v);

/**
 * ���� pair ���Ͷ���
 * @param context
 * @param v
 * @return
 */
EXPORT_API OUT NOTNULL GC object
pair_make_op(REF NOTNULL context_t context, REF NULLABLE object car, REF NULLABLE object cdr);

/**
 * ���� symbol ���Ͷ���
 * <p>symbol_len() ������������ '\0', ���� object->symbol.len ���� '\0', ����Ϊ�˷�������ʱ��������С</p>
 * @param context
 * @param cstr C�ַ���, '\0'��β
 * @return
 */
EXPORT_API OUT NOTNULL GC object
symbol_make_from_cstr_op(REF NOTNULL context_t context, COPY char *cstr);

/**
 * ���� bytes ����
 * @param context
 * @param capacity bytes ����, bytes_size * sizeof(char)
 * @return
 */
EXPORT_API OUT NOTNULL GC object
bytes_make_op(REF NOTNULL context_t context, IN size_t capacity);

/**
 * ���� string ���Ͷ���
 * <p>string_len() ������������ '\0',
 * ���� object->string.len ���� '\0', ����Ϊ�˷�������ʱ��������С</p>
 * @param context
 * @param cstr C�ַ���, '\0'��β
 * @return
 */
EXPORT_API OUT NOTNULL GC object
string_make_from_cstr_op(REF NOTNULL context_t context, COPY char *cstr);

/**
 * ���� string_buffer ����
 * @param context
 * @param char_size char ����, ע�� string_buffer ���� '\0' ����
 * @return
 */
EXPORT_API OUT NOTNULL GC object
string_buffer_make_op(REF NOTNULL context_t context, IN size_t char_size);

/**
 * �� string ���� string_buffer ����, ĩβ���� '\0'
 * @param context
 * @param str string, ���
 * @return
 */
EXPORT_API OUT NOTNULL GC object
string_buffer_make_from_string_op(REF NOTNULL context_t context, COPY object str);

/**
 * ���� vector ���Ͷ���, ��ʼ����� Unit, �� '()
 * @param context
 * @param vector_len vector ����
 * @return
 */
EXPORT_API OUT NOTNULL GC object
vector_make_op(REF NOTNULL context_t context, IN size_t vector_len);

/**
 * ���� stack ���Ͷ���, ��� Unit.
 * @param context
 * @param stack_size
 * @return
 */
EXPORT_API OUT NOTNULL GC object
stack_make_op(REF NOTNULL context_t context, IN size_t stack_size);

/**
 * �������ַ����� input port, ����� string Ӧ���ǲ��ɱ����
 * <p>obj->value.string_port.length ���� ������ '\0'</p>
 * <p>(open-input-string "string here")</p>
 * @param context
 * @param str
 * @return ��ʧ�ܷ��� IMM_UNIT
 */
EXPORT_API OUT NOTNULL GC object
string_port_input_from_string(REF NOTNULL context_t context, REF NULLABLE object str);

/**
 * �� output port, �����ڲ�����
 * <p>(open-output-string)</p>
 * @param context
 * @return ��ʧ�ܷ��� IMM_UNIT, ���򷵻�ֵ���� is_srfi6_port(obj),
 */
EXPORT_API OUT NOTNULL GC object
string_port_output_use_buffer(REF NOTNULL context_t context);

/**
 * �������ַ����� input-output port, ���, �����ڲ�����
 * <p>(open-input-output-string "string here")</p>
 * @param context
 * @param str
 * @return ��ʧ�ܷ��� IMM_UNIT, ���򷵻�ֵ���� is_srfi6_port(obj), is_string_port_in_out_put(obj)
 */
EXPORT_API OUT NOTNULL GC object
string_port_in_out_put_from_string_use_buffer(REF NOTNULL context_t context, COPY NULLABLE object str);

/**
 * ���ļ����� file port
 * @param context
 * @param filename
 * @param kind PORT_INPUT / PORT_OUTPUT / PORT_INPUT & PORT_OUTPUT
 * @return ��ʧ�ܷ��� IMM_UNIT
 */
EXPORT_API OUT NOTNULL GC object
stdio_port_from_filename(REF NOTNULL context_t context, REF NULLABLE object filename, IN enum port_kind kind);

/**
 * �� FILE * �� file port
 * @param context
 * @param file
 * @param kind PORT_INPUT / PORT_OUTPUT / PORT_INPUT & PORT_OUTPUT
 * @return
 */
EXPORT_API OUT NOTNULL GC object
stdio_port_from_file(REF NOTNULL context_t context, REF NOTNULL FILE *file, enum port_kind kind);

/**
 * ���� hashset
 * @param context
 * @param init_capacity hashset ��ʼ��С (Ĭ�� DEFAULT_HASH_SET_MAP_INIT_CAPACITY)
 * @param load_factor �������� (Ĭ�ϴ�С DEFAULT_HASH_SET_MAP_LOAD_FACTOR)
 * @return
 */
EXPORT_API OUT NOTNULL GC object
hashset_make_op(REF NOTNULL context_t context, IN size_t init_capacity, IN double load_factor);

/**
 * ���� hashmap
 * @param context
 * @param init_capacity hashmap ��ʼ��С (Ĭ�� DEFAULT_HASH_SET_MAP_INIT_CAPACITY)
 * @param load_factor �������� (Ĭ�ϴ�С DEFAULT_HASH_SET_MAP_LOAD_FACTOR)
 * @return
 */
EXPORT_API OUT NOTNULL GC object
hashmap_make_op(REF NOTNULL context_t context, IN size_t init_capacity, IN double load_factor);

/**
 * ���� weak ref
 * @param context
 * @param obj
 * @return
 */
EXPORT_API OUT NOTNULL GC object
weak_ref_make_op(REF NOTNULL context_t context, REF NULLABLE object obj);

/******************************************************************************
                                ������� API
******************************************************************************/

/**
 * string ����ƴ��, ���
 * @param context
 * @param string_a
 * @param string_b
 * @return ƴ�Ӻ�� string
 */
EXPORT_API OUT NOTNULL GC object
string_append_op(REF NOTNULL context_t context, COPY NULLABLE object string_a, COPY NULLABLE object string_b);

/**
 * string_buffer ƴ�� string, ���
 * @param context
 * @param str_buffer string_buffer
 * @param str string
 * @return �޸ĺ�� string_buffer
 */
EXPORT_API OUT NOTNULL GC object
string_buffer_append_string_op(
        REF NOTNULL context_t context,
        IN NULLABLE object str_buffer, COPY NULLABLE object str);

/**
 * string_buffer ƴ�� imm_char, ���
 * @param context
 * @param str_buffer
 * @param imm_char
 * @return
 */
EXPORT_API OUT NOTNULL GC object
string_buffer_append_imm_char_op(
        REF NOTNULL context_t context,
        IN NULLABLE object str_buffer, COPY NOTNULL object imm_char);

/**
 * string_buffer ƴ�� char, ���
 * @param context
 * @param str_buffer
 * @param ch
 * @return
 */
EXPORT_API OUT NOTNULL GC object
string_buffer_append_char_op(REF NOTNULL context_t context, IN NULLABLE object str_buffer, COPY char ch);


/**
 * hashset �Ƿ����ָ���Ķ���
 * <p>���ᴥ�� GC</p>
 * @param context
 * @param hashset
 * @param obj object ����Ϊ NULL
 * @return IMM_TRUE / IMM_FALSE
 */
EXPORT_API OUT NOTNULL object
hashset_contains_op(REF NOTNULL context_t context, REF NOTNULL object hashset, REF NOTNULL object obj);

/**
 * obj ���� hashset
 * @param context
 * @param obj
 */
EXPORT_API GC void
hashset_put_op(REF NOTNULL context_t context, REF NOTNULL object hashset, REF NOTNULL object obj);

/**
 * hashset_b ȫ������ hashset_a, ǳ����
 * @param context
 * @param hashset_a ����Ϊ��
 * @param hashset_b ����Ϊ��
 */
EXPORT_API GC void
hashset_put_all_op(REF NOTNULL context_t context, REF NOTNULL object hashset_a, REF NOTNULL object hashset_b);

/**
 * ��� hashset
 * <p>���ᴥ�� GC</p>
 * @param context
 * @param hashset ����Ϊ��
 * @return
 */
EXPORT_API void hashset_clear_op(REF NOTNULL context_t context, REF NOTNULL object hashset);

/**
 * �� hashset ���Ƴ� object
 * <p>���ᴥ�� GC</p>
 * @param context
 * @param hashset
 * @param obj ����Ϊ��, ����Ϊ IMM_UNIT
 */
EXPORT_API void
hashset_remove_op(REF NOTNULL context_t context, REF NOTNULL object hashset, REF NOTNULL object obj);


/**
 * hashmap �Ƿ����ָ���Ķ���
 * <p>���ᴥ�� GC</p>
 * @param context
 * @param hashmap
 * @param key object ����Ϊ NULL
 * @return IMM_TRUE / IMM_FALSE
 */
EXPORT_API OUT NOTNULL object
hashmap_contains_key_op(REF NOTNULL context_t context, REF NOTNULL object hashmap, REF NOTNULL object key);

/**
 * obj ���� hashmap
 * @param context
 * @param hashmap
 * @param k ��
 * @param v ֵ
 * @return ��� k �Ѿ�����, �򷵻ؾɵ� v, ���򷵻� IMM_UNIT
 */
EXPORT_API OUT NOTNULL GC object
hashmap_put_op(REF NOTNULL context_t context, object hashmap, REF NOTNULL object k, REF NOTNULL object v);

/**
 * hashmap ȡ�� key ��Ӧ�� value
 * <p>���ᴥ�� GC</p>
 * @param context
 * @param hashmap
 * @param key
 * @return ��� key ����, �򷵻ض�Ӧ�� value, ���򷵻� IMM_UNIT
 */
EXPORT_API OUT NOTNULL object
hashmap_get_op(REF NOTNULL context_t context, object hashmap, REF NOTNULL object key);

/**
 * hashmap_b ȫ������ hashmap_a, ǳ����
 * @param context
 * @param hashmap_a
 * @param hashmap_b
 */
EXPORT_API void
hashmap_put_all_op(REF NOTNULL context_t context, REF NOTNULL object hashmap_a, REF NOTNULL object hashmap_b);

/**
 * ��� hashmap
 * <p>���ᴥ�� GC</p>
 * @param context
 * @param hashmap
 */
EXPORT_API void hashmap_clear_op(REF NOTNULL context_t context, REF NOTNULL object hashmap);

/**
 * �� hashmap �Ƴ�ָ���� key
 * <p>���ᴥ�� GC</p>
 * @param context
 * @param hashmap
 * @param key
 * @return ��� key �Ѿ�����, ���ر��Ƴ��� value, ���򷵻� IMM_UNIT
 */
EXPORT_API OUT NOTNULL object
hashmap_remove_op(REF NOTNULL context_t context, REF NOTNULL object hashmap, REF NOTNULL object key);


/******************************************************************************
                                �������� API
******************************************************************************/

/**
 * bytes ����, ���
 * @param context
 * @param bytes
 * @param add_size ���ӵĴ�С
 * @return �᷵���¶���
 */
EXPORT_API OUT NOTNULL GC object
bytes_capacity_increase(REF NOTNULL context_t context, IN object bytes, size_t add_size);

/**
 * string_buffer ����, ���
 * @param context
 * @param str_buffer
 * @param add_size ������С
 * @return ����ԭʼ string_buffer
 */
EXPORT_API OUT NOTNULL GC object
string_buffer_capacity_increase(REF NOTNULL context_t context, IN object str_buffer, size_t add_size);

/**
 * vector ����, ���
 * @param context
 * @param vec
 * @param add_size
 * @return �᷵���� vector
 */
EXPORT_API OUT NOTNULL GC object
vector_capacity_increase(REF NOTNULL context_t context, IN object vec, size_t add_size);

/**
 * stack ����, ���
 * @param context
 * @param stack
 * @param add_size
 * @return �᷵���� stack
 */
EXPORT_API OUT NOTNULL GC object
stack_capacity_increase(REF NOTNULL context_t context, IN object stack, size_t add_size);

/**
 * stack �Զ���������ջ
 * <p>ע��, push ��Ӧ�����¸�ԭ����ջ��ֵ</p>
 * @param context
 * @param stack
 * @param element
 * @param extern_growth_size ���ջ��, ���Զ����� extern_growth_size + 1 (�� 0 ���Զ����� 1)
 * @return ���ܷ����� stack
 */
EXPORT_API OUT NOTNULL GC object
stack_push_auto_increase(REF NOTNULL context_t context, REF object stack, REF object element,
                         size_t extern_growth_size);


/******************************************************************************
                                ����ת�� API
******************************************************************************/

/**
 * char ������ ת string
 * @param context
 * @param imm_char ������
 * @return string
 */
EXPORT_API OUT NOTNULL GC object
imm_char_to_string(REF NOTNULL context_t context, COPY object imm_char);

/**
 * char ת string
 * @param context
 * @param ch
 * @return string
 */
EXPORT_API OUT NOTNULL GC object
char_to_string(REF NOTNULL context_t context, COPY char ch);

/**
 * symbol ת string
 * @param context
 * @param symbol
 * @return
 */
EXPORT_API OUT NOTNULL GC object
symbol_to_string(REF NOTNULL context_t context, COPY object symbol);

/**
 * string ת symbol
 * @param context
 * @param string
 * @return
 */
EXPORT_API OUT NOTNULL GC object
string_to_symbol(REF NOTNULL context_t context, COPY object str);

/**
 * string_buffer ת��Ϊ string, ���
 * @param context
 * @param str_buffer string_buffer
 * @return string
 */
EXPORT_API OUT NOTNULL GC object
string_buffer_to_string(REF NOTNULL context_t context, COPY object str_buffer);

/**
 * string_buffer ת��Ϊ symbol, ���
 * @param context
 * @param str_buffer string_buffer
 * @return symbol
 */
EXPORT_API OUT NOTNULL GC object
string_buffer_to_symbol(REF NOTNULL context_t context, COPY object str_buffer);

#endif //BASE_SCHEME_RUNTIME_H
