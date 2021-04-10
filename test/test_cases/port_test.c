#include "lib/utest.h"
#include <paper-scheme/runtime.h>


UTEST(port_test, port_get_char_test) {
    context_t context = context_make(0x1000, 1, 0x1000);
    gc_var3(context, port, str, str_buff);

    // string port
    char cstr[] = "this is a port_get_char_test";
    str = string_make_from_cstr_op(context, cstr);
    port = string_port_input_from_string_op(context, str);

    UTEST_PRINTF("ch read: \"");
    for (size_t i = 0; i < string_len(str); i++) {
        object ch = port_get_char(port);
        UTEST_PRINTF("%c", char_imm_getvalue(ch));
        ASSERT_EQ(string_index(str, i), char_imm_getvalue(ch));
    }
    object ch = port_get_char(port);
    ASSERT_EQ(IMM_EOF, ch);
    ch = port_get_char(port);
    ASSERT_EQ(IMM_EOF, ch);
    ch = port_get_char(port);
    ASSERT_EQ(IMM_EOF, ch);
    ASSERT_TRUE(is_port_eof(port));
    UTEST_PRINTF("\"EOF\n");
}

UTEST(port_test, port_unget_char_test) {
    context_t context = context_make(0x1000, 1, 0x1000);
    gc_var3(context, port, str, str_buff);
    object ch;

    // string port
    char cstr[] = "this is a port_unget_char_test";
    str = string_make_from_cstr_op(context, cstr);
    port = string_port_input_from_string_op(context, str);

    UTEST_PRINTF("ch read: \"");
    for (size_t i = 0; i < string_len(str); i++) {
        ch = port_get_char(port);
        UTEST_PRINTF("%c", char_imm_getvalue(ch));
        ASSERT_EQ(string_index(str, i), char_imm_getvalue(ch));
    }
    ch = port_get_char(port);
    UTEST_PRINTF("\"EOF\n");
    ASSERT_EQ(IMM_EOF, ch);

    // unget
    port_unget_char(port, char_imm_make('x'));
    ASSERT_FALSE(is_port_eof(port));
    ch = port_get_char(port);
    ASSERT_EQ(char_imm_getvalue(ch), 't');
    ch = port_get_char(port);
    ASSERT_EQ(ch, IMM_EOF);
    ASSERT_TRUE(is_port_eof(port));

    for (int64_t i = strlen(cstr) - 1; i >= 0; i--) {
        port_unget_char(port, char_imm_make(cstr[i]));
    }
    UTEST_PRINTF("ch read after unget: \"");
    for (size_t i = 0; i < string_len(str); i++) {
        ch = port_get_char(port);
        UTEST_PRINTF("%c", char_imm_getvalue(ch));
        ASSERT_EQ(string_index(str, i), char_imm_getvalue(ch));
    }
    ch = port_get_char(port);
    UTEST_PRINTF("\"EOF\n");
    ASSERT_EQ(IMM_EOF, ch);
}

UTEST(port_test, port_output_test) {
    context_t context = context_make(0x1000, 1, 0x1000);
    gc_var3(context, port, str, str_buff);

    object ch;

    // string port
    char cstr[] = "this is a port_output_test";
    char that_cstr[] = "that is a port_output_test";
    char end_fix_cstr[] = "that is a port_output_test. over";
    str = string_make_from_cstr_op(context, cstr);
    port = string_port_in_out_put_use_buffer_op(context);
    port_put_string(context, port, str);

    port_seek(port, 0, 0);
    UTEST_PRINTF("ch read: \"");
    for (size_t i = 0; i < string_len(str); i++) {
        ch = port_get_char(port);
        UTEST_PRINTF("%c", char_imm_getvalue(ch));
        ASSERT_EQ(string_index(str, i), char_imm_getvalue(ch));
    }
    ch = port_get_char(port);
    ASSERT_EQ(IMM_EOF, ch);
    ch = port_get_char(port);
    ASSERT_EQ(IMM_EOF, ch);
    ch = port_get_char(port);
    ASSERT_EQ(IMM_EOF, ch);
    ASSERT_TRUE(is_port_eof(port));
    UTEST_PRINTF("\"EOF\n");

    port_seek(port, 0, 0);
    port_put_char(context, port, char_imm_make('t'));
    port_put_char(context, port, char_imm_make('h'));
    port_put_char(context, port, char_imm_make('a'));
    port_put_char(context, port, char_imm_make('t'));

    port_seek(port, 0, 0);
    str = string_make_from_cstr_op(context, that_cstr);
    UTEST_PRINTF("ch read: \"");
    for (size_t i = 0; i < string_len(str); i++) {
        ch = port_get_char(port);
        UTEST_PRINTF("%c", char_imm_getvalue(ch));
        ASSERT_EQ(string_index(str, i), char_imm_getvalue(ch));
    }
    ch = port_get_char(port);
    ASSERT_EQ(IMM_EOF, ch);
    ch = port_get_char(port);
    ASSERT_EQ(IMM_EOF, ch);
    ch = port_get_char(port);
    ASSERT_EQ(IMM_EOF, ch);
    ASSERT_TRUE(is_port_eof(port));
    UTEST_PRINTF("\"EOF\n");

    port_seek(port, 0, 2);
    ch = port_get_char(port);
    ASSERT_EQ(IMM_EOF, ch);
    ch = port_get_char(port);
    ASSERT_EQ(IMM_EOF, ch);
    port_put_cstr(context, port, ". over");
    ch = port_get_char(port);
    ASSERT_EQ(IMM_EOF, ch);

    port_seek(port, -4, 2);
    port_put_char(context, port, char_imm_make('o'));
    port_put_char(context, port, char_imm_make('v'));
    port_put_char(context, port, char_imm_make('e'));
    port_put_char(context, port, char_imm_make('r'));

    port_seek(port, 0, 0);
    str = string_make_from_cstr_op(context, end_fix_cstr);
    UTEST_PRINTF("ch read: \"");
    for (size_t i = 0; i < string_len(str); i++) {
        ch = port_get_char(port);
        UTEST_PRINTF("%c", char_imm_getvalue(ch));
        ASSERT_EQ(string_index(str, i), char_imm_getvalue(ch));
    }
    ch = port_get_char(port);
    ASSERT_EQ(IMM_EOF, ch);
    ch = port_get_char(port);
    ASSERT_EQ(IMM_EOF, ch);
    ch = port_get_char(port);
    ASSERT_EQ(IMM_EOF, ch);
    ASSERT_TRUE(is_port_eof(port));
    UTEST_PRINTF("\"EOF\n");
}