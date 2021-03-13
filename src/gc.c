#include "base-scheme/gc.h"


/**
    �������� API
******************************************************************************/

/**
 * �ڲ����� ��Ǵ�����
 * @param context
 * @return <li>IMM_TRUE: ���гɹ�</li><li>IMM_FALSE: ����ʧ��</li>
 */
static object gc_mark(context_t context) {
    // TODO gc_mark
    return IMM_TRUE;
}

/**
 * ����������������
 * @param context
 * @return <li>IMM_TRUE: ���гɹ�</li><li>IMM_FALSE: ����ʧ��</li>
 */
EXPORT_API object gc_collect(context_t context) {
    // TODO ʵ�� gc_collect
    gc_mark(context);
    return IMM_TRUE;
}

/**
 * ���ԴӶ��з����ڴ�, ���ᴥ�� gc, ʧ�ܷ��� NULL
 * @param heap �ѽṹ
 * @param size Ҫ����Ķ����С
 * @return <li>NULL: �Ҳ����㹻��Ŀ��пؼ�</li>
 */
EXPORT_API object gc_try_alloc(context_t context, size_t size) {
    assert(context != NULL);
    assert(context->heap != NULL);
    assert_aligned_size_check(size);

    heap_t heap = context->heap;
    object obj = NULL;

    // ����������
    for (heap_node_t node = heap->first_node; node != NULL; node = node->next) {
        size_t used_space = node->free_ptr - node->data;
        size_t free_space = node->chunk_size - used_space;
        if (free_space >= size) {
            // �ҵ�����λ��
            obj = (object) node->free_ptr;
            // ���� free_ptr
            node->free_ptr += size;
            break;
        }
    }

    // �Ҳ����Ļ��᷵�� NULL
    return obj;
}

/**
 * �Ӷ��з����ڴ�
 * <p>ע��, ���˵ײ����й��ڴ�ķֽ�����</p>
 * <p>!�˺���ʧ�ܻ�ֱ�ӽ�������</p>
 * @param heap �ѽṹ
 * @param size Ҫ����Ķ����С
 * @return
 * <li>exit(EXIT_FAILURE_OUT_OF_MEMORY): �ﵽ���Ѵ�С</li>
 * <li>exit(EXIT_FAILURE_MALLOC_FAILED): δ�ﵽ���Ѵ�С, ����ϵͳ�ڴ治��</li>
 */
EXPORT_API object gc_alloc(context_t context, size_t size) {
    assert(context != NULL);
    assert(context->heap != NULL);
    assert_aligned_size_check(size);

    // 1. ��һ�γ��Է���
    object obj = gc_try_alloc(context, size);
    // ����ɹ�, ����
    if (obj != NULL) {
        return obj;
    }

    // 2. obj Ϊ��, ����Ҫ gc, ����һ�� gc
    gc_collect(context);

    // 3. gc ��ڶ��γ��Է���
    obj = gc_try_alloc(context, size);
    // ����ɹ�, ����
    if (obj != NULL) {
        return obj;
    }

    // 4. �ѿռ䲻��, ���������ѿռ�����
    object heap_grow_result = heap_grow(context->heap);
    obj = gc_try_alloc(context, size);
    // ��γ���, ֱ������ʧ�ܻ����ɹ�Ϊֹ
    while ((obj == NULL) && (heap_grow_result == IMM_TRUE)) {
        heap_grow_result = heap_grow(context->heap);
        obj = gc_try_alloc(context, size);
    }

    // 5. ���ս׶�.
    // ������ַ���ɹ�, ���� obj
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