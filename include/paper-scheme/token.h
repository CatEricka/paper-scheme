#ifndef PAPER_SCHEME_TOKEN_H
#define PAPER_SCHEME_TOKEN_H
#pragma once

#include <paper-scheme/runtime.h>

enum token_e {
    TOKEN_LEFT_PAREN = 0,   // (
    TOKEN_RIGHT_PAREN,      // )
    TOKEN_DOT,              // .
    TOKEN_QUOTE,            // '
    TOKEN_ATOM,             // atom
    TOKEN_DOUBLE_QUOTE,     // "
    TOKEN_BACK_QUOTE,       // `
    TOKEN_AT_MART,          // @
    TOKEN_COMMA,            // ,
    TOKEN_SHARP_CONST,      // #tfodxb
    TOKEN_VECTOR,           // #(var1 var2 ...)
    TOKEN_EOF,              // end of file
};

// 终结符: () " \f \t \v \n \r 空格
#define DELIMITERS "()\";\f\t\v\n\r "

/**
 * 从 context->in_port 读入直到遇到 terminal
 * @param context
 * @param terminal
 * @return
 */
EXPORT_API object read_upto(context_t context, char *terminal);

/**
 * 从 context->in_port 读入一个字符串
 * <p>内部实现, 请勿使用</p>
 * <p>from tiny_scheme</p>
 * @param context
 * @return 如果失败返回 IMM_UNIT
 */
EXPORT_API GC object read_string_expr(context_t context);

/**
 * 读入 token, 返回给 context->token
 * <p>from tiny_scheme</p>
 * @param context
 * @return enum token_e
 */
EXPORT_API int token(context_t context);

#endif //PAPER_SCHEME_TOKEN_H
