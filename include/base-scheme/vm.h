#ifndef BASE_SCHEME_VM_H
#define BASE_SCHEME_VM_H
#pragma once


/**
 * vm.h vm.c
 * ��������������Ľṹ����������������صĲ���
 */

#include "base-scheme/util.h"
#include "base-scheme/gc.h"



/******************************************************************************
    �����������Ľṹ
******************************************************************************/
/**
 * �����Ľṹ, ���� scheme ������������Ҫ����ṹΪ����
 */
typedef struct scheme_context_t {
    // ��
    heap_t *heap;
} *context_t;

/**
 * ����������, �������������������Ľṹ���е�
 * @return
 */
EXPORT_API context_t mk_context() {
    context_t context = raw_alloc(sizeof(struct scheme_context_t));
    context->heap = init_scheme_heap();
    return context;
}

#endif //BASE_SCHEME_VM_H
