#include "base-scheme/vm.h"


/******************************************************************************
    对象构造 API
******************************************************************************/

/**
 * 构造 i64 类型对象, 不建议直接使用, 参见 i64_imm_auto_make()
 * @param heap
 * @param v i64 值
 * @return
 */
EXPORT_API OUT NOTNULL object i64_make(REF NOTNULL context_t context, IN int64_t v) {
    assert(context != NULL);

    // 此处要注意内存对齐
    size_t size = aligned_size(object_size(i64));
    object ret = gc_alloc(context, size);
    if (!is_object(ret)) {
        return ret;
    }
    memset(ret, 0, size);
    ret->type = OBJ_I64;
    ret->marked = 0;
    ret->value.i64 = v;
    return ret;
}

/**
 * 构造 i64 类型对象, 如果值范围属于 [- 2^(63-1), 2^(63-1)-1] 则构造立即数
 * @param heap
 * @param v i64 值
 * @return object 或立即数
 */
EXPORT_API OUT NOTNULL object i64_imm_auto_make(REF NOTNULL context_t context, IN int64_t v) {
    if (v >= I64_IMM_MIN && v <= I64_IMM_MAX) {
        return i64_imm_make(v);
    } else {
        return i64_make(context, v);
    }
}

/**
 * 构造 doublenum 类型对象
 * @param context
 * @param v
 * @return
 */
EXPORT_API OUT NOTNULL object doublenum_make(REF NOTNULL context_t context, int64_t v) {
    assert(context != NULL);

    size_t size = aligned_size(object_size(doublenum));
    object ret = gc_alloc(context, size);
    if (!is_object(ret)) {
        return ret;
    }
    memset(ret, 0, size);
    ret->type = OBJ_D64;
    ret->marked = 0;
    ret->value.i64 = v;
    return ret;
}

/**
 * 构造 pair 类型对象
 * @param context
 * @param v
 * @return
 */
EXPORT_API OUT NOTNULL object
pair_make(REF NOTNULL context_t context, REF NULLABLE object car, REF NULLABLE object cdr) {
    assert(context != NULL);

    size_t size = aligned_size(object_size(pair));
    object ret = gc_alloc(context, size);
    if (!is_object(ret)) {
        return ret;
    }
    memset(ret, 0, size);
    ret->type = OBJ_PAIR;
    ret->marked = 0;
    ret->value.pair.car = car;
    ret->value.pair.cdr = cdr;
    return ret;
}