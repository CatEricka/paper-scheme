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


#include <paper-scheme/gc.h>
#include <paper-scheme/opcodes.h>

#endif //PAPER_SCHEME_INTERPRETER_H
