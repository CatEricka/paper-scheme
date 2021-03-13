#include "base-scheme/object.h"


/**
    ������� API
******************************************************************************/
/**
 * �����������Ĵ�С
 * �÷�: aligned_object_size(object_size(value_field))
 * ����һ����С, ���ض��뵽 1<<ALIGN_BITS �Ĵ�С
 * @param un_aligned_object_size
 * @return
 */
EXPORT_API size_t aligned_object_size(size_t un_aligned_object_size) {
    size_t align = un_aligned_object_size & (size_t) ALIGN_MASK;
    return un_aligned_object_size + (align == 0 ? 0 : ALIGN_SIZE);
}


/**
    malloc & free ��װ
******************************************************************************/

/**
 * malloc() �ķ�װ
 * @param size �ֽ���
 * @return ������ڴ��, Ϊ�������ʧ��
 */
EXPORT_API void *raw_alloc(size_t size) {
    void *obj = malloc(size);
    memset(obj, 0, size);
    // ��ַ���뵽 8bytes
    assert_aligned_ptr_check(obj);
    return obj;
}

/**
 * free() �ķ�װ
 * @param obj raw_alloc() ������ڴ�
 */
EXPORT_API void raw_free(void *obj) {
    assert_aligned_ptr_check(obj);
    free(obj);
}