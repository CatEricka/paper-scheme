#ifndef BASE_SCHEME_HEAP_H
#define BASE_SCHEME_HEAP_H
#pragma once


/**
 * heap.h heap.c
 * �����ѽṹ����
 * �������ݼ� test/test_cases/gc_test.h
 */


#include <paper-scheme/object.h>


/**
                                   �ѽṹ
******************************************************************************/

/**
 * �ѽڵ�
 */
typedef struct scheme_heap_node_t {
    // ��ǰ���С
    size_t chunk_size;
    // ��һ�ڵ�
    struct scheme_heap_node_t *next;
    //��ǰ�ڴ����ʼ���е�ַ
    char *free_ptr;
    //��ǰ���ڴ�ָ��, ע���ڴ����
    char *data;
} *heap_node_t;

/**
 * �ѽṹͷ�ڵ�
 */
typedef struct scheme_heap_t {
    // ��ʼ��С
    size_t init_size;
    // �ܴ�С
    size_t total_size;
    // ����С
    size_t max_size;
    // ��������
    size_t growth_scale;
    // �ѿ�ͷ�ڵ�
    heap_node_t first_node;
    // ���ڵ�
    heap_node_t last_node;
} *heap_t;

/**
 * ��ʼ���ѽṹ
 * @param init_size ��ʼ���С (bytes), ������ aligned_size() �����Ĵ�С
 * @param growth_scale ÿ�ζ�����ʱ, �µĶ�����һ�η���Ѵ�С�Ķ��ٱ�
 * @param max_size ���Ѵ�С, ������� init_size
 * @return malloc ʧ��ʱ���� NULL
 */
EXPORT_API OUT NULLABLE heap_t heap_make(IN size_t init_size, IN size_t growth_scale, IN size_t max_size);

/**
 * �ͷŶѽṹ
 * @param heap
 */
EXPORT_API void heap_destroy(IN NOTNULL heap_t heap);

/**
 * ����Ѵ�С
 * @param heap
 * @return <li>IMM_FALSE: �ﵽ max_size;</li><li>IMM_TRUE: �����ɹ�</li><li>IMM_UNIT: ϵͳ�ڴ治���ѽṹ�쳣</li>
 */
EXPORT_API OUT CHECKED object heap_grow(REF NOTNULL heap_t heap);


#endif //BASE_SCHEME_HEAP_H
