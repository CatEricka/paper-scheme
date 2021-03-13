#include "base-scheme/vm.h"


/******************************************************************************
    对象构造 API
******************************************************************************/

/**
 * 构造 i64 类型对象
 * @param heap
 * @param v i64 值
 * @return
 */
EXPORT_API object i64_make(context_t context, int64_t v) {
    assert(context != NULL);

    // 此处要注意内存对齐
    size_t size = aligned_object_size(object_size(i64));
    object ret = gc_alloc(context, size);
    if (!is_pointer(ret)) {
        return ret;
    }
    memset(ret, 0, size);
    ret->type = OBJ_I64;
    ret->value.i64 = v;
    return ret;
}

/**
 * 构造 doublenum 类型对象
 * @param context
 * @param v
 * @return
 */
EXPORT_API object doublenum_make(context_t context, int64_t v) {
    assert(context != NULL);

    size_t size = aligned_object_size(object_size(doublenum));
    object ret = gc_alloc(context, size);
    if (!is_pointer(ret)) {
        return ret;
    }
    memset(ret, 0, size);
    ret->type = OBJ_I64;
    ret->value.i64 = v;
    return ret;
}

/**
 * 构造 pair 类型对象
 * @param context
 * @param v
 * @return
 */
EXPORT_API object pair_make(context_t context, object car, object cdr) {
    assert(context != NULL);

    size_t size = aligned_object_size(object_size(pair));
    object ret = gc_alloc(context, size);
    if (!is_pointer(ret)) {
        return ret;
    }
    memset(ret, 0, size);
    ret->type = OBJ_PAIR;
    ret->value.pair.car = car;
    ret->value.pair.cdr = cdr;
    return ret;
}