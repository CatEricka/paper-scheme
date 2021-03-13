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
EXPORT_API OUT size_t aligned_object_size(IN size_t un_aligned_object_size) {
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
EXPORT_API OUT NULLABLE void *raw_alloc(IN size_t size) {
    void *obj = malloc(size);
    notnull_or_return(obj, "raw_alloc() failed", NULL);
    memset(obj, 0, size);
    // 地址对齐到 8bytes
    assert_aligned_ptr_check(obj);
    return obj;
}

/**
 * free() 的封装
 * @param obj raw_alloc() 分配的内存
 */
EXPORT_API void raw_free(IN NOTNULL void *obj) {
    assert(obj != NULL);
    assert_aligned_ptr_check(obj);
    free(obj);
}


/**
    对象值操作
******************************************************************************/

/**
 * 获取 i64 对象的值
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
 * 判断 object 是否为 i64
 * @param i64
 * @return !0 -> true, 0 -> false
 */
EXPORT_API OUT int is_i64(REF NOTNULL object i64) {
    if (is_i64_imm(i64)) {
        // i64 立即数
        return 1;
    } else if (is_imm(i64)) {
        // 其它立即数
        return 0;
    } else {
        // 指针
        return i64->type == OBJ_I64;
    }
}