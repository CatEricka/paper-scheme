#include <paper-scheme/heap.h>


/**
 * 内部方法 构造堆节点, 不要直接调用, 参见 heap_make()
 * @param chunk_size 节点块大小
 * @return malloc 失败时返回 NULL
 */
static heap_node_t heap_node_make(size_t chunk_size) {
    assert_aligned_size_check(chunk_size);

    heap_node_t node = raw_alloc(sizeof(struct scheme_heap_node_t));
    //return;
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
    //return;
    notnull_or_return(node->data, "heap_node_t node -> data malloc failed", NULL);
    return node;
}

/**
 * 初始化堆结构
 * @param init_size 初始块大小 (bytes), 必须是 aligned_size() 对齐后的大小
 * @param growth_scale 每次堆增长时, 新的堆是上一次分配堆大小的多少倍
 * @param max_size 最大堆大小, 必须大于 init_size
 * @return malloc 失败时返回 NULL
 */
EXPORT_API OUT NULLABLE heap_t heap_make(IN size_t init_size, IN size_t growth_scale, IN size_t max_size) {
    assert(init_size <= max_size);
    assert_aligned_ptr_check(init_size);

    // 记得释放 heap_t
    heap_t new_heap = raw_alloc(sizeof(struct scheme_heap_t));
    //return;
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
    //return;
    notnull_or_return(node, "make first heap_node_t node malloc failed", NULL);
    new_heap->first_node = node;
    new_heap->last_node = node;

    return new_heap;
}


/**
 * 释放堆节点, 不要直接使用, 参见 heap_destroy()
 * @param node
 */
static void heap_node_destroy(heap_node_t node) {
    if (node != NULL) {
        if (node->data != NULL) {
            raw_free(node->data);
        }
        raw_free(node);
    }
}

/**
 * 释放堆结构
 * @param heap
 */
EXPORT_API void heap_destroy(IN NOTNULL heap_t heap) {
    if (heap == NULL) {
        return;
    }

    // 释放堆块链表
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
 * @return <li>IMM_FALSE: 达到 max_size;</li><li>IMM_TRUE: 增长成功</li><li>IMM_UNIT: 系统内存不足或堆结构异常</li>
 */
EXPORT_API OUT CHECKED object heap_grow(REF NOTNULL heap_t heap) {
    assert(heap != NULL);

    // 安全检查
    if (heap->first_node == NULL || heap->last_node == NULL) {
        return IMM_UNIT;
    }
    if (heap->first_node->data == NULL || heap->last_node->data == NULL) {
        return IMM_UNIT;
    }
    if (heap->first_node->free_ptr == NULL || heap->last_node->free_ptr == NULL) {
        return IMM_UNIT;
    }

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
        return IMM_UNIT;
    }

    // 只有操作成功才更新的堆大小
    heap->last_node->next = new_node;
    heap->last_node = new_node;
    heap->total_size += new_node_size;

    return IMM_TRUE;
}
