#ifndef BASE_SCHEME_GC_H
#define BASE_SCHEME_GC_H
#pragma once


/**
 * gc.h gc.c
 * �����ѽṹ����, �Լ��ڶѽṹ�з���ͻ��ն���� API
 */


#include "base-scheme/util.h"
#include "base-scheme/object.h"
#include "base-scheme/heap.h"
#include "base-scheme/context.h"



/******************************************************************************
    �������� API
******************************************************************************/

/**
 * ��Ǵ�����
 * @param heap �ѽṹ
 * @return �����Ƿ�ɹ�
 */
static object gc_mark(context_t context) {
    // TODO gc_mark
    return IMM_TRUE;
}

/**
 * ����������������
 * @param heap
 * @return
 */
EXPORT_API object gc_collect(context_t context) {
    // TODO ʵ�� gc_collect
    gc_mark(context);
    // context->gc_sink = IMM_FALSE;
    return IMM_TRUE;
}

/**
 * ���ԴӶ��з����ڴ�, ���ᴥ�� gc, ʧ�ܷ��� NULL
 * @param heap �ѽṹ
 * @param size Ҫ����Ķ����С
 * @return ������ڴ��, Ϊ�������ʧ���ڴ治��
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


//    if (obj == IMM_NIL) {
//        // ����Ҳ���, obj Ϊ NULL, ������Ҫ gc ���
//        context->gc_sink = IMM_TRUE;
//    } else {
//        // ����ҵ�, obj Ϊ����Ķ���, gc���Ϊ false
//        context->gc_sink = IMM_FALSE;
//    }
    return obj;
}

/**
 * �Ӷ��з����ڴ�
 * @param heap �ѽṹ
 * @param size Ҫ����Ķ����С
 * @return ������ڴ��
 * IMM_FALSE: �ﵽ���Ѵ�С
 * IMM_NIL: δ�ﵽ���Ѵ�С, ����ϵͳ�ڴ治��
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

    if (heap_grow_result == IMM_FALSE) {
        return IMM_FALSE;
    } else {
        return IMM_NIL;
    }

}


#endif // BASE_SCHEME_GC_H