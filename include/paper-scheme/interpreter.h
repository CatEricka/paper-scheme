#ifndef PAPER_SCHEME_INTERPRETER_H
#define PAPER_SCHEME_INTERPRETER_H
#pragma once


/**
 * interpreter.h interpreter.c
 * 包含解释器元循环实现和 API
 *
 * 该解释器实际为 AST Walker, 通过 opcodes dispatch 循环对 sexp(pair) 结构进行 EVAL-APPLY 元循环
 *
 * 性能上存在优化的余地, 但是最优先的不是性能而是代码的可读性与实验特性添加的便利性
 */


#include <paper-scheme/runtime.h>
#include <paper-scheme/opcodes.h>
#include <paper-scheme/token.h>



/******************************************************************************
                                  常用常量
******************************************************************************/
#define ZERO    i64_imm_make(0)
#define ONE     i64_imm_make(1)
#define TWO     i64_imm_make(2)
#define THREE   i64_imm_make(3)


/******************************************************************************
                                解释器初始化
******************************************************************************/
#define assert_init_done(context) assert((context)->init_done)

/**
 * 创建解释器
 * @param heap_init_size
 * @param heap_growth_scale
 * @param heap_max_size
 * @return
 */
EXPORT_API context_t interpreter_create(size_t heap_init_size, size_t heap_growth_scale, size_t heap_max_size);

/**
 * 析构解释器
 * @param context
 */
EXPORT_API void interpreter_destory(context_t context);

/******************************************************************************
                         global_symbol_table 操作
******************************************************************************/
/**
 * 向全局符号表添加 symbol 并返回这个 symbol
 * @param context
 * @param symbol symbol 对象
 */
EXPORT_API OUT NOTNULL GC void
global_symbol_add_from_symbol_obj(REF NOTNULL context_t context, REF NOTNULL object symbol);

/**
 * 从全局符号表中查找是否存在此符号
 * @param context
 * @param symbol
 * @return 存在返回 IMM_TRUE, 否则返回 IMM_UNIT
 */
EXPORT_API OUT NOTNULL GC object
global_symbol_found(REF NOTNULL context_t context, REF NOTNULL object symbol);

/**
 * 返回全局符号表全部内容
 * @param context
 * @return vector, vector 中可能存在 IMM_UNIT
 */
EXPORT_API OUT NOTNULL GC object
global_symbol_all_symbol(REF NOTNULL context_t context);

/**
 * 构造 symbol 类型对象
 * <p>symbol_len() 运算结果不包括 '\0', 但是 object->symbol.len 包括 '\0', 这是为了方便运行时计算对象大小</p>
 * <p>该方法会把 symbol 添加进全局符号表</p>
 * @param context
 * @param cstr C字符串, '\0'结尾
 * @return
 */
EXPORT_API OUT NOTNULL GC object
symbol_make_from_cstr_op(REF NOTNULL context_t context, COPY char *cstr);

/**
 * string 转 symbol
 * <p>该方法会把 symbol 添加进全局符号表</p>
 * @param context
 * @param string
 * @return
 */
EXPORT_API OUT NOTNULL GC object
string_to_symbol_op(REF NOTNULL context_t context, NOTNULL COPY object str);

/**
 * string_buffer 转换为 symbol, 深拷贝
 * <p>该方法会把 symbol 添加进全局符号表</p>
 * @param context
 * @param str_buffer string_buffer
 * @return symbol
 */
EXPORT_API OUT NOTNULL GC object
string_buffer_to_symbol_op(REF NOTNULL context_t context, NOTNULL COPY object str_buffer);


/******************************************************************************
                          global_environment 操作
******************************************************************************/



/******************************************************************************
                               current_env 操作
******************************************************************************/



/******************************************************************************
                                    TODO 元循环
******************************************************************************/
EXPORT_API uint32_t eval_apply_loop(context_t context, enum opcode_e opcode);


#endif //PAPER_SCHEME_INTERPRETER_H
