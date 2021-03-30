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


#include <paper-scheme/gc.h>


/******************************************************************************
                            ��������ʼ��������� API
******************************************************************************/

EXPORT_API context_t interpreter_create(size_t heap_init_size, size_t heap_growth_scale, size_t heap_max_size);



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
 * @param char_size char ����, ʵ�� string_buffer Ҫ����һ�� char �洢 '\0\
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
 * ���� vector ���Ͷ���, ��� Unit, �� '()
 * @param context
 * @param vector_len vector ��С
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
 * <p>obj->value.string_port.length ���Ȱ��� '\0'</p>
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
 * @return ��ʧ�ܷ��� IMM_UNIT
 */
EXPORT_API OUT NOTNULL GC object
stdio_port_from_file(REF NOTNULL context_t context, REF NOTNULL FILE *file, enum port_kind kind);


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

/******************************************************************************
                                �������� API
******************************************************************************/

/**
 * bytes ����, ���
 * @param context
 * @param bytes
 * @param add_size ���ӵĴ�С
 * @return
 */
EXPORT_API OUT NOTNULL GC object
bytes_capacity_increase(REF NOTNULL context_t context, IN object bytes, size_t add_size);

/**
 * string_buffer ����, ���
 * @param context
 * @param str_buffer
 * @param add_size ������С
 * @return
 */
EXPORT_API OUT NOTNULL GC object
string_buffer_capacity_increase(REF NOTNULL context_t context, IN object str_buffer, size_t add_size);

/**
 * vector ����, ���
 * @param context
 * @param vec
 * @param add_size
 * @return
 */
EXPORT_API OUT NOTNULL GC object
vector_capacity_increase(REF NOTNULL context_t context, IN object vec, size_t add_size);

/**
 * stack ����, ���
 * @param context
 * @param stack
 * @param add_size
 * @return
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
 * @return
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
 * string_buffer ת��Ϊ string, ���
 * @param context
 * @param str_buffer string_buffer
 * @return string
 */
EXPORT_API OUT NOTNULL GC object
string_buffer_to_string(REF NOTNULL context_t context, COPY object str_buffer);


#endif //BASE_SCHEME_RUNTIME_H
