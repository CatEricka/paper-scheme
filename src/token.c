#include <paper-scheme/token.h>

/**
 * �����հ׷���, ������ǿհ��ַ�
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

// from tiny_scheme
static int is_one_of(char *s, object c) {
    if (c == IMM_EOF) return 1;
    char ch = char_imm_getvalue(c);
    while (*s)
        if (*s++ == ch)
            return 1;
    return 0;
}

EXPORT_API object read_upto(context_t context, char *terminal) {
    assert(context != NULL);
    gc_var1(context, buff);

    object ch = port_get_char(context->in_port);
    while (ch != IMM_EOF && !is_one_of(terminal, ch)) {
        buff = string_buffer_append_char_op(context, buff, char_imm_getvalue(ch));
        ch = port_get_char(context->in_port);
    }
    // ���� EOF ���� ������, �������ͻ� port
    port_unget_char(context->in_port, ch);

    gc_release_var(context);
    return string_buffer_to_string_op(context, buff);
}

/**
 * �� context->in_port ����һ���ַ���
 * <p>�ڲ�ʵ��, ����ʹ��</p>
 * @param context
 * @return ���ʧ�ܷ��� IMM_UNIT
 */
EXPORT_API GC object read_string_expr(context_t context) {
    assert(context != NULL);
    // ��ʱ in_port �ն���һ�� "
    return IMM_UNIT;
}

/**
 * ���� token, ���ظ� context->token
 * @param context
 * @return enum token_e
 */
EXPORT_API int token(context_t context) {
    object c;
    if (skip_space(context) == EOF) {
        return (TOKEN_EOF);
    }

    int raw_ch;
    c = port_get_char(context->in_port);
    if (c == IMM_EOF) {
        raw_ch = EOF;
    } else {
        raw_ch = char_imm_getvalue(c);
    }
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
            // ; ע��
            while ((c = port_get_char(context->in_port)) != char_imm_make('\n') && c != IMM_EOF);

            // �����������
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
            // # hook
            c = port_get_char(context->in_port);
            if (c == char_imm_make('(')) {
                return TOKEN_VECTOR;
            } else if (c == char_imm_make('!')) {
                // #! ע��
                while ((c = port_get_char(context->in_port)) != char_imm_make('\n') && c != IMM_EOF);

                // �����������
                if (c == char_imm_make('\n') && is_stdio_port(context->in_port))
                    stdio_port_get_line(context->in_port)++;

                if (c == IMM_EOF) { return TOKEN_EOF; }
                else { return token(context); }
            } else {
                port_unget_char(context->in_port, c);
                return TOKEN_SHARP_CONST;
            }
        default:
            port_unget_char(context->in_port, c);
            return TOKEN_ATOM;
    }
}