#ifndef BASE_SCHEME_GC_H
#define BASE_SCHEME_GC_H
#pragma once


/**
 * gc.h gc.c
 * 包含堆结构定义, 以及在堆结构中分配和回收对象的 API
 * 测试内容见 test/test_cases/gc_test.h
 */


#include <paper-scheme/feature.h>
#include <paper-scheme/object.h>
#include <paper-scheme/heap.h>
#include <paper-scheme/context.h>


/**
                               垃圾回收 API
******************************************************************************/

#define gc_collect_disable(context) ((context)->gc_collect_on = 0)
#define gc_collect_enable(context) ((context)->gc_collect_on = 1)

/**
 * 启动回收垃圾回收
 * <p>todo 注意! 所有 c 函数栈 上使用的被分配的对象都必须保存到保护链, 否则会因为对象移动破坏引用</p>
 * <p>详见 context.h: macro gc_var()</p>
 * <p>幸运的是, 保护链的生命周期与 c 函数栈生命周期相关</p>
 * <p>只要正确使用 gc_var 和 gc_preserve 就能保证引用的正确更新</p>
 * <p>注意所有会触发 gc 的函数 (带有 GC 标记的函数) 内的 object 参数都应该加入保护链</p>
 * <p>注意有些测试需要关闭 gc 才能保证测试正常进行, 参见 macro gc_collect_disable()</p>
 * @param context
 * @return <li>IMM_TRUE: 运行成功</li><li>IMM_FALSE: 运行失败</li>
 */
EXPORT_API CHECKED GC object gc_collect(REF NOTNULL context_t context);

/**
 * 尝试从堆中分配内存, 不会触发 gc, 失败返回 NULL
 * <p>不要直接使用</p>
 * @param heap 堆结构
 * @param size 要分配的对象大小
 * @return <li>NULL: 找不到足够大的空闲控件</li>
 */
EXPORT_API OUT CHECKED GC object gc_try_alloc(REF NOTNULL context_t context, IN size_t size);

/**
 * 从堆中分配内存
 * <p>不要直接使用</p>
 * <p>注意, 到了底层与托管内存的分界线了</p>
 * <p>!此函数失败会直接结束进程</p>
 * @param heap 堆结构
 * @param size 要分配的对象大小
 * @return
 * <li>exit(EXIT_FAILURE_OUT_OF_MEMORY): 达到最大堆大小</li>
 * <li>exit(EXIT_FAILURE_MALLOC_FAILED): 未达到最大堆大小, 但是系统内存不足</li>
 */
EXPORT_API OUT GC object gc_alloc(REF NOTNULL context_t context, IN size_t size);


#endif // BASE_SCHEME_GC_H