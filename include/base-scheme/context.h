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


/**
    解释器上下文结构
******************************************************************************/

/**
 * 构造上下文, 虚拟机操作是针对上下文结构进行的
 * @return NULL: 内存分配失败
 */
EXPORT_API context_t context_make(size_t heap_init_size, size_t heap_growth_scale, size_t heap_max_size);

/**
 * 释放上下文结构
 * @param context
 */
EXPORT_API void context_destroy(context_t context);


#endif //BASE_SCHEME_CONTEXT_H
