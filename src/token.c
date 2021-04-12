#include <paper-scheme/token.h>

/**
 * �����հ׷��Ŷ���һ���ַ�
 * @param context
 * @return
 */
static object skip_space(context_t context) {
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

    return c;
}

/**
 * ���� token, ���ظ� context->token
 * @param context
 * @return enum token_e
 */
EXPORT_API int token(context_t context) {
    int c;
    return 0;
}