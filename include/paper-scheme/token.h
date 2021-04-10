#ifndef PAPER_SCHEME_TOKEN_H
#define PAPER_SCHEME_TOKEN_H
#pragma once

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

// жу╫А╥Ш: () " \f \t \v \n \r ©у╦Я
#define DELIMITERS "()\";\f\t\v\n\r "

#endif //PAPER_SCHEME_TOKEN_H
