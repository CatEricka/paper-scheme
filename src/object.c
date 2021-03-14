#include "base-scheme/object.h"


/**
                               ������� API
******************************************************************************/
/**
 * �����������Ĵ�С
 * �÷�: aligned_size(object_size(value_field))
 * ����һ����С, ���ض��뵽 1<<ALIGN_BITS �Ĵ�С
 * @param unaligned_size
 * @return
 */
EXPORT_API OUT size_t aligned_size(IN size_t unaligned_size) {
    size_t need_aligned = unaligned_size & (size_t) ALIGN_MASK;
    size_t real_alloc_size = unaligned_size & (~((size_t) ALIGN_MASK));
    if (need_aligned) {
        // �����Ҫ����, ����λһ�� ALIGN_SIZE
        real_alloc_size += ALIGN_SIZE;
    }

    return real_alloc_size;
}

/**
 * ����ʱ��������С, ��λ bytes, �ṩ�� gc ʹ��
 * @param object NOTNULL ���������������ָ��
 * @return ��������Ƿ�, ���� 0
 */
EXPORT_API OUT OUT size_t object_size_runtime(REF NOTNULL object obj) {
    assert(obj != NULL);
    assert(is_object(obj));

    if (is_imm(obj)) {
        // ��Ӧ�ö�������ִ���������
        assert(!is_imm(obj));
        return 0;
    }

#define size_helper(value_field, flexible_array_size) \
    (object_size(value_field) + (flexible_array_size) + (obj)->padding_size)

    if (is_i64_real(obj)) {
        return size_helper(i64, 0);
    } else if (is_doublenum(obj)) {
        return size_helper(doublenum, 0);
    } else if (is_pair(obj)) {
        return size_helper(pair, 0);
    } else if (is_symbol(obj)) {
        return size_helper(symbol, sizeof(char) * obj->value.symbol.len);
    } else if (is_string(obj)) {
        return size_helper(string, sizeof(char) * obj->value.string.len);
    } else if (is_vector(obj)) {
        return size_helper(vector, sizeof(object) * obj->value.vector.len);
    }
        // TODO ��ʵ�ֵ����ͼǵ��޸�����

#define UNKNOWN_OBJECT_TYPE
    // δ֪������
    assert(UNKNOWN_OBJECT_TYPE
                   0);
#undef UNKNOWN_OBJECT_TYPE
#undef size_helper
    return 0;
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
    // �����ڴ�
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

    if (is_object(i64)) {
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
EXPORT_API OUT int is_i64(REF NULLABLE object i64) {
    if (is_i64_imm(i64)) return 1;
    else if (is_object(i64)) return i64->type == OBJ_I64;
    else return 0;
}