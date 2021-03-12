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
 * @return �������ʧ��, ���� IMM_FALSE �� IMM_NIL
 */
EXPORT_API object i64_make(context_t context, int64_t v) {
    assert(context != NULL);

    // �˴�Ҫע���ڴ����
    size_t size = aligned_object_size(object_size(i64));
    object ret = gc_alloc(context, size);
    if (!is_pointer(ret)) {
        return ret;
    }
    memset(ret, 0, size);
    ret->type = OBJ_I64;
    ret->value.i64 = v;
    return ret;
}

EXPORT_API object doublenum_make(context_t context, int64_t v) {
    assert(context != NULL);

    size_t size = aligned_object_size(object_size(doublenum));
    object ret = gc_alloc(context, size);
    if (!is_pointer(ret)) {
        return ret;
    }
    memset(ret, 0, size);
    ret->type = OBJ_I64;
    ret->value.i64 = v;
    return ret;
}

#endif //BASE_SCHEME_VM_H
