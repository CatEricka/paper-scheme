#ifndef PAPER_SCHEME_OPCODES_H
#define PAPER_SCHEME_OPCODES_H
#pragma once

#include <paper-scheme/feature.h>

enum opcode_e {
    // op_exec_repl
            OP_LOAD = 0,
    OP_TOP_LEVEL_SETUP,
    OP_TOP_LEVEL,
    OP_READ_INTERNAL,
    OP_EVAL,
    OP_APPLY,
    OP_VALUE_PRINT,

    // op_exec_syntax
            OP_EXPAND_MACRO,
    OP_LAMBDA,
    OP_MAKE_CLOSURE,
    OP_QUOTE,
    OP_DEFINE,
    OP_DEFINE_PROCEDURE,
    OP_BEGIN,
    OP_IF,
    OP_SET,
    OP_LET,
    OP_LET_,
    OP_LEC_REC,
    OP_COND,
    OP_DELAY,
    OP_AND,
    OP_OR,
    OP_CASE,

    // op_exec_compute
            OP_EXP,
    OP_LOG,
    OP_SIN,
    OP_COS,
    OP_TAN,
    OP_ASIN,
    OP_ACOS,
    OP_ATAN,
    OP_SQRT,
    OP_EXPT,
    OP_FLOOR,
    OP_CEILING,
    OP_TRUNCATE,
    OP_ROUND,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_INT_DIV,
    OP_REM,
    OP_MOD,

    // op_exec_object_operation
            OP_VECTOR,

    // op_exec_lexical
            OP_READ_SEXP,
    OP_READ_LIST,
    OP_READ_DOT,
    OP_READ_QUOTE,
    OP_READ_UNQUOTE,
    OP_READ_QUASIQUOTE,
    OP_READ_QUASIQUOTE_VECTOR,
    OP_READ_UNQUOTE_SPLICING,
    OP_READ_VECTOR,
    OP_PRINT_OBJECT,
    OP_PRINT_LIST,
    OP_PRINT_VECTOR,

    // op_exec_predicate

    // op_exec_builtin_function
            OP_QUIT,
    OP_GC,
    OP_ERROR,
    OP_ERROR_PRINT_OBJECT,
    // TODO opcode
            MAX_OP_CODE,
};

#endif //PAPER_SCHEME_OPCODES_H