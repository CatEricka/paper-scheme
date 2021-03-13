#include "base-scheme/gc.h"


/**
    垃圾回收 API
******************************************************************************/

/**
 * 内部方法 标记存活对象
 * @param context
 * @return <li>IMM_TRUE: 运行成功</li><li>IMM_FALSE: 运行失败</li>
 */
static object gc_mark(context_t context) {
    // TODO gc_mark
    return IMM_TRUE;
}

/**
 * 启动回收垃圾回收
 * @param context
 * @return <li>IMM_TRUE: 运行成功</li><li>IMM_FALSE: 运行失败</li>
 */
EXPORT_API object gc_collect(context_t context) {
    // TODO 实现 gc_collect
    gc_mark(context);
    return IMM_TRUE;
}

/**
 * 尝试从堆中分配内存, 不会触发 gc, 失败返回 NULL
 * @param heap 堆结构
 * @param size 要分配的对象大小
 * @return <li>NULL: 找不到足够大的空闲控件</li>
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

    // 找不到的话会返回 NULL
    return obj;
}

/**
 * 从堆中分配内存
 * <p>注意, 到了底层与托管内存的分界线了</p>
 * <p>!此函数失败会直接结束进程</p>
 * @param heap 堆结构
 * @param size 要分配的对象大小
 * @return
 * <li>exit(EXIT_FAILURE_OUT_OF_MEMORY): 达到最大堆大小</li>
 * <li>exit(EXIT_FAILURE_MALLOC_FAILED): 未达到最大堆大小, 但是系统内存不足</li>
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

    heap_t heap = context->heap;
    if (heap_grow_result == IMM_FALSE) {
        fprintf(context->port_stderr, "[ERROR] Out of Memory:");
        fprintf(context->port_stderr, " heap total size 0x%zx, try to growth to 0x%zx, max heap size 0x%zx\n",
                heap->total_size, heap->last_node->chunk_size * heap->growth_scale + heap->total_size, heap->max_size);
        exit(EXIT_FAILURE_OUT_OF_MEMORY);
        //return IMM_FALSE;
    } else {
        fprintf(context->port_stderr, "[ERROR] malloc() failed:");
        fprintf(context->port_stderr, " heap total size 0x%zx, try to growth to 0x%zx, max heap size 0x%zx\n",
                heap->total_size, heap->last_node->chunk_size * heap->growth_scale + heap->total_size, heap->max_size);
        exit(EXIT_FAILURE_MALLOC_FAILED);
        //return IMM_NIL;
    }

}