#ifndef BASE_SCHEME_GC_H
#define BASE_SCHEME_GC_H
#pragma once


/**
 * gc.h gc.c
 * 包含堆结构定义, 以及在堆结构中分配和回收对象的 API
 */


#include "base-scheme/util.h"
#include "base-scheme/object.h"
#include "base-scheme/heap.h"
#include "base-scheme/context.h"


/**
    垃圾回收 API
******************************************************************************/

/**
 * 启动回收垃圾回收
 * @param context
 * @return <li>IMM_TRUE: 运行成功</li><li>IMM_FALSE: 运行失败</li>
 */
EXPORT_API object gc_collect(context_t context);

/**
 * 尝试从堆中分配内存, 不会触发 gc, 失败返回 NULL
 * @param heap 堆结构
 * @param size 要分配的对象大小
 * @return <li>NULL: 找不到足够大的空闲控件</li>
 */
EXPORT_API object gc_try_alloc(context_t context, size_t size);

/**
 * 从堆中分配内存
 * <p>注意, 到了底层与托管内存的分界线了</p>
 * <p>!此函数失败会直接结束进程</p>
 * @param heap 堆结构
 * @param size 要分配的对象大小
 * @return
 * <li>exit(EXIT_FAILURE_OUT_OF_MEMORY): 达到最大堆大小</li>
 * <li>exit(EXIT_FAILURE_MALLOC_FAILED): 未达到最大堆大小, 但是系统内存不足</li>
 */
EXPORT_API object gc_alloc(context_t context, size_t size);


#endif // BASE_SCHEME_GC_H