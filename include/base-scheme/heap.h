#ifndef BASE_SCHEME_HEAP_H
#define BASE_SCHEME_HEAP_H
#pragma once


/**
 * heap.h heap.c
 * 包含堆结构定义
 */


#include "base-scheme/util.h"
#include "base-scheme/object.h"


/******************************************************************************
    堆结构
******************************************************************************/
/**
 * 堆节点
 */
typedef struct scheme_heap_node_t {
    // 当前块大小
    size_t chunk_size;
    // 下一节点
    struct scheme_heap_node_t *next;
    //当前内存块起始空闲地址
    char *free_ptr;
    //当前堆内存指针, 注意内存对齐
    char *data;
} *heap_node_t;

/**
 * 堆结构头节点
 */
typedef struct scheme_heap_t {
    // 初始大小
    size_t init_size;
    // 总大小
    size_t total_size;
    // 最大大小
    size_t max_size;
    // 增长倍数
    size_t growth_scale;
    // 堆块头节点
    heap_node_t first_node;
    // 最后节点
    heap_node_t last_node;
} *heap_t;


EXPORT_API heap_node_t heap_node_make(size_t chunk_size) {
    // TODO heap_node_make 测试
    assert_aligned_size_check(chunk_size);

    heap_node_t node = raw_alloc(sizeof(struct scheme_heap_node_t));
    notnull_or_return(node, "heap_node_t node malloc failed", NULL);
    // 初始化
    node->chunk_size = chunk_size;
    node->next = NULL;
    // 分配实际内存块
    node->data = raw_alloc(chunk_size);
    node->free_ptr = node->data;
    if (node->data == NULL) {
        // 子结构分配失败, 销毁父结构
        raw_free(node);
    }
    notnull_or_return(node->data, "heap_node_t node -> data malloc failed", NULL);
    return node;
}

/**
 * 初始化堆结构
 * @param init_size 初始块大小 (bytes), 必须是 aligned_object_size() 对齐后的大小
 * @param growth_scale 每次堆增长时, 新的堆是上一次分配堆大小的多少倍
 * @param max_size 最大堆大小, 必须大于 init_size
 * @return
 */
EXPORT_API heap_t heap_make(size_t init_size, size_t growth_scale, size_t max_size) {
    // TODO heap_make 测试
    assert(init_size <= max_size);
    assert_aligned_ptr_check(init_size);

    // 记得释放 heap_t
    heap_t new_heap = raw_alloc(sizeof(struct scheme_heap_t));
    notnull_or_return(new_heap, "make heap_t new_heap malloc failed", NULL);
    // 初始化堆结构头
    new_heap->init_size = init_size;
    new_heap->total_size = init_size;
    new_heap->max_size = max_size;
    new_heap->growth_scale = growth_scale;
    new_heap->first_node = NULL;
    new_heap->last_node = NULL;

    // 构造第一个节点
    heap_node_t node = heap_node_make(init_size);
    if (node == NULL) {
        raw_free(new_heap);
    }
    notnull_or_return(node, "make first heap_node_t node malloc failed", NULL);
    new_heap->first_node = node;
    new_heap->last_node = node;

    return new_heap;
}


// 释放堆节点
static void heap_node_destroy(heap_node_t node) {
    // TODO heap_node_destroy 测试
    if (node != NULL) {
        if (node->data != NULL) {
            raw_free(node->data);
        }
        raw_free(node);
    }
}

// 释放堆结构
EXPORT_API void heap_destroy(heap_t heap) {
    // TODO heap_destroy 测试
    if (heap == NULL) {
        return;
    }

    heap_node_t next = heap->first_node;
    while (next != NULL) {
        heap_node_t tmp = next;
        next = next->next;
        heap_node_destroy(tmp);
    }

    raw_free(heap);
}

/**
 * 增大堆大小
 * @param heap
 * @return IMM_FALSE: 达到 max_size; IMM_TRUE: 增长成功; IMM_NIL: 系统内存不足
 */
EXPORT_API object heap_grow(heap_t heap) {
    assert(heap != NULL);

    // TODO heap_grow 测试
    size_t new_node_size = heap->last_node->chunk_size * heap->growth_scale;
    size_t new_total_size = heap->total_size + new_node_size;

    // 检查是否达到 max_size
    if (new_total_size > heap->max_size) {
        // 无法继续增长
        return IMM_FALSE;
    }

    // 创建新堆块
    heap_node_t new_node = heap_node_make(new_node_size);
    if (new_node == NULL) {
        // 进程无法获得足够的内存, 也许应当停止解释器运行
        return IMM_NIL;
    }

    // 只有操作成功才更新的堆大小
    heap->last_node->next = new_node;
    heap->last_node = new_node;
    heap->total_size += new_node_size;

    return IMM_TRUE;
}


#endif //BASE_SCHEME_HEAP_H
