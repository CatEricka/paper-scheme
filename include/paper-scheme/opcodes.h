#ifndef PAPER_SCHEME_OPCODES_H
#define PAPER_SCHEME_OPCODES_H
#pragma once

#include <paper-scheme/feature.h>

enum opcode_e {
    OP_TOP_LEVEL = 0,
    READ_SEXP_OP,
    // TODO opcode
            MAX_OP_CODE,
};

/**
 * 考虑到内部过程需要参数检查
 * 内部过程参数检查表会被编码成 char *
 */
compile_time_assert(MAX_OP_CODE < CHAR_MAX);

#endif //PAPER_SCHEME_OPCODES_H