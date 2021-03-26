#ifndef BASE_SCHEME_EVAL_H
#define BASE_SCHEME_EVAL_H
#pragma once


/**
 * interpreter.h interpreter.c
 * ���������������Ľṹ����������������صĲ���, �Լ�����ֵ���͹��캯��
 *
 * �������ݼ� test/test_cases/repl_test.h test/test_cases/value_test.h
 *
 * �ý�����ʵ��Ϊ AST Walker, ͨ�� opcodes dispatch ѭ���� sexp(pair) �ṹ���� EVAL-APPLY ѭ��
 *
 * �����ϴ����Ż������, ���������ȵĲ������ܶ��Ǵ���Ŀɶ�����ʵ��������ӵı�����
 */


#include <paper-scheme/gc.h>
#include <paper-scheme/opcodes.h>


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
EXPORT_API OUT NOTNULL GC object doublenum_make_op(REF NOTNULL context_t context, double v);

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
symbol_make_from_cstr_op(REF NOTNULL context_t context, char *cstr);

/**
 * ���� string ���Ͷ���
 * <p>string_len() ������������ '\0',
 * ���� object->string.len ���� '\0', ����Ϊ�˷�������ʱ��������С</p>
 * @param context
 * @param cstr C�ַ���, '\0'��β
 * @return
 */
EXPORT_API OUT NOTNULL GC object
string_make_from_cstr_op(REF NOTNULL context_t context, char *cstr);

/**
 * ���� vector ���Ͷ���, ��� Unit, �� '()
 * @param context
 * @param vector_len vector ��С
 * @return
 */
EXPORT_API OUT NOTNULL GC object
vector_make_op(REF NOTNULL context_t context, IN size_t vector_len);


#endif //BASE_SCHEME_EVAL_H
