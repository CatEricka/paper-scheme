#ifndef BASE_SCHEME_GC_H
#define BASE_SCHEME_GC_H
#pragma once


/**
 * gc.h gc.c
 * 包含堆结构定义, 以及在堆结构中分配和回收对象的 API
 */

#include <stdlib.h>
#include <string.h>


#include "base-scheme/object.h"


/******************************************************************************
    堆结构
******************************************************************************/
/**
 * 堆结构
 */
typedef struct scheme_heap_t {
    //总大小
    size_t size;
    //最大大小
    size_t max_size;
    //当前块大小
    size_t chunk_size;
    //下一个堆块
    struct scheme_heap_t *next;
    /* 注意这里要保证内存对齐 */
    //当前堆内存指针
    char *data;
} *heap_t;


/******************************************************************************
    垃圾回收 API
******************************************************************************/
/**
 * malloc() 的封装
 * @param size 字节数
 * @return 分配的内存块, 为空则分配失败
 */
EXPORT_API void *raw_alloc(size_t size) {
    void *mem = malloc(size);
    assert(((((uintptr_t) obj) & ((uintptr_t) 3)) == 0));
    return mem;
}
/**
 * free() 的封装
 * @param obj raw_alloc() 分配的内存
 */
EXPORT_API void raw_free(void *obj) {
    assert(((((uintptr_t) obj) & ((uintptr_t) 3)) == 0));
    free(obj);
}

/**
 * 从堆中分配内存
 * @param heap 堆结构
 * @param size 要分配的对象大小
 * @return 分配的内存块, 为空则分配失败内存不足
 */
static object gc_alloc(struct scheme_heap_t *heap, size_t size) {
    // TODO 真正实现托管的内存上分配
    return (object) raw_alloc(size);
}

/**
 * 标记存活对象
 * @param heap 堆结构
 * @return 运行是否成功
 */
static object gc_mark(struct scheme_heap_t *heap) {
    // TODO gc_mark
    return 0;
}

/**
 * 启动回收垃圾回收
 * @param heap
 * @return
 */
EXPORT_API object gc_collect(struct scheme_heap_t *heap) {
    // TODO 实现 gc_collect
    gc_mark(heap);
    return 0;
}


/**
 * 初始化堆结构
 * @param init_size 初始块大小
 * @param grown_scale 每次堆增长时, 新的堆是上一次分配堆大小的多少倍
 * @param max_size 最大堆大小
 * @return
 */
EXPORT_API heap_t init_scheme_heap(size_t init_size, size_t grown_scale, size_t max_size) {
    // TODO 写测试; 实现内存增长
    // TODO grown_scale 要存在某个地方
    // 记得释放 heap_t
    heap_t new_heap = raw_alloc(sizeof(struct scheme_heap_t));
    new_heap->next = NULL;
    new_heap->size = init_size;
    new_heap->chunk_size = init_size;   // 初始的时候, 第一个 heap 块大小与总的 heap 大小相同
    new_heap->max_size = max_size;
    // 记得实放 heap_t->data
    new_heap->data = raw_alloc(init_size);

    return new_heap;
}

// 释放堆结构
EXPORT_API void destroy_scheme_heap(heap_t heap) {
    // TODO 测试销毁堆结构
    heap_t next;
    while (heap != NULL) {
        next = heap->next;
        // 释放 heap_t->data
        raw_free(heap->data);
        // 释放 heap_t
        raw_free(heap);
        heap = next;
    }
}


/******************************************************************************
    对象构造 API
******************************************************************************/
/**
 * 分配 i64 类型的对象, 对齐到 sizeof void*, 不要直接调用
 * @param heap
 * @return
 */
static object alloc_i64(struct scheme_heap_t *heap) {
    // TODO 实现内存对齐
    object ret = (object) gc_alloc(heap, object_size(i64));
    memset(ret, 0, object_size(i64));
    return ret;
}
/**
 * 构造 i64 类型对象
 * @param heap
 * @param v i64 值
 * @return
 */
EXPORT_API object mk_i64(struct scheme_heap_t *heap, int64_t v) {
    object ret = alloc_i64(heap);
    ret->type = OBJ_I64;
    ret->value.i64 = v;
    return ret;
}


#endif // _BASE_SCHEME_GC_HEADER_