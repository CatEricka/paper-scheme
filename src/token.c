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

    buff = string_buffer_make_op(context, STRING_BUFFER_DEFAULT_INIT_SIZE);
    object ch = port_get_char(context->in_port);
    while (ch != IMM_EOF && !is_one_of(terminal, ch)) {
        string_buffer_append_char_op(context, buff, char_imm_getvalue(ch));
        ch = port_get_char(context->in_port);
    }
    // 遇到 EOF 或者 结束符, 将符号送回 port
    port_unget_char(context->in_port, ch);

    gc_release_var(context);
    return string_buffer_to_string_op(context, buff);
}

/**
 * 从 context->in_port 读入一个字符串
 * <p>内部实现, 请勿使用</p>
 * <p>from tinyscheme</p>
 * @param context
 * @return 如果失败返回 IMM_UNIT
 */
EXPORT_API GC object read_string_expr(context_t context) {
    assert(context != NULL);
    // 此时 in_port 刚读入一个 "
    gc_var1(context, buffer);

    buffer = string_buffer_make_op(context, STRING_BUFFER_DEFAULT_INIT_SIZE);

    object c;
    int tmp_ch;
    unsigned escape_value = 0;
    enum {
        status_normal, status_escape,
        status_escape_x1, status_escape_x2,
        status_escape_oct1, status_escape_oct2
    } state = status_normal;

    do {
        c = port_get_char(context->in_port);
        if (c == IMM_EOF) {
            gc_release_var(context);
            return IMM_UNIT;
        }
        switch (state) {
            case status_normal:
                switch (char_imm_getvalue(c)) {
                    case '\\':
                        // 转义
                        state = status_escape;
                        break;
                    case '"':
                        // 结束
                        gc_release_var(context);
                        return string_buffer_to_string_op(context, buffer);
                    default:
                        // 普通情况
                        string_buffer_append_imm_char_op(context, buffer, c);
                        break;
                }
                break;
            case status_escape:
                switch (char_imm_getvalue(c)) {
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                        // "\0****"
                        state = status_escape_oct1;
                        escape_value = char_imm_getvalue(c) - '0';
                        break;
                    case 'x':
                    case 'X':
                        // "\x**", 只能最多两个字符
                        state = status_escape_x1;
                        escape_value = 0;
                        break;
                    case 'n':
                        string_buffer_append_char_op(context, buffer, '\n');
                        state = status_normal;
                        break;
                    case 't':
                        string_buffer_append_char_op(context, buffer, '\t');
                        state = status_normal;
                        break;
                    case 'r':
                        string_buffer_append_char_op(context, buffer, '\r');
                        state = status_normal;
                        break;
                    case '"':
                        string_buffer_append_char_op(context, buffer, '"');
                        state = status_normal;
                        break;
                    default:
                        // 其他情况正常读入
                        string_buffer_append_imm_char_op(context, buffer, c);
                        state = status_normal;
                        break;
                }
                break;
            case status_escape_x1:
            case status_escape_x2:
                tmp_ch = char_imm_getvalue(c);
                tmp_ch = toupper(tmp_ch);
                if (tmp_ch >= '0' && tmp_ch <= 'F') {
                    if (tmp_ch <= '9') {
                        escape_value = (escape_value << 4u) + tmp_ch - '0';
                    } else {
                        escape_value = (escape_value << 4u) + tmp_ch - 'A' + 10;
                    }
                    if (state == status_escape_x1) {
                        // 读入 \x 后第一个字符
                        state = status_escape_x2;
                    } else {
                        // 读入 \x 后第二个字符, 恢复正常状态
                        string_buffer_append_char_op(context, buffer, (char) escape_value);
                        state = status_normal;
                    }
                } else {
                    // \x 转义错误
                    gc_release_var(context);
                    return IMM_UNIT;
                }
                break;
            case status_escape_oct1:
            case status_escape_oct2:
                tmp_ch = char_imm_getvalue(c);
                if (tmp_ch < '0' || tmp_ch > '7') {
                    // 超出正常 8 进制范围
                    string_buffer_append_char_op(context, buffer, (char) escape_value);
                    port_unget_char(context->in_port, c);
                    state = status_normal;
                } else {
                    if (state == status_escape_oct2 && escape_value >= 32) {
                        // 超出转义数值范围
                        // 最小 \000, 最大 \377
                        // 最多读入 3 个字符
                        // 如果第二个字符读入时大小 >= 32 则证明一定超过最大范围
                        gc_release_var(context);
                        return IMM_UNIT;
                    }

                    tmp_ch = char_imm_getvalue(c);
                    escape_value = (escape_value << 3u) + (tmp_ch - '0');

                    if (state == status_escape_oct1)
                        state = status_escape_oct2;
                    else {
                        string_buffer_append_char_op(context, buffer, (char) escape_value);
                        state = status_normal;
                    }
                }
                break;
            default:
                gc_release_var(context);
                return IMM_UNIT;
        }
    } while (1);

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
        case '[':
            return TOKEN_LEFT_PAREN;
        case ')':
        case ']':
            return TOKEN_RIGHT_PAREN;
        case '.':
            c = port_get_char(context->in_port);
            if (is_one_of(" \n\t", c)) {
                return TOKEN_DOT;
            } else if (is_one_of(")]", c)) {
                return TOKEN_ILLEGAL;
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
            // # hook
            c = port_get_char(context->in_port);
            if (c == char_imm_make('(') || c == char_imm_make('[')) {
                return TOKEN_VECTOR;
            } else if (c == char_imm_make('!')) {
                // #! 注释
                while ((c = port_get_char(context->in_port)) != char_imm_make('\n') && c != IMM_EOF);

                // 增加行数标记
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