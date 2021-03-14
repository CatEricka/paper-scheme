#ifndef BASE_SCHEME_ASSERT_HELPER_H
#define BASE_SCHEME_ASSERT_HELPER_H
#pragma once


/**
 * util.h
 * 常用工具与系统头文件定义
 */


#define __STDC_FORMAT_MACROS

#include <stdint.h>
#include <inttypes.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


/**
                                静态断言
******************************************************************************/
#define ___STATIC_ASSERT(COND, MSG) typedef char ___static_assertion_##MSG[(!!(COND))*2-1]
#define ___COMPILE_TIME_ASSERT3(X, L) ___STATIC_ASSERT(X,static_assertion_at_line_##L)
#define ___COMPILE_TIME_ASSERT2(X, L) ___COMPILE_TIME_ASSERT3(X,L)
// 编译时断言
#define compile_time_assert(X)    ___COMPILE_TIME_ASSERT2(X,__LINE__)

/**
                                方法标记
******************************************************************************/
// 公开的 API
#define EXPORT_API
// 生命周期标记, 传入所有权
#define IN
// 生命周期标记, 传出所有权 (gc 分配的对象也属于 context->heap)
#define OUT
// 生命周期标记, IN & OUT
#define REF
// 返回值标记, 需要检查函数执行结果
#define CHECKED
// 参数标记, 非空
#define NOTNULL
// 参数标记, 可能为空
#define NULLABLE


/**
                                 辅助宏
******************************************************************************/
#ifdef DEBUG_ONLY
/**
 * <p>检查ptr是否为空指针<p>
 * <p>如果为空, 输出 message 并返回 ret</p>
 * <p>!会修改控制流<p>
 */
# define notnull_or_return(ptr, message, ret) \
    do { \
        if ((ptr) == NULL) { \
            fprintf(stderr, "null pointer: %s\n", message); \
            return ret; \
        } \
    } while(0)
#else
/**
 * 检查ptr是否为空指针
 * 如果为空, 返回 ret
 */
# define notnull_or_return(ptr, message, ret) \
    do { \
        if (ptr == NULL) { \
            return ret; \
        } \
    } while(0)
#endif // DEBUG_ONLY


#define ptr_to_uintptr(x) ((uintptr_t)(x))
#define ptr_to_intptr(x) ((intptr_t)(x))

#define is_null(x) ((x) == NULL)


/**
                                辅助方法
******************************************************************************/

/**
 * 保证对 int64_t 类型实现算数右移
 * @param x 被算数右移值
 * @param n 右移位数
 * @return
 */
int64_t i64_arithmetic_right_shift(int64_t x, size_t n);


/**
                             exit() 返回值定义
******************************************************************************/
// 未知错误
#define EXIT_FAILURE_UNKNOWN (-2)
// 内存不足: 堆增长超过 max_size
#define EXIT_FAILURE_OUT_OF_MEMORY (-3)
// malloc() 函数失败
#define EXIT_FAILURE_MALLOC_FAILED (-4)

#endif // BASE_SCHEME_ASSERT_HELPER_H