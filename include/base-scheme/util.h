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

#define ___STATIC_ASSERT(COND, MSG) typedef char ___static_assertion_##MSG[(!!(COND))*2-1]
#define COMPILE_TIME_ASSERT3(X, L) ___STATIC_ASSERT(X,static_assertion_at_line_##L)
#define COMPILE_TIME_ASSERT2(X, L) COMPILE_TIME_ASSERT3(X,L)
/**
 * 静态断言
 */
#define COMPILE_TIME_ASSERT(X)    COMPILE_TIME_ASSERT2(X,__LINE__)

/**
 * 导出的API标记
 */
#define EXPORT_API

#ifdef DEBUG_ONLY
/**
 * 检查ptr是否为空指针
 * 如果为空, 输出 message 并返回 ret
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

#endif // BASE_SCHEME_ASSERT_HELPER_H