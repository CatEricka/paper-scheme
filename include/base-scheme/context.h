#ifndef BASE_SCHEME_CONTEXT_H
#define BASE_SCHEME_CONTEXT_H
#pragma once


/**
 * context.h context.c
 * 上下文结构, 所有 scheme 解释器操作需要这个结构为基础
 */


#include "base-scheme/util.h"
#include "base-scheme/object.h"
#include "base-scheme/heap.h"

typedef struct scheme_context_t {
    // 堆
    heap_t heap;
    FILE *port_stdin;
    FILE *port_stdout;
    FILE *port_stderr;
} *context_t;


/******************************************************************************
    解释器上下文结构
******************************************************************************/


/**
 * 构造上下文, 虚拟机操作是针对上下文结构进行的
 * @return
 */
EXPORT_API context_t context_make() {
    context_t context = raw_alloc(sizeof(struct scheme_context_t));
    notnull_or_return(context, "context make failed.", NULL);

    context->heap = heap_make(0x100, 2, 0x10000);
    notnull_or_return(context->heap, "context->heap make failed.", NULL);
    context->port_stdin = stdin;
    context->port_stdout = stdout;
    context->port_stderr = stderr;
    return context;
}

EXPORT_API void context_destroy(context_t context) {
    heap_destroy(context->heap);
    raw_free(context);
}


#endif //BASE_SCHEME_CONTEXT_H
