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
EXPORT_API object i64_make(context_t context, int64_t v);

/**
 * ���� doublenum ���Ͷ���
 * @param context
 * @param v
 * @return
 */
EXPORT_API object doublenum_make(context_t context, int64_t v);

/**
 * ���� pair ���Ͷ���
 * @param context
 * @param v
 * @return
 */
EXPORT_API object pair_make(context_t context, object car, object cdr);

#endif //BASE_SCHEME_VM_H
