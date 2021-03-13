#include "base-scheme/object.h"


/**
    对象操作 API
******************************************************************************/
/**
 * 计算对齐后对象的大小
 * 用法: aligned_object_size(object_size(value_field))
 * 给定一个大小, 返回对齐到 1<<ALIGN_BITS 的大小
 * @param un_aligned_object_size
 * @return
 */
EXPORT_API size_t aligned_object_size(size_t un_aligned_object_size) {
    size_t align = un_aligned_object_size & (size_t) ALIGN_MASK;
    return un_aligned_object_size + (align == 0 ? 0 : ALIGN_SIZE);
}


/**
    malloc & free 封装
******************************************************************************/

/**
 * malloc() 的封装
 * @param size 字节数
 * @return 分配的内存块, 为空则分配失败
 */
EXPORT_API void *raw_alloc(size_t size) {
    void *obj = malloc(size);
    memset(obj, 0, size);
    // 地址对齐到 8bytes
    assert_aligned_ptr_check(obj);
    return obj;
}

/**
 * free() 的封装
 * @param obj raw_alloc() 分配的内存
 */
EXPORT_API void raw_free(void *obj) {
    assert_aligned_ptr_check(obj);
    free(obj);
}