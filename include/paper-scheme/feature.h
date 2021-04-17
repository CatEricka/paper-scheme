#ifndef BASE_SCHEME_FEATURE_H
#define BASE_SCHEME_FEATURE_H
#pragma once


/**
 * feature.h
 * 常用工具与系统头文件定义
 */


#ifndef __STDC_FORMAT_MACROS
# define __STDC_FORMAT_MACROS
#endif // __STDC_FORMAT_MACROS

#include <stdint.h>
#include <inttypes.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>



/**
                                调试定义
******************************************************************************/
// 使用带有 debug 信息的 gc
// DEBUG_ONLY 宏, 见 CMakeLists.txt 定义
#ifdef DEBUG_ONLY
# define USE_DEBUG_GC 1
#endif


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
// 生命周期标记, 函数会引用这个对象
#define REF
// 生命周期标记, 函数会执行拷贝操作
#define COPY
// 返回值标记, 需要检查函数执行结果
#define CHECKED
// 参数标记, 非空
#define NOTNULL
// 参数标记, 可能为空
#define NULLABLE
// 返回的参数一定为立即数
#define IMM

// 该函数可能触发 GC 导致对象移动, 调用该函数的函数需要将参数存入保护链
// 具有传染性
#define GC
// 弱引用标记
#define WEAK_REF


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
        if ((ptr) == NULL) { \
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
 * 随机数生成器的包装
 * 为 hash 值生成提供有限的随机性, [0, RAND_MAX]
 * @return uint32_t
 */
uint32_t rand_helper();

/**
                             exit() 返回值定义
******************************************************************************/
// 未知错误
#define EXIT_FAILURE_UNKNOWN            (-2)
// 内存不足: 堆增长超过 max_size
#define EXIT_FAILURE_OUT_OF_MEMORY      (-3)
// malloc() 函数失败
#define EXIT_FAILURE_MALLOC_FAILED      (-4)

/**
                         context->ret 返回值定义
******************************************************************************/
// 正常退出
#define NO_ERROR                        (0)
// 未知错误
#define UNKNOWN_ERROR                   (-5)
// (error) 未被捕获
#define ERROR_UNCAUGHT_ERROR            (-6)
// 括号不匹配
#define ERROR_PARENTHESES_NOT_MATCH     (-7)


/**
                                gc 特征
******************************************************************************/
// 初始标记栈深度
#define GC_MAX_MARK_STACK_DEEP 1024u

/**
                               解释器特征
******************************************************************************/
// 单个调用栈块最大深度, 超出这个长度会分配新的 stack 块
#define MAX_STACK_BLOCK_DEEP 1024
// 全局符号表初始大小
#define GLOBAL_SYMBOL_TABLE_INIT_SIZE 512
// 全局 environment 初始大小
#define GLOBAL_ENVIRONMENT_INIT_SIZE 512
// (load "") 函数最大嵌套深度
#define MAX_LOAD_FILE_DEEP 64
// 内部字符串缓冲大小
#define INTERNAL_STR_BUFFER_SIZE 2048

/**
                             基础对象特征
******************************************************************************/
#define DEFAULT_HASH_SET_MAP_LOAD_FACTOR 0.75
#define DEFAULT_HASH_SET_MAP_INIT_CAPACITY 16

/**
                                其它定义
******************************************************************************/
// string_buffer 默认初始大小
#define STRING_BUFFER_DEFAULT_INIT_SIZE 16
// string_buffer 默认增长大小
#define STRING_BUFFER_DEFAULT_GROWTH_SIZE 1024

// stack push 自动增长 默认增长大小
#define STACK_DEFAULT_GROWTH_SIZE 16

// 解释器内部格式化字符串缓冲长度
// 不要修改
#define __Format_buff_size__ 30

// repl 提示符
#define USER_OVERTURE "ps => "

/**
                                标准库
******************************************************************************/
#define PAPER_SCHEME_INIT_DEFAULT_FILENAME "ps_init.scm"

#endif // BASE_SCHEME_FEATURE_H