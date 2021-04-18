#ifndef PAPER_SCHEME_OPCODES_H
#define PAPER_SCHEME_OPCODES_H
#pragma once

#include <paper-scheme/feature.h>

enum opcode_e {
    // op_exec_repl
            OP_LOAD = 0,    // (load "source_name")
    OP_TOP_LEVEL_SETUP,     // load_file <-
    OP_TOP_LEVEL,
    OP_READ_INTERNAL,
    OP_VALUE_PRINT,
    OP_EVAL,                // eval debug 信息
    OP_EVAL_REAL,
    OP_EXPAND_MACRO,
    OP_EVAL_ARGS,
    OP_APPLY,               // apply debug 信息
    OP_APPLY_REAL,
    OP_GENSYM,              // (gensym)

    // op_exec_syntax
            OP_EVAL_EXPANDED_MACRO,
    OP_LAMBDA0,             // (lambda (...) begin...)
    OP_LAMBDA1,
    OP_MAKE_CLOSURE,        // (make-closure sexp [env])
    OP_GET_CLOSURE_CODE,    // (get-closure-code sexp)
    OP_QUOTE,               // (quote)
    OP_DEFINE0,             // (define (var args ...) begin...)
    OP_DEFINE1,             // or (define var sexp)
    OP_MACRO0,              // (macro (macro_name form) list...)
    OP_MACRO1,              // or (macro macro_name (lambda (form) list...)
    OP_BEGIN,               // (begin sexp ... return_value)
    OP_IF0,                 // (if cond true_sexp false_sexp)
    OP_IF1,
    OP_SET0,                // (set! var sexp)
    OP_SET1,
    OP_LET0,                // (let ((var1 sexp1) ...) begin...)
    OP_LET1,                // or (let name ((var1 sexp1) ...) begin...)
    OP_LET2,
    OP_LET_AST0,            // (let* ((var1 sexp1) ...) begin...)
    OP_LET_AST1,            // or (let* name ((var1 sexp1) ...) begin...)
    OP_LET_AST2,            // 后面的 sexp_n 能使用前面 var_n-1 的绑定
    OP_LET_REC0,            // letrec 类似 let*, 但是允许向前向后查找定义
    OP_LET_REC1,            // 为递归函数设计
    OP_LET_REC2,
    OP_COND0,               // (cond (cond1 begin...) (cond2 begin...) ... (else sexp_n))
    OP_COND1,               // or (cond (cond1 => (lambda (arg) ...)) ...)
    OP_AND0,                // (and sexp1 sexp2 ...) 短路求值
    OP_AND1,
    OP_OR0,                 // (or sepx1 sexp2 ...)  短路求值, 返回第一个非 #t 的值
    OP_OR1,
    OP_CASE0,               // (case test-expr case-clause ...)
    OP_CASE1,               // case-clause => [(datum ...) then-body ...]
    OP_CASE2,               //      | [else then-body ...]
    OP_DELAY,               // (delay expr) => promise
    OP_FORCE,               // (force promise) => value
    OP_FORCED_P,            // (promise-forced? promise)
    OP_SAVE_FORCED,         //
    OP_CONS_STREAM0,        // (cons-stream val rest) => (cons a (delay b))
    OP_CONS_STREAM1,
    OP_CONTINUATION,

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
    OP_NUMBER_EQ,
    OP_LESS,
    OP_GRE,
    OP_LEQ,
    OP_GEQ,
    OP_CAR,
    OP_CDR,
    OP_CONS,
    OP_SET_CAR,
    OP_SET_CDR,

    // op_exec_object_operation
            OP_INEXACT_TO_EXACT,
    OP_CHAR_TO_INT,
    OP_INT_TO_CHAR,
    OP_CHAR_UPPER,
    OP_CHAR_LOWER,
    OP_SYMBOL_TO_STRING,
    OP_STRING_TO_SYMBOL,
    OP_ATOM_TO_STRING,
    OP_STRING_TO_ATOM,
    OP_MAKE_STRING,
    OP_STRING_LENGTH,
    OP_STRING_REF,
    OP_STRING_SET,
    OP_STRING_APPEND,
    OP_SUBSTRING,
    OP_LIST_STAR,            //list*
    OP_LIST_APPEND,
    OP_LIST_REVERSE,
    OP_LIST_LENGTH,
    OP_ASSQ,                // Association Lists 关联列表
    OP_VECTOR,
    OP_MAKE_VECTOR,
    OP_VECTOR_LENGTH,
    OP_VECTOR_REF,
    OP_VECTOR_SET,
    OP_NOT,

    // op_exec_io
            OP_CURRENT_INPUT_PORT,
    OP_CURRENT_OUTPUT_PORT,
    OP_OPEN_INPUT_FILE,
    OP_OPEN_OUTPUT_FILE,
    OP_OPEN_INPUT_OUTPUT_FILE,
    OP_OPEN_INPUT_STRING,
    OP_OPEN_OUTPUT_STRING,
    OP_OPEN_INPUT_OUTPUT_STRING,
    OP_GET_OUTPUT_STRING,
    OP_CLOSE_INPUT_PORT,
    OP_CLOSE_OUTPUT_PORT,
    OP_READ,
    OP_READ_CHAR,
    OP_WRITE,
    OP_WRITE_CHAR,
    OP_PEEK_CHAR,
    OP_CHAR_READY_P,
    OP_SET_INPUT_PORT,
    OP_SET_OUTPUT_PORT,

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
            OP_DEFINE_P,    // (defined? sexp)
    OP_CLOSURE_P,           // (closure? sexp)
    OP_MACRO_P,             // (macro? sexp)
    OP_BOOLEAN_P,           // (boolean? sexp)
    OP_EOF_OBJ_P,           // (eof? sexp)
    OP_NULL_P,              // (null? sexp)
    OP_SYMBOL_P,            // (symbol? sexp)
    OP_NUMBER_P,            // (number? sexp)
    OP_STRING_P,            // (string? sexp)
    OP_INTEGER_P,           // (string? sexp)
    OP_REAL_P,              // real?
    OP_CHAR_P,              // char?
    OP_CHAR_ALPHABETIC_P,   // char-alphabetic?
    OP_CHAR_NUMERIC_P,      // char-numeric?
    OP_CHAR_WHITESPACE_P,   // char-whitespace?
    OP_CHAR_UPPER_CASE_P,   // char-upper-case?
    OP_CHAR_LOWER_CASE_P,   // char-lower-case?
    OP_PORT_P,              // port?
    OP_INPUT_PORT_P,        // input-port?
    OP_OUT_PUT_P,           // output-port?
    OP_PROCEDURE_P,         // procedure?
    OP_PROMISE_P,           // promise?
    OP_PAIR_P,              // pair?
    OP_LIST_P,              // list?
    OP_ENVIRONMENT_P,       // environment?
    OP_VECTOR_P,            // vector?
    OP_EQ,                  // eq?
    OP_EQV,                 // eqv?

    // op_exec_builtin_function
            OP_QUIT,        // (quit) | (quit ret_value)
    OP_DEBUG,               // (debug 1/0)
    OP_GC,                  // (gc)
    OP_ERROR,               // (error "message" [error_obj ...])
    OP_ERROR_PRINT_OBJECT,
    OP_PROC_DISPlAY,        // (display sexp)
    OP_PROC_NEWLINE,        // (newline)
    OP_PROC_EVAL,           // (eval (quote ...) [env])
    OP_PROC_APPLY,          // (apply procedure_name [arg ...])
    OP_INTERACTION_ENV,     // (interaction-environment)
    OP_CURRENT_ENV,         // (current-environment)
#ifdef DEBUG_ONLY
    OP_TRACING_OPCODE,      // (tracing 1/0)
#endif
    // OP_CODE 计数
            MAX_OP_CODE,
};

#endif //PAPER_SCHEME_OPCODES_H