#ifndef BASE_SCHEME_VM_H
#define BASE_SCHEME_VM_H
#pragma once


/**
 * vm.h vm.c
 * ��������������Ľṹ����������������صĲ���, �Լ�����ֵ���͹��캯��
 * �������ݼ� test/test_cases/vm_test.h test/test_cases/value_test.h
 */


#include <paper-scheme/util.h>
#include <paper-scheme/object.h>
#include <paper-scheme/heap.h>
#include <paper-scheme/context.h>
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
EXPORT_API OUT NOTNULL object i64_make_real_object_op(REF NOTNULL context_t context, IN int64_t v);

/**
 * ���� i64 ���Ͷ���, ���ֵ��Χ���� [- 2^(63-1), 2^(63-1)-1] ����������
 * @param heap
 * @param v i64 ֵ
 * @return object ��������
 */
EXPORT_API OUT NOTNULL object i64_make_op(REF NOTNULL context_t context, IN int64_t v);

/**
 * ���� doublenum ���Ͷ���
 * @param context
 * @param v
 * @return
 */
EXPORT_API OUT NOTNULL object doublenum_make_op(REF NOTNULL context_t context, double v);

/**
 * ���� pair ���Ͷ���
 * @param context
 * @param v
 * @return
 */
EXPORT_API OUT NOTNULL object
pair_make_op(REF NOTNULL context_t context, REF NULLABLE object car, REF NULLABLE object cdr);

/**
 * ���� symbol ���Ͷ���
 * <p>symbol_len() ������������ '\0', ���� object->symbol.len ���� '\0', ����Ϊ�˷�������ʱ��������С</p>
 * @param context
 * @param cstr C�ַ���, '\0'��β
 * @return
 */
EXPORT_API OUT NOTNULL object
symbol_make_from_cstr_op(REF NOTNULL context_t context, char *cstr);

/**
 * ���� string ���Ͷ���
 * <p>string_len() ������������ '\0',
 * ���� object->string.len ���� '\0', ����Ϊ�˷�������ʱ��������С</p>
 * @param context
 * @param cstr C�ַ���, '\0'��β
 * @return
 */
EXPORT_API OUT NOTNULL object
string_make_from_cstr_op(REF NOTNULL context_t context, char *cstr);

/**
 * ���� vector ���Ͷ���, ��� Unit, �� '()
 * @param context
 * @param vector_len vector ��С
 * @return
 */
EXPORT_API OUT NOTNULL object
vector_make_op(REF NOTNULL context_t context, IN size_t vector_len);


#endif //BASE_SCHEME_VM_H
