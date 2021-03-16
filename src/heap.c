#include <paper-scheme/heap.h>


/**
 * �ڲ����� ����ѽڵ�, ��Ҫֱ�ӵ���, �μ� heap_make()
 * @param chunk_size �ڵ���С
 * @return malloc ʧ��ʱ���� NULL
 */
static heap_node_t heap_node_make(size_t chunk_size) {
    assert_aligned_size_check(chunk_size);

    heap_node_t node = raw_alloc(sizeof(struct scheme_heap_node_t));
    //return;
    notnull_or_return(node, "heap_node_t node malloc failed", NULL);
    // ��ʼ��
    node->chunk_size = chunk_size;
    node->next = NULL;
    // ����ʵ���ڴ��
    node->data = raw_alloc(chunk_size);
    node->free_ptr = node->data;
    if (node->data == NULL) {
        // �ӽṹ����ʧ��, ���ٸ��ṹ
        raw_free(node);
    }
    //return;
    notnull_or_return(node->data, "heap_node_t node -> data malloc failed", NULL);
    return node;
}

/**
 * ��ʼ���ѽṹ
 * @param init_size ��ʼ���С (bytes), ������ aligned_size() �����Ĵ�С
 * @param growth_scale ÿ�ζ�����ʱ, �µĶ�����һ�η���Ѵ�С�Ķ��ٱ�
 * @param max_size ���Ѵ�С, ������� init_size
 * @return malloc ʧ��ʱ���� NULL
 */
EXPORT_API OUT NULLABLE heap_t heap_make(IN size_t init_size, IN size_t growth_scale, IN size_t max_size) {
    assert(init_size <= max_size);
    assert_aligned_ptr_check(init_size);

    // �ǵ��ͷ� heap_t
    heap_t new_heap = raw_alloc(sizeof(struct scheme_heap_t));
    //return;
    notnull_or_return(new_heap, "make heap_t new_heap malloc failed", NULL);
    // ��ʼ���ѽṹͷ
    new_heap->init_size = init_size;
    new_heap->total_size = init_size;
    new_heap->max_size = max_size;
    new_heap->growth_scale = growth_scale;
    new_heap->first_node = NULL;
    new_heap->last_node = NULL;

    // �����һ���ڵ�
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
 * �ͷŶѽڵ�, ��Ҫֱ��ʹ��, �μ� heap_destroy()
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
 * �ͷŶѽṹ
 * @param heap
 */
EXPORT_API void heap_destroy(IN NOTNULL heap_t heap) {
    if (heap == NULL) {
        return;
    }

    // �ͷŶѿ�����
    heap_node_t next = heap->first_node;
    while (next != NULL) {
        heap_node_t tmp = next;
        next = next->next;
        heap_node_destroy(tmp);
    }

    raw_free(heap);
}

/**
 * ����Ѵ�С
 * @param heap
 * @return <li>IMM_FALSE: �ﵽ max_size;</li><li>IMM_TRUE: �����ɹ�</li><li>IMM_UNIT: ϵͳ�ڴ治���ѽṹ�쳣</li>
 */
EXPORT_API OUT CHECKED object heap_grow(REF NOTNULL heap_t heap) {
    assert(heap != NULL);

    // ��ȫ���
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

    // ����Ƿ�ﵽ max_size
    if (new_total_size > heap->max_size) {
        // �޷���������
        return IMM_FALSE;
    }

    // �����¶ѿ�
    heap_node_t new_node = heap_node_make(new_node_size);
    if (new_node == NULL) {
        // �����޷�����㹻���ڴ�, Ҳ��Ӧ��ֹͣ����������
        return IMM_UNIT;
    }

    // ֻ�в����ɹ��Ÿ��µĶѴ�С
    heap->last_node->next = new_node;
    heap->last_node = new_node;
    heap->total_size += new_node_size;

    return IMM_TRUE;
}
