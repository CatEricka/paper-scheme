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
#define COMPILE_TIME_ASSERT3(X, L) ___STATIC_ASSERT(X,static_assertion_at_line_##L)
#define COMPILE_TIME_ASSERT2(X, L) COMPILE_TIME_ASSERT3(X,L)
// 静态断言
#define COMPILE_TIME_ASSERT(X)    COMPILE_TIME_ASSERT2(X,__LINE__)

/**
    导出的 API 标记
******************************************************************************/
#define EXPORT_API



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