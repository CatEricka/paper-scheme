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



/******************************************************************************
                                解释器初始化
******************************************************************************/
/**
 * 创建解释器
 * @param heap_init_size
 * @param heap_growth_scale
 * @param heap_max_size
 * @return
 */
EXPORT_API context_t interpreter_create(size_t heap_init_size, size_t heap_growth_scale, size_t heap_max_size);


/******************************************************************************
                         global_symbol_table 操作
******************************************************************************/



/******************************************************************************
                          global_environment 操作
******************************************************************************/



/******************************************************************************
                               current_env 操作
******************************************************************************/



#endif //PAPER_SCHEME_INTERPRETER_H
