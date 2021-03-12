#ifndef BASE_SCHEME_GC_H
#define BASE_SCHEME_GC_H
#pragma once


/**
 * gc.h gc.c
 * 包含堆结构定义, 以及在堆结构中分配和回收对象的 API
 */


#include "base-scheme/util.h"
#include "base-scheme/object.h"
#include "base-scheme/heap.h"
#include "base-scheme/context.h"



/******************************************************************************
    垃圾回收 API
******************************************************************************/

/**
 * 标记存活对象
 * @param heap 堆结构
 * @return 运行是否成功
 */
static object gc_mark(context_t context) {
    // TODO gc_mark
    return IMM_TRUE;
}

/**
 * 启动回收垃圾回收
 * @param heap
 * @return
 */
EXPORT_API object gc_collect(context_t context) {
    // TODO 实现 gc_collect
    gc_mark(context);
    // context->gc_sink = IMM_FALSE;
    return IMM_TRUE;
}

/**
 * 尝试从堆中分配内存, 不会触发 gc, 失败返回 NULL
 * @param heap 堆结构
 * @param size 要分配的对象大小
 * @return 分配的内存块, 为空则分配失败内存不足
 */
EXPORT_API object gc_try_alloc(context_t context, size_t size) {
    assert(context != NULL);
    assert(context->heap != NULL);
    assert_aligned_size_check(size);

    heap_t heap = context->heap;
    object obj = NULL;

    // 链表中搜索
    for (heap_node_t node = heap->first_node; node != NULL; node = node->next) {
        size_t used_space = node->free_ptr - node->data;
        size_t free_space = node->chunk_size - used_space;
        if (free_space >= size) {
            // 找到空闲位置
            obj = (object) node->free_ptr;
            // 后移 free_ptr
            node->free_ptr += size;
            break;
        }
    }


//    if (obj == IMM_NIL) {
//        // 如果找不到, obj 为 NULL, 设置需要 gc 标记
//        context->gc_sink = IMM_TRUE;
//    } else {
//        // 如果找到, obj 为分配的对象, gc标记为 false
//        context->gc_sink = IMM_FALSE;
//    }
    return obj;
}

/**
 * 从堆中分配内存
 * @param heap 堆结构
 * @param size 要分配的对象大小
 * @return 分配的内存块
 * IMM_FALSE: 达到最大堆大小
 * IMM_NIL: 未达到最大堆大小, 但是系统内存不足
 */
EXPORT_API object gc_alloc(context_t context, size_t size) {
    assert(context != NULL);
    assert(context->heap != NULL);
    assert_aligned_size_check(size);

    // 1. 第一次尝试分配
    object obj = gc_try_alloc(context, size);
    // 分配成功, 返回
    if (obj != NULL) {
        return obj;
    }

    // 2. obj 为空, 且需要 gc, 进行一次 gc
    gc_collect(context);

    // 3. gc 后第二次尝试分配
    obj = gc_try_alloc(context, size);
    // 分配成功, 反回
    if (obj != NULL) {
        return obj;
    }

    // 4. 堆空间不足, 尝试增长堆空间后分配
    object heap_grow_result = heap_grow(context->heap);
    obj = gc_try_alloc(context, size);
    // 多次尝试, 直到增长失败或分配成功为止
    while ((obj == NULL) && (heap_grow_result == IMM_TRUE)) {
        heap_grow_result = heap_grow(context->heap);
        obj = gc_try_alloc(context, size);
    }

    // 5. 最终阶段.
    // 如果发现分配成功, 返回 obj
    if (obj != NULL) {
        return obj;
    }

    if (heap_grow_result == IMM_FALSE) {
        return IMM_FALSE;
    } else {
        return IMM_NIL;
    }

}


#endif // BASE_SCHEME_GC_H