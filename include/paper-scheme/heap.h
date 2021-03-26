#ifndef BASE_SCHEME_HEAP_H
#define BASE_SCHEME_HEAP_H
#pragma once


/**
 * heap.h heap.c
 * 包含堆结构定义
 * 测试内容见 test/test_cases/gc_test.h
 */


#include <paper-scheme/object.h>


/**
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

/**
 * 初始化堆结构
 * @param init_size 初始块大小 (bytes), 必须是 aligned_size() 对齐后的大小
 * @param growth_scale 每次堆增长时, 新的堆是上一次分配堆大小的多少倍
 * @param max_size 最大堆大小, 必须大于 init_size
 * @return malloc 失败时返回 NULL
 */
EXPORT_API OUT NULLABLE heap_t heap_make(IN size_t init_size, IN size_t growth_scale, IN size_t max_size);

/**
 * 释放堆结构
 * @param heap
 */
EXPORT_API void heap_destroy(IN NOTNULL heap_t heap);

/**
 * 增大堆大小
 * @param heap
 * @return <li>IMM_FALSE: 达到 max_size;</li><li>IMM_TRUE: 增长成功</li><li>IMM_UNIT: 系统内存不足或堆结构异常</li>
 */
EXPORT_API OUT CHECKED object heap_grow(REF NOTNULL heap_t heap);


#endif //BASE_SCHEME_HEAP_H
