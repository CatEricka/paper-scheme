#ifndef BASE_SCHEME_CONTEXT_H
#define BASE_SCHEME_CONTEXT_H
#pragma once


/**
 * context.h context.c
 * �����Ľṹ, ���� scheme ������������Ҫ����ṹΪ����
 */


#include "base-scheme/util.h"
#include "base-scheme/object.h"
#include "base-scheme/heap.h"


typedef struct scheme_context_t {
    // ��
    heap_t heap;
    FILE *port_stdin;
    FILE *port_stdout;
    FILE *port_stderr;
} *context_t;


/**
    �����������Ľṹ
******************************************************************************/

/**
 * ����������, �������������������Ľṹ���е�
 * @return NULL: �ڴ����ʧ��
 */
EXPORT_API context_t context_make(size_t heap_init_size, size_t heap_growth_scale, size_t heap_max_size);

/**
 * �ͷ������Ľṹ
 * @param context
 */
EXPORT_API void context_destroy(context_t context);


#endif //BASE_SCHEME_CONTEXT_H
