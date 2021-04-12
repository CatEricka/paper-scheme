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

#endif //PAPER_SCHEME_OPCODES_H