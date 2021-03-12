#ifndef BASE_SCHEME_VM_H
#define BASE_SCHEME_VM_H
#pragma once


/**
 * vm.h vm.c
 * 包含虚拟机上下文结构定义和虚拟机运行相关的操作, 以及各种值类型构造函数
 */


#include "base-scheme/util.h"
#include "base-scheme/object.h"
#include "base-scheme/heap.h"
#include "base-scheme/context.h"
#include "base-scheme/gc.h"




/******************************************************************************
    对象构造 API
******************************************************************************/
/**
 * 构造 i64 类型对象
 * @param heap
 * @param v i64 值
 * @return 如果分配失败, 返回 IMM_FALSE 或 IMM_NIL
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

#endif //BASE_SCHEME_VM_H
