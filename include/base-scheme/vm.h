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
EXPORT_API OUT NOTNULL object i64_make(REF NOTNULL context_t context, IN int64_t v);

/**
 * 构造 i64 类型对象, 如果值范围属于 [- 2^(63-1), 2^(63-1)-1] 则构造立即数
 * @param heap
 * @param v i64 值
 * @return object 或立即数
 */
EXPORT_API OUT NOTNULL object i64_imm_auto_make(REF NOTNULL context_t context, IN int64_t v);

/**
 * 构造 doublenum 类型对象
 * @param context
 * @param v
 * @return
 */
EXPORT_API OUT NOTNULL object doublenum_make(REF NOTNULL context_t context, int64_t v);

/**
 * 构造 pair 类型对象
 * @param context
 * @param v
 * @return
 */
EXPORT_API OUT NOTNULL object
pair_make(REF NOTNULL context_t context, REF NULLABLE object car, REF NULLABLE object cdr);


#endif //BASE_SCHEME_VM_H
