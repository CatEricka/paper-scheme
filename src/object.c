#include "base-scheme/object.h"


/**
                               对象操作 API
******************************************************************************/
/**
 * 计算对齐后对象的大小
 * 用法: aligned_size(object_size(value_field))
 * 给定一个大小, 返回对齐到 1<<ALIGN_BITS 的大小
 * @param unaligned_size
 * @return
 */
EXPORT_API OUT size_t aligned_size(IN size_t unaligned_size) {
    size_t need_aligned = unaligned_size & (size_t) ALIGN_MASK;
    size_t real_alloc_size = unaligned_size & (~((size_t) ALIGN_MASK));
    if (need_aligned) {
        // 如果需要对齐, 向大进位一个 ALIGN_SIZE
        real_alloc_size += ALIGN_SIZE;
    }

    return real_alloc_size;
}

/**
 * 运行时计算对象大小, 单位 bytes, 提供给 gc 使用
 * @param object NOTNULL 不能是立即数或空指针
 * @return 如果参数非法, 返回 0
 */
EXPORT_API OUT OUT size_t object_size_runtime(REF NOTNULL object obj) {
    assert(obj != NULL);
    assert(is_object(obj));

    if (is_imm(obj)) {
        // 不应该对立即数执行这个方法
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
        // TODO 新实现的类型记得修改这里

#define UNKNOWN_OBJECT_TYPE
    // 未知的类型
    assert(UNKNOWN_OBJECT_TYPE
                   0);
#undef UNKNOWN_OBJECT_TYPE
#undef size_helper
    return 0;
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
    // 清零内存
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

    if (is_object(i64)) {
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
EXPORT_API OUT int is_i64(REF NULLABLE object i64) {
    if (is_i64_imm(i64)) return 1;
    else if (is_object(i64)) return i64->type == OBJ_I64;
    else return 0;
}