#ifndef BASE_SCHEME_VM_H
#define BASE_SCHEME_VM_H
#pragma once


/**
 * vm.h vm.c
 * 包含虚拟机上下文结构定义和虚拟机运行相关的操作
 */

#include "base-scheme/util.h"
#include "base-scheme/gc.h"



/******************************************************************************
    解释器上下文结构
******************************************************************************/
/**
 * 上下文结构, 所有 scheme 解释器操作需要这个结构为基础
 */
typedef struct scheme_context_t {
    // 堆
    heap_t *heap;
} *context_t;

/**
 * 构造上下文, 虚拟机操作是针对上下文结构进行的
 * @return
 */
EXPORT_API context_t mk_context() {
    context_t context = raw_alloc(sizeof(struct scheme_context_t));
    context->heap = init_scheme_heap();
    return context;
}

#endif //BASE_SCHEME_VM_H
