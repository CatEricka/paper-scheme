#include <paper-scheme/token.h>

/**
 * 跳过空白符号, 不读入非空白字符
 * @param context
 * @return
 */
static int skip_space(context_t context) {
    object c;
    size_t line = 0;
    do {
        c = port_get_char(context->in_port);
        if (c == char_imm_make('\n'))
            line++;
    } while (isspace(char_imm_getvalue(c)));

    if (is_stdio_port(context->in_port)) {
        context->in_port->value.stdio_port.current_line += line;
    }

    if (c != IMM_EOF) {
        port_unget_char(context->in_port, c);
        return 1;
    } else return EOF;
}

static int is_one_of(char *s, object c) {
    if (c == IMM_EOF) return 1;
    char ch = char_imm_getvalue(c);
    while (*s)
        if (*s++ == ch)
            return 1;
    return 0;
}

/**
 * 读入 token, 返回给 context->token
 * @param context
 * @return enum token_e
 */
EXPORT_API int token(context_t context) {
    object c;
    if (skip_space(context) == EOF) {
        return (TOKEN_EOF);
    }

    c = port_get_char(context->in_port);
    char raw_ch = char_imm_getvalue(c);
    switch (raw_ch) {
        case EOF:
            return TOKEN_EOF;
        case '(':
            return TOKEN_LEFT_PAREN;
        case ')':
            return TOKEN_RIGHT_PAREN;
        case '.':
            c = port_get_char(context->in_port);
            if (is_one_of(" \n\t", c)) {
                return TOKEN_DOT;
            } else {
                port_unget_char(context->in_port, c);
                port_unget_char(context->in_port, char_imm_make('.'));
                return TOKEN_ATOM;
            }
        case '\'':
            return TOKEN_QUOTE;
        case ';':
            // ; 注释
            while ((c = port_get_char(context->in_port)) != char_imm_make('\n') && c != IMM_EOF);

            // 增加行数标记
            if (c == char_imm_make('\n') && is_stdio_port(context->in_port))
                stdio_port_get_line(context->in_port)++;

            if (c == IMM_EOF) { return TOKEN_EOF; }
            else { return token(context); }
        case '"':
            return TOKEN_DOUBLE_QUOTE;
        case '`':
            return TOKEN_BACK_QUOTE;
        case ',':
            if ((c = port_get_char(context->in_port)) == char_imm_make('@')) {
                return TOKEN_AT_MART;
            } else {
                port_unget_char(context->in_port, c);
                return TOKEN_COMMA;
            }
        case '#':
            // #! 注释
            c = port_get_char(context->in_port);
            if (c == char_imm_make('(')) {
                return TOKEN_VECTOR;
            } else if (c == char_imm_make('!')) {
                while ((c = port_get_char(context->in_port)) != char_imm_make('\n') && c != IMM_EOF);

                // 增加行数标记
                if (c == char_imm_make('\n') && is_stdio_port(context->in_port))
                    stdio_port_get_line(context->in_port)++;

                if (c == IMM_EOF) { return TOKEN_EOF; }
                else { return token(context); }
            } else {
                port_unget_char(context->in_port, c);
                if (is_one_of(" tfodxb\\", c)) {
                    return TOKEN_SHARP_CONST;
                } else {
                    return TOKEN_SHARP;
                }
            }
        default:
            port_unget_char(context->in_port, c);
            return TOKEN_ATOM;
    }
}