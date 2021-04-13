#ifndef PAPER_SCHEME_OPCODES_H
#define PAPER_SCHEME_OPCODES_H
#pragma once

#include <paper-scheme/feature.h>

enum opcode_e {
    OP_TOP_LEVEL = 0,
    OP_READ_ONCE,
    OP_READ_SEXP,
    OP_ERROR,
    // TODO opcode
            MAX_OP_CODE,
};

#endif //PAPER_SCHEME_OPCODES_H