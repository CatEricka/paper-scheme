#ifndef BASE_SCHEME_EVAL_H
#define BASE_SCHEME_EVAL_H
#pragma once


/**
 * interpreter.h interpreter.c
 * 包含解释器上下文结构定义和虚拟机运行相关的操作, 以及各种值类型构造函数
 *
 * 测试内容见 test/test_cases/repl_test.h test/test_cases/value_test.h
 *
 * 该解释器实际为 AST Walker, 通过 opcodes dispatch 循环对 sexp(pair) 结构进行 EVAL-APPLY 循环
 *
 * 性能上存在优化的余地, 但是最优先的不是性能而是代码的可读性与实验特性添加的便利性
 */


#include <paper-scheme/gc.h>
#include <paper-scheme/opcodes.h>


/******************************************************************************
                            解释器初始化与解释器 API
******************************************************************************/
EXPORT_API context_t interpreter_create(size_t heap_init_size, size_t heap_growth_scale, size_t heap_max_size);



/******************************************************************************
                                对象构造 API
******************************************************************************/

/**
 * 构造 i64 类型对象, 不建议直接使用, 因为要考虑到复杂的边界条件, 参见 i64_make_op()
 * @param heap
 * @param v i64 值
 * @return
 */
EXPORT_API OUT NOTNULL GC object i64_make_real_object_op(REF NOTNULL context_t context, IN int64_t v);

/**
 * 构造 i64 类型对象, 如果值范围属于 [- 2^(63-1), 2^(63-1)-1] 则构造立即数
 * @param heap
 * @param v i64 值
 * @return object 或立即数
 */
EXPORT_API OUT NOTNULL GC object i64_make_op(REF NOTNULL context_t context, IN int64_t v);

/**
 * 构造 doublenum 类型对象
 * @param context
 * @param v
 * @return
 */
EXPORT_API OUT NOTNULL GC object doublenum_make_op(REF NOTNULL context_t context, double v);

/**
 * 构造 pair 类型对象
 * @param context
 * @param v
 * @return
 */
EXPORT_API OUT NOTNULL GC object
pair_make_op(REF NOTNULL context_t context, REF NULLABLE object car, REF NULLABLE object cdr);

/**
 * 构造 symbol 类型对象
 * <p>symbol_len() 运算结果不包括 '\0', 但是 object->symbol.len 包括 '\0', 这是为了方便运行时计算对象大小</p>
 * @param context
 * @param cstr C字符串, '\0'结尾
 * @return
 */
EXPORT_API OUT NOTNULL GC object
symbol_make_from_cstr_op(REF NOTNULL context_t context, char *cstr);

/**
 * 构造 string 类型对象
 * <p>string_len() 运算结果不包括 '\0',
 * 但是 object->string.len 包括 '\0', 这是为了方便运行时计算对象大小</p>
 * @param context
 * @param cstr C字符串, '\0'结尾
 * @return
 */
EXPORT_API OUT NOTNULL GC object
string_make_from_cstr_op(REF NOTNULL context_t context, char *cstr);

/**
 * 构造 vector 类型对象, 填充 Unit, 即 '()
 * @param context
 * @param vector_len vector 大小
 * @return
 */
EXPORT_API OUT NOTNULL GC object
vector_make_op(REF NOTNULL context_t context, IN size_t vector_len);


#endif //BASE_SCHEME_EVAL_H
