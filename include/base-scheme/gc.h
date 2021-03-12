#ifndef BASE_SCHEME_GC_H
#define BASE_SCHEME_GC_H
#pragma once


/**
 * gc.h gc.c
 * �����ѽṹ����, �Լ��ڶѽṹ�з���ͻ��ն���� API
 */

#include <stdlib.h>
#include <string.h>


#include "base-scheme/object.h"


/******************************************************************************
    �ѽṹ
******************************************************************************/
/**
 * �ѽṹ
 */
typedef struct scheme_heap_t {
    //�ܴ�С
    size_t size;
    //����С
    size_t max_size;
    //��ǰ���С
    size_t chunk_size;
    //��һ���ѿ�
    struct scheme_heap_t *next;
    /* ע������Ҫ��֤�ڴ���� */
    //��ǰ���ڴ�ָ��
    char *data;
} *heap_t;


/******************************************************************************
    �������� API
******************************************************************************/
/**
 * malloc() �ķ�װ
 * @param size �ֽ���
 * @return ������ڴ��, Ϊ�������ʧ��
 */
EXPORT_API void *raw_alloc(size_t size) {
    void *mem = malloc(size);
    assert(((((uintptr_t) obj) & ((uintptr_t) 3)) == 0));
    return mem;
}
/**
 * free() �ķ�װ
 * @param obj raw_alloc() ������ڴ�
 */
EXPORT_API void raw_free(void *obj) {
    assert(((((uintptr_t) obj) & ((uintptr_t) 3)) == 0));
    free(obj);
}

/**
 * �Ӷ��з����ڴ�
 * @param heap �ѽṹ
 * @param size Ҫ����Ķ����С
 * @return ������ڴ��, Ϊ�������ʧ���ڴ治��
 */
static object gc_alloc(struct scheme_heap_t *heap, size_t size) {
    // TODO ����ʵ���йܵ��ڴ��Ϸ���
    return (object) raw_alloc(size);
}

/**
 * ��Ǵ�����
 * @param heap �ѽṹ
 * @return �����Ƿ�ɹ�
 */
static object gc_mark(struct scheme_heap_t *heap) {
    // TODO gc_mark
    return 0;
}

/**
 * ����������������
 * @param heap
 * @return
 */
EXPORT_API object gc_collect(struct scheme_heap_t *heap) {
    // TODO ʵ�� gc_collect
    gc_mark(heap);
    return 0;
}


/**
 * ��ʼ���ѽṹ
 * @param init_size ��ʼ���С
 * @param grown_scale ÿ�ζ�����ʱ, �µĶ�����һ�η���Ѵ�С�Ķ��ٱ�
 * @param max_size ���Ѵ�С
 * @return
 */
EXPORT_API heap_t init_scheme_heap(size_t init_size, size_t grown_scale, size_t max_size) {
    // TODO д����; ʵ���ڴ�����
    // TODO grown_scale Ҫ����ĳ���ط�
    // �ǵ��ͷ� heap_t
    heap_t new_heap = raw_alloc(sizeof(struct scheme_heap_t));
    new_heap->next = NULL;
    new_heap->size = init_size;
    new_heap->chunk_size = init_size;   // ��ʼ��ʱ��, ��һ�� heap ���С���ܵ� heap ��С��ͬ
    new_heap->max_size = max_size;
    // �ǵ�ʵ�� heap_t->data
    new_heap->data = raw_alloc(init_size);

    return new_heap;
}

// �ͷŶѽṹ
EXPORT_API void destroy_scheme_heap(heap_t heap) {
    // TODO �������ٶѽṹ
    heap_t next;
    while (heap != NULL) {
        next = heap->next;
        // �ͷ� heap_t->data
        raw_free(heap->data);
        // �ͷ� heap_t
        raw_free(heap);
        heap = next;
    }
}


/******************************************************************************
    ������ API
******************************************************************************/
/**
 * ���� i64 ���͵Ķ���, ���뵽 sizeof void*, ��Ҫֱ�ӵ���
 * @param heap
 * @return
 */
static object alloc_i64(struct scheme_heap_t *heap) {
    // TODO ʵ���ڴ����
    object ret = (object) gc_alloc(heap, object_size(i64));
    memset(ret, 0, object_size(i64));
    return ret;
}
/**
 * ���� i64 ���Ͷ���
 * @param heap
 * @param v i64 ֵ
 * @return
 */
EXPORT_API object mk_i64(struct scheme_heap_t *heap, int64_t v) {
    object ret = alloc_i64(heap);
    ret->type = OBJ_I64;
    ret->value.i64 = v;
    return ret;
}


#endif // _BASE_SCHEME_GC_HEADER_