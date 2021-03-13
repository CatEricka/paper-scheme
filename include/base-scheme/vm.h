#ifndef BASE_SCHEME_VM_H
#define BASE_SCHEME_VM_H
#pragma once


/**
 * vm.h vm.c
 * ��������������Ľṹ����������������صĲ���, �Լ�����ֵ���͹��캯��
 */


#include "base-scheme/util.h"
#include "base-scheme/object.h"
#include "base-scheme/heap.h"
#include "base-scheme/context.h"
#include "base-scheme/gc.h"




/******************************************************************************
    ������ API
******************************************************************************/
/**
 * ���� i64 ���Ͷ���
 * @param heap
 * @param v i64 ֵ
 * @return
 */
EXPORT_API OUT NOTNULL object i64_make(REF NOTNULL context_t context, IN int64_t v);

/**
 * ���� i64 ���Ͷ���, ���ֵ��Χ���� [- 2^(63-1), 2^(63-1)-1] ����������
 * @param heap
 * @param v i64 ֵ
 * @return object ��������
 */
EXPORT_API OUT NOTNULL object i64_imm_auto_make(REF NOTNULL context_t context, IN int64_t v);

/**
 * ���� doublenum ���Ͷ���
 * @param context
 * @param v
 * @return
 */
EXPORT_API OUT NOTNULL object doublenum_make(REF NOTNULL context_t context, int64_t v);

/**
 * ���� pair ���Ͷ���
 * @param context
 * @param v
 * @return
 */
EXPORT_API OUT NOTNULL object
pair_make(REF NOTNULL context_t context, REF NULLABLE object car, REF NULLABLE object cdr);


#endif //BASE_SCHEME_VM_H
