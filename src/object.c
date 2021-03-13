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
EXPORT_API OUT size_t aligned_object_size(IN size_t un_aligned_object_size) {
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
EXPORT_API OUT NULLABLE void *raw_alloc(IN size_t size) {
    void *obj = malloc(size);
    notnull_or_return(obj, "raw_alloc() failed", NULL);
    memset(obj, 0, size);
    // ��ַ���뵽 8bytes
    assert_aligned_ptr_check(obj);
    return obj;
}

/**
 * free() �ķ�װ
 * @param obj raw_alloc() ������ڴ�
 */
EXPORT_API void raw_free(IN NOTNULL void *obj) {
    assert(obj != NULL);
    assert_aligned_ptr_check(obj);
    free(obj);
}


/**
    ����ֵ����
******************************************************************************/

/**
 * ��ȡ i64 �����ֵ
 * @param i64
 * @return
 */
EXPORT_API OUT int64_t i64_getvalue(REF NOTNULL object i64) {
    assert(i64 != NULL);

    if (is_pointer(i64)) {
        return i64->value.i64;
    } else {
        return (ptr_to_intptr(i64) >> I64_EXTENDED_BITS); // NOLINT(hicpp-signed-bitwise)
    }
}

/**
 * �ж� object �Ƿ�Ϊ i64
 * @param i64
 * @return !0 -> true, 0 -> false
 */
EXPORT_API OUT int is_i64(REF NOTNULL object i64) {
    if (is_i64_imm(i64)) {
        // i64 ������
        return 1;
    } else if (is_imm(i64)) {
        // ����������
        return 0;
    } else {
        // ָ��
        return i64->type == OBJ_I64;
    }
}