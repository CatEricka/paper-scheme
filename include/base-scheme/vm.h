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
 * @return
 */
EXPORT_API object i64_make(context_t context, int64_t v);

/**
 * 构造 doublenum 类型对象
 * @param context
 * @param v
 * @return
 */
EXPORT_API object doublenum_make(context_t context, int64_t v);

/**
 * 构造 pair 类型对象
 * @param context
 * @param v
 * @return
 */
EXPORT_API object pair_make(context_t context, object car, object cdr);

#endif //BASE_SCHEME_VM_H
