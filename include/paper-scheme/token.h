#ifndef PAPER_SCHEME_TOKEN_H
#define PAPER_SCHEME_TOKEN_H
#pragma once

#include <paper-scheme/runtime.h>

enum token_e {
    TOKEN_LEFT_PAREN = 0,   // (
    TOKEN_RIGHT_PAREN,      // )
    TOKEN_DOT,              // ,
    TOKEN_QUOTE,            // '
    TOKEN_ATOM,             // atom
    TOKEN_COMMENT,          // ; comment
    TOKEN_DOUBLE_QUOTE,     // "
    TOKEN_BACK_QUOTE,       // `
    TOKEN_AT_MART,          // @
    TOKEN_SHARP,            // #
    TOKEN_SHARP_CONST,      // #tfodxb
    TOKEN_VECTOR,           // #(var1 var2 ...)
    TOKEN_EOF,              // end of file
};

// 终结符: () " \f \t \v \n \r 空格
#define DELIMITERS "()\";\f\t\v\n\r "

/**
 * 读入 token, 返回给 context->token
 * @param context
 * @return enum token_e
 */
EXPORT_API int token(context_t context);

#endif //PAPER_SCHEME_TOKEN_H
