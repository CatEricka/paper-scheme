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

// �ս��: () " \f \t \v \n \r �ո�
#define DELIMITERS "()\";\f\t\v\n\r "

/**
 * �� context->in_port ����ֱ������ terminal
 * @param context
 * @param terminal
 * @return
 */
EXPORT_API object read_upto(context_t context, char *terminal);

/**
 * �� context->in_port ����һ���ַ���
 * <p>�ڲ�ʵ��, ����ʹ��</p>
 * <p>from tiny_scheme</p>
 * @param context
 * @return ���ʧ�ܷ��� IMM_UNIT
 */
EXPORT_API GC object read_string_expr(context_t context);

/**
 * ���� token, ���ظ� context->token
 * <p>from tiny_scheme</p>
 * @param context
 * @return enum token_e
 */
EXPORT_API int token(context_t context);

#endif //PAPER_SCHEME_TOKEN_H
