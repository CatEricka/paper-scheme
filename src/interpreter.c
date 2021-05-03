#include "paper-scheme/interpreter.h"

/******************************************************************************
                            �ؼ��ֺ��ڲ����̳�ʼ��
******************************************************************************/
/**
 * �������� symbol �Ƿ��� syntax
 * @param context
 * @param symbol
 * @return IMM_TRUE/IMM_FALSE
 */
EXPORT_API object symbol_is_syntax(context_t context, object symbol) {
    assert(context != NULL);
    assert(is_symbol(symbol));

    return hashmap_contains_key_op(context, context->syntax_table, symbol);
}

/**
 * �� symbol ��ȡ syntax
 * <p>���ᴥ�� GC</p>
 * @param context
 * @param symbol
 * @return syntax, �������򷵻� IMM_UNIT
 */
EXPORT_API object syntax_get_by_symbol(context_t context, object symbol) {
    assert(context != NULL);
    assert(is_symbol(symbol));
    object syntax = hashmap_get_op(context, context->syntax_table, symbol);
    assert(is_syntax(syntax));
    return syntax;
}

/**
 * �� cstr ��ȡ syntax
 * @param context
 * @param symbol
 * @return syntax, �������򷵻� IMM_UNIT
 */
static GC object syntax_get_by_cstr(context_t context, char *cstr) {
    assert(context != NULL);
    assert(cstr != NULL);
    gc_var2(context, sym, syntax);
    sym = symbol_make_from_cstr_op(context, cstr);
    syntax = hashmap_get_op(context, context->syntax_table, sym);
    assert(is_syntax(syntax));
    gc_release_var(context);
    return syntax;
}

static GC void assign_syntax(context_t context, char *name, enum opcode_e opcode) {
    assert(context != NULL);
    assert(name != NULL);
    assert(strlen(name) != 0);
    gc_var2(context, sym, syntax);

    sym = symbol_make_from_cstr_op(context, name);
    set_immutable(sym);
    syntax = syntax_make_internal(context, sym, opcode);
    hashmap_put_op(context, context->syntax_table, sym, syntax);

    gc_release_var(context);
}

static GC void assign_proc(context_t context, char *name, enum opcode_e opcode) {
    assert(context != NULL);
    assert(name != NULL);
    assert(strlen(name) != 0);
    assert(opcode >= 0 && opcode < MAX_OP_CODE);
    gc_var2(context, proc, sym);

    sym = symbol_make_from_cstr_op(context, name);
    proc = proc_make_internal(context, sym, opcode);
    new_slot_in_spec_env(context, sym, proc, context->global_environment);

    gc_release_var(context);
}


/******************************************************************************
                              �ʷ����������
******************************************************************************/

// x �� pair �� cdr(x) �� IMM_UNIT
#define ok_abbrev(x)   (is_pair((x)) && is_imm_unit(pair_cdr((x))))

static int64_t binary_decode(const char *s) {
    uint64_t x = 0;

    while (*s != 0 && (*s == '1' || *s == '0')) {
        x <<= 1u;
        x += *s - '0';
        s++;
    }

    return (int64_t) x;
}
/**
 * ���������ַ������� sharp ����
 * @param context
 * @param str
 * @return sharp ������ IMM_UNIT (�������Ƿ�)
 */
EXPORT_API GC object sharp_const_make_op(context_t context, object str) {
    assert(context != NULL);
    gc_param1(context, str);
    gc_var1(context, obj);
    char format_buff[__Format_buff_size__];
    obj = IMM_UNIT;

    if (0 == string_len(str)) {
        gc_release_param(context);
        return IMM_UNIT;
    }

    if (!strcmp(string_get_cstr(str), "t")) {
        gc_release_param(context);
        return IMM_TRUE;
    } else if (!strcmp(string_get_cstr(str), "f")) {
        gc_release_param(context);
        return IMM_FALSE;
    } else if (string_index(str, 0) == 'o') {
        snprintf(format_buff, __Format_buff_size__, "0%s", string_get_cstr(str) + 1);
        uint64_t x;
        if (sscanf(format_buff, "%"SCNo64, &x) == 1) {
            // �ǵ�, ����������ת���İ�ȫ����, ����ʵ���з�����̫�鷳��.
            // ����ʵ���з���������ʵ�ִ�������
            obj = i64_make_op(context, (int64_t) x);
        } else {
            obj = IMM_UNIT;
        }
        gc_release_param(context);
        return obj;

    } else if (string_index(str, 0) == 'd') {
        int64_t x;
        if (sscanf(string_get_cstr(str) + 1, "%"SCNd64, &x) == 1) {
            obj = i64_make_op(context, (int64_t) x);
        } else {
            obj = IMM_UNIT;
        }
        gc_release_param(context);
        return obj;
    } else if (string_index(str, 0) == 'x') {
        snprintf(format_buff, __Format_buff_size__, "0x%s", string_get_cstr(str) + 1);
        uint64_t x;
        if (sscanf(format_buff, "%"SCNx64, &x) == 1) {
            obj = i64_make_op(context, (int64_t) x);
        } else {
            obj = IMM_UNIT;
        }
        gc_release_param(context);
        return obj;
    } else if (string_index(str, 0) == 'b') {
        int64_t x = binary_decode(string_get_cstr(str) + 1);
        obj = i64_make_op(context, x);
        gc_release_param(context);
        return obj;
    } else if (string_index(str, 0) == '\\') {
        // char ������
        obj = IMM_UNIT;
        if (!strcmp(string_get_cstr(str) + 1, "space")) {
            obj = char_imm_make(' ');
        } else if (!strcmp(string_get_cstr(str) + 1, "newline")) {
            obj = char_imm_make('\n');
        } else if (!strcmp(string_get_cstr(str) + 1, "return")) {
            obj = char_imm_make('\r');
        } else if (!strcmp(string_get_cstr(str) + 1, "tab")) {
            obj = char_imm_make('\t');
        } else if (string_index(str, 1) == 'x' && string_index(str, 2) != 0) {
            int hex = 0;
            if (sscanf(string_get_cstr(str) + 2, "%x", &hex) == 1) {
                if (hex >= 0 && hex <= UCHAR_MAX) {
                    obj = char_imm_make((char) hex);
                }
            }
        } else if (string_index(str, 2) == 0) {
            obj = char_imm_make(string_index(str, 1));
        } else {
            obj = IMM_UNIT;
        }
    } else {
        obj = IMM_UNIT;
    }

    // ������ȫ�����, ���� UNIT
    gc_release_param(context);
    return obj;
}

/**
 * ���ַ������� symbol, syntax ���� ����
 * @param context
 * @param str
 * @return
 */
EXPORT_API GC object atom_make_op(context_t context, object str) {
    assert(context != NULL);
    assert(is_string(str));
    gc_param1(context, str);
    gc_var2(context, sym, syntax);

    char *cstr = string_get_cstr(str);
    if (string_len(str) == 0) {
        // ���ַ���Ҳ���� atom
        gc_release_param(context);
        return IMM_UNIT;
    }
    if (strcmp(cstr, ".") == 0) {
        // . ��Ӧ���� atom
        gc_release_param(context);
        return IMM_UNIT;
    }
    char ch, *cstr_cursor = cstr;
    int has_dec_point = 0;

    ch = *cstr_cursor++;

    if ((ch == '+') || (ch == '-')) {
        ch = *cstr_cursor++;
        if (ch == '.') {
            has_dec_point = 1;
            ch = *cstr_cursor++;
        }
        if (!isdigit(ch)) {
            gc_release_param(context);
            return string_to_symbol_op(context, str);
        }
    } else if (ch == '.') {
        has_dec_point = 1;
        ch = *cstr_cursor++;
        if (!isdigit(ch)) {
            gc_release_param(context);
            return string_to_symbol_op(context, str);
        }
    } else if (!isdigit(ch)) {
        gc_release_param(context);
        return string_to_symbol_op(context, str);
    }

    for (; (ch = *cstr_cursor) != 0; ++cstr_cursor) {
        if (!isdigit(ch)) {
            if (ch == '.') {
                if (!has_dec_point) {
                    // ָ�����ֲ�����С����
                    has_dec_point = 1;
                    continue;
                }
            } else if ((ch == 'e') || (ch == 'E')) {
                // example: 12e3
                has_dec_point = 1;
                cstr_cursor++;
                if ((*cstr_cursor == '-') || (*cstr_cursor == '+') || isdigit(*cstr_cursor)) {
                    continue;
                }
            }
            gc_release_param(context);
            return string_to_symbol_op(context, str);
        }
    }
    if (has_dec_point) {
        double f = atof(string_get_cstr(str));
        gc_release_param(context);
        return doublenum_make_op(context, f);
    } else {
        gc_release_param(context);
        int64_t i = atoll(string_get_cstr(str));
        return i64_make_op(context, i);
    }
}

/******************************************************************************
                          opcode procedure ���ͼ��
******************************************************************************/
// û�м��
#define TYPE_NONE         NULL
// �κ�����
#define TYPE_ANY            "\001"
#define TYPE_INTEGER        "\002"
#define TYPE_REAL           "\003"
#define TYPE_NATURAL        "\004"
#define TYPE_NUMBER         "\005"
#define TYPE_CHAR           "\006"
#define TYPE_STRING         "\007"
#define TYPE_SYMBOL         "\010"
#define TYPE_PAIR           "\011"
#define TYPE_LIST           "\012"
#define TYPE_VECTOR         "\013"
#define TYPE_PORT           "\014"
#define TYPE_INPUT_PORT     "\015"
#define TYPE_OUTPUT_PORT    "\016"
#define TYPE_ENVIRONMENT    "\017"

// ����������������
#define ARGS_INF            (INT64_MAX-1)

// 001
static int is_any_test(object obj) { return 1; }

// 003
static int is_real_test(object obj) { return is_doublenum(obj); }

// 004
static int is_natural_test(object obj) { return is_i64(obj) && i64_getvalue(obj) >= 0; }

// 005
static int is_number_test(object obj) { return is_i64(obj) || is_doublenum(obj); }

// 006
static int is_char_test(object obj) { return is_imm_char(obj); }

// 007
static int is_string_test(object obj) { return is_string(obj); }

// 010
static int is_symbol_test(object obj) { return is_symbol(obj); }

// 011
static int is_pair_test(object obj) { return is_pair(obj) && !is_ext_type(obj); }

// 012
static int is_list_test(object obj) { return obj == IMM_UNIT || is_pair(obj); }

// 013
static int is_vector_test(object obj) { return is_vector(obj); }

// 014
static int is_port_test(object obj) { return is_port(obj); }

// 015
static int is_input_port_test(object obj) { return is_port_input(obj); }

// 016
static int is_output_port_test(object obj) { return is_port_output(obj); }

// 017
static int is_environment_test(object obj) { return is_ext_type_environment(obj); }

typedef int (*type_test_func)(object);

struct type_test_t {
    type_test_func test;
    const char *type_kind;
} type_test_table[] = {
        {.test = NULL, .type_kind = NULL},                              // NULL δʹ��: �������ͼ��
        {.test = is_any_test, .type_kind = "any"},                      // 001
        {.test = is_i64, .type_kind = "integer"},                       // 002
        {.test = is_real_test, .type_kind = "real number"},             // 003
        {.test = is_natural_test, .type_kind = "non-negative integer"}, // 004
        {.test = is_number_test, .type_kind = "number"},                // 005
        {.test = is_char_test, .type_kind = "char"},                    // 006
        {.test = is_string_test, .type_kind = "string"},                // 007
        {.test = is_symbol_test, .type_kind = "symbol"},                // 010
        {.test = is_pair_test, .type_kind = "pair"},                    // 011
        {.test = is_list_test, .type_kind = "list"},                    // 012
        {.test = is_vector_test, .type_kind = "vector"},                // 013
        {.test = is_port_test, .type_kind = "port"},                    // 014
        {.test = is_input_port_test, .type_kind = "input port"},        // 015
        {.test = is_output_port_test, .type_kind = "output port"},      // 016
        {.test = is_environment_test, .type_kind = "environment"},      // 017
};


/******************************************************************************
                         �ڲ� opcode dispatch table
******************************************************************************/
// repl ����
static object op_exec_repl(context_t context, enum opcode_e opcode);

// �﷨ʵ��
static object op_exec_syntax(context_t context, enum opcode_e opcode);

// ��������
static object op_exec_compute(context_t context, enum opcode_e opcode);

// �����������
static object op_exec_object_operation(context_t context, enum opcode_e opcode);

// I/O ����
static object op_exec_io(context_t context, enum opcode_e opcode);

// �ʷ��������ַ������
static object op_exec_lexical(context_t context, enum opcode_e opcode);

// ν�ʺ���
static object op_exec_predicate(context_t context, enum opcode_e opcode);

// �ڽ�����
static object op_exec_builtin_function(context_t context, enum opcode_e opcode);

/**
 * op_exec_ �ַ����ڲ�����
 */
op_code_info const internal_dispatch_table[] = {
        // op_exec_repl
        {"load", 1, 1, TYPE_STRING, OP_LOAD, op_exec_repl},
        {NULL, 0, 0, TYPE_NONE, OP_TOP_LEVEL_SETUP, op_exec_repl},
        {NULL, 0, 0, TYPE_NONE, OP_TOP_LEVEL, op_exec_repl},
        {NULL, 0, 0, TYPE_NONE, OP_READ_INTERNAL, op_exec_repl},
        {NULL, 0, 0, TYPE_NONE, OP_VALUE_PRINT, op_exec_repl},
        {NULL, 0, 0, TYPE_NONE, OP_EVAL, op_exec_repl},
        {NULL, 0, 0, TYPE_NONE, OP_EVAL_REAL, op_exec_repl},
        {NULL, 0, 0, TYPE_NONE, OP_EXPAND_MACRO, op_exec_repl},
        {NULL, 0, 0, TYPE_NONE, OP_EVAL_ARGS, op_exec_repl},
        {NULL, 0, 0, TYPE_NONE, OP_APPLY, op_exec_repl},
        {NULL, 0, 0, TYPE_NONE, OP_APPLY_REAL, op_exec_repl},
        {"gensym", 0, 0, TYPE_NONE, OP_GENSYM, op_exec_repl},

        // op_exec_syntax
        {NULL, 0, 0, TYPE_NONE, OP_EVAL_EXPANDED_MACRO, op_exec_syntax},
        {NULL, 0, 0, TYPE_NONE, OP_LAMBDA0, op_exec_syntax},
        {NULL, 0, 0, TYPE_NONE, OP_LAMBDA1, op_exec_syntax},
        {"make-closure", 1, 2, TYPE_PAIR TYPE_ENVIRONMENT, OP_MAKE_CLOSURE, op_exec_syntax},
        {"get-closure-code", 1, 1, TYPE_NONE, OP_GET_CLOSURE_CODE, op_exec_syntax},
        {NULL, 0, 0, TYPE_NONE, OP_QUOTE, op_exec_syntax},
        {NULL, 0, 0, TYPE_NONE, OP_DEFINE0, op_exec_syntax},
        {NULL, 0, 0, TYPE_NONE, OP_DEFINE1, op_exec_syntax},
        {NULL, 0, 0, TYPE_NONE, OP_MACRO0, op_exec_syntax},
        {NULL, 0, 0, TYPE_NONE, OP_MACRO1, op_exec_syntax},
        {NULL, 0, 0, TYPE_NONE, OP_BEGIN, op_exec_syntax},
        {NULL, 0, 0, TYPE_NONE, OP_IF0, op_exec_syntax},
        {NULL, 0, 0, TYPE_NONE, OP_IF1, op_exec_syntax},
        {NULL, 0, 0, TYPE_NONE, OP_SET0, op_exec_syntax},
        {NULL, 0, 0, TYPE_NONE, OP_SET1, op_exec_syntax},
        {NULL, 0, 0, TYPE_NONE, OP_LET0, op_exec_syntax},
        {NULL, 0, 0, TYPE_NONE, OP_LET1, op_exec_syntax},
        {NULL, 0, 0, TYPE_NONE, OP_LET2, op_exec_syntax},
        {NULL, 0, 0, TYPE_NONE, OP_LET_AST0, op_exec_syntax},
        {NULL, 0, 0, TYPE_NONE, OP_LET_AST1, op_exec_syntax},
        {NULL, 0, 0, TYPE_NONE, OP_LET_AST2, op_exec_syntax},
        {NULL, 0, 0, TYPE_NONE, OP_LET_REC0, op_exec_syntax},
        {NULL, 0, 0, TYPE_NONE, OP_LET_REC1, op_exec_syntax},
        {NULL, 0, 0, TYPE_NONE, OP_LET_REC2, op_exec_syntax},
        {NULL, 0, 0, TYPE_NONE, OP_COND0, op_exec_syntax},
        {NULL, 0, 0, TYPE_NONE, OP_COND1, op_exec_syntax},
        {NULL, 0, 0, TYPE_NONE, OP_AND0, op_exec_syntax},
        {NULL, 0, 0, TYPE_NONE, OP_AND1, op_exec_syntax},
        {NULL, 0, 0, TYPE_NONE, OP_OR0, op_exec_syntax},
        {NULL, 0, 0, TYPE_NONE, OP_OR1, op_exec_syntax},
        {NULL, 0, 0, TYPE_NONE, OP_CASE0, op_exec_syntax},
        {NULL, 0, 0, TYPE_NONE, OP_CASE1, op_exec_syntax},
        {NULL, 0, 0, TYPE_NONE, OP_CASE2, op_exec_syntax},
        {NULL, 0, 0, TYPE_NONE, OP_DELAY, op_exec_syntax},
        {"force", 1, 1, TYPE_ANY, OP_FORCE, op_exec_syntax},
        {"promise-forced?", 1, 1, TYPE_ANY, OP_FORCED_P, op_exec_syntax},
        {NULL, 0, 0, TYPE_NONE, OP_SAVE_FORCED, op_exec_syntax},
        {NULL, 0, 0, TYPE_NONE, OP_CONS_STREAM0, op_exec_syntax},
        {NULL, 0, 0, TYPE_NONE, OP_CONS_STREAM1, op_exec_syntax},
        {"call-with-current-continuation", 1, 1, TYPE_NONE, OP_CONTINUATION, op_exec_syntax},

        // op_exec_compute
        {"exp", 1, 1, TYPE_NUMBER, OP_EXP, op_exec_compute},
        {"log", 1, 1, TYPE_NUMBER, OP_LOG, op_exec_compute},
        {"sin", 1, 1, TYPE_NUMBER, OP_SIN, op_exec_compute},
        {"cos", 1, 1, TYPE_NUMBER, OP_COS, op_exec_compute},
        {"tan", 1, 1, TYPE_NUMBER, OP_TAN, op_exec_compute},
        {"asin", 1, 1, TYPE_NUMBER, OP_ASIN, op_exec_compute},
        {"acos", 1, 1, TYPE_NUMBER, OP_ACOS, op_exec_compute},
        {"atan", 1, 2, TYPE_NUMBER, OP_ATAN, op_exec_compute},
        {"sqrt", 1, 1, TYPE_NUMBER, OP_SQRT, op_exec_compute},
        {"expt", 2, 2, TYPE_NUMBER, OP_EXPT, op_exec_compute},
        {"floor", 1, 1, TYPE_NUMBER, OP_FLOOR, op_exec_compute},
        {"ceiling", 1, 1, TYPE_NUMBER, OP_CEILING, op_exec_compute},
        {"truncate", 1, 1, TYPE_NUMBER, OP_TRUNCATE, op_exec_compute},
        {"round", 1, 1, TYPE_NUMBER, OP_ROUND, op_exec_compute},
        {"+", 0, ARGS_INF, TYPE_NUMBER, OP_ADD, op_exec_compute},
        {"-", 1, ARGS_INF, TYPE_NUMBER, OP_SUB, op_exec_compute},
        {"*", 0, ARGS_INF, TYPE_NUMBER, OP_MUL, op_exec_compute},
        {"/", 1, ARGS_INF, TYPE_NUMBER, OP_DIV, op_exec_compute},
        {"quotient", 1, ARGS_INF, TYPE_INTEGER, OP_INT_DIV, op_exec_compute},
        {"remainder", 2, 2, TYPE_INTEGER, OP_REM, op_exec_compute},
        {"modulo", 2, 2, TYPE_INTEGER, OP_MOD, op_exec_compute},
        {"=", 2, ARGS_INF, TYPE_NUMBER, OP_NUMBER_EQ, op_exec_compute},
        {"<", 2, ARGS_INF, TYPE_NUMBER, OP_LESS, op_exec_compute},
        {">", 2, ARGS_INF, TYPE_NUMBER, OP_GRE, op_exec_compute},
        {"<=", 2, ARGS_INF, TYPE_NUMBER, OP_LEQ, op_exec_compute},
        {">=", 2, ARGS_INF, TYPE_NUMBER, OP_GEQ, op_exec_compute},
        {"car", 1, 1, TYPE_PAIR, OP_CAR, op_exec_compute},
        {"cdr", 1, 1, TYPE_PAIR, OP_CDR, op_exec_compute},
        {"cons", 2, 2, TYPE_NONE, OP_CONS, op_exec_compute},
        {"set-car!", 2, 2, TYPE_PAIR TYPE_ANY, OP_SET_CAR, op_exec_compute},
        {"set-cdr!", 2, 2, TYPE_PAIR TYPE_ANY, OP_SET_CDR, op_exec_compute},

        // op_exec_object_operation
        {"inexact->exact", 1, 1, TYPE_NUMBER, OP_INEXACT_TO_EXACT, op_exec_object_operation},
        {"char->integer", 1, 1, TYPE_CHAR, OP_CHAR_TO_INT, op_exec_object_operation},
        {"integer->char", 1, 1, TYPE_NATURAL, OP_INT_TO_CHAR, op_exec_object_operation},
        {"char-upcase", 1, 1, TYPE_CHAR, OP_CHAR_UPPER, op_exec_object_operation},
        {"char-downcase", 1, 1, TYPE_CHAR, OP_CHAR_LOWER, op_exec_object_operation},
        {"symbol->string", 1, 1, TYPE_SYMBOL, OP_SYMBOL_TO_STRING, op_exec_object_operation},
        {"string->symbol", 1, 1, TYPE_STRING, OP_STRING_TO_SYMBOL, op_exec_object_operation},
        {"atom->string", 1, 2, TYPE_ANY TYPE_NATURAL, OP_ATOM_TO_STRING, op_exec_object_operation},
        {"string->atom", 1, 2, TYPE_STRING TYPE_NATURAL, OP_STRING_TO_ATOM, op_exec_object_operation},
        {"make-string", 1, 2, TYPE_NATURAL TYPE_CHAR, OP_MAKE_STRING, op_exec_object_operation},
        {"string-length", 1, 1, TYPE_STRING, OP_STRING_LENGTH, op_exec_object_operation},
        {"string-ref", 2, 2, TYPE_STRING TYPE_NATURAL, OP_STRING_REF, op_exec_object_operation},
        {"string-set!", 3, 3, TYPE_STRING TYPE_NATURAL TYPE_CHAR, OP_STRING_SET, op_exec_object_operation},
        {"string-append", 0, ARGS_INF, TYPE_STRING, OP_STRING_APPEND, op_exec_object_operation},
        {"substring", 2, 3, TYPE_VECTOR TYPE_NATURAL, OP_SUBSTRING, op_exec_object_operation},
        {"list*", 1, ARGS_INF, TYPE_NONE, OP_LIST_STAR, op_exec_object_operation},
        {"append", 0, ARGS_INF, TYPE_LIST, OP_LIST_APPEND, op_exec_object_operation},
        {"reverse", 1, 1, TYPE_LIST, OP_LIST_REVERSE, op_exec_object_operation},
        {"length", 1, 1, TYPE_LIST, OP_LIST_LENGTH, op_exec_object_operation},
        {"assq", 2, 2, TYPE_NONE, OP_ASSQ, op_exec_object_operation},
        {"vector", 0, ARGS_INF, TYPE_NONE, OP_VECTOR, op_exec_object_operation},
        {"make-vector", 1, 2, TYPE_NATURAL TYPE_ANY, OP_MAKE_VECTOR, op_exec_object_operation},
        {"vector-length", 1, 1, TYPE_VECTOR, OP_VECTOR_LENGTH, op_exec_object_operation},
        {"vector-ref", 2, 2, TYPE_VECTOR TYPE_NATURAL, OP_VECTOR_REF, op_exec_object_operation},
        {"vector-set!", 3, 3, TYPE_VECTOR TYPE_NATURAL TYPE_ANY, OP_VECTOR_SET, op_exec_object_operation},
        {"not", 1, 1, TYPE_NONE, OP_NOT, op_exec_object_operation},

        // op_exec_io
        {"current-input-port", 0, 0, TYPE_NONE, OP_CURRENT_INPUT_PORT, op_exec_io},
        {"current-output-port", 0, 0, TYPE_NONE, OP_CURRENT_OUTPUT_PORT, op_exec_io},
        {"open-input-file", 1, 1, TYPE_STRING, OP_OPEN_INPUT_FILE, op_exec_io},
        {"open-output-file", 1, 1, TYPE_STRING, OP_OPEN_OUTPUT_FILE, op_exec_io},
        {"open-input-output-file", 1, 1, TYPE_STRING, OP_OPEN_INPUT_OUTPUT_FILE, op_exec_io},
        {"open-input-string", 1, 1, TYPE_STRING, OP_OPEN_INPUT_STRING, op_exec_io},
        {"open-output-string", 0, 1, TYPE_STRING, OP_OPEN_OUTPUT_STRING, op_exec_io},
        {"open-input-output-string", 1, 1, TYPE_STRING, OP_OPEN_INPUT_OUTPUT_STRING, op_exec_io},
        {"get-output-string", 1, 1, TYPE_OUTPUT_PORT, OP_GET_OUTPUT_STRING, op_exec_io},
        {"close-input-port", 1, 1, TYPE_INPUT_PORT, OP_CLOSE_INPUT_PORT, op_exec_io},
        {"close-output-port", 1, 1, TYPE_OUTPUT_PORT, OP_CLOSE_OUTPUT_PORT, op_exec_io},
        {"read", 0, 1, TYPE_INPUT_PORT, OP_READ, op_exec_io},
        {"read-char", 0, 1, TYPE_INPUT_PORT, OP_READ_CHAR, op_exec_io},
        {"write", 1, 2, TYPE_ANY TYPE_OUTPUT_PORT, OP_WRITE, op_exec_io},
        {"write-char", 1, 2, TYPE_CHAR TYPE_OUTPUT_PORT, OP_WRITE_CHAR, op_exec_io},
        {"peek-char", 0, 1, TYPE_INPUT_PORT, OP_PEEK_CHAR, op_exec_io},
        {"char-ready?", 0, 1, TYPE_INPUT_PORT, OP_CHAR_READY_P, op_exec_io},
        {"set-input-port", 1, 1, TYPE_INPUT_PORT, OP_SET_INPUT_PORT, op_exec_io},
        {"set-output-port", 1, 1, TYPE_OUTPUT_PORT, OP_SET_OUTPUT_PORT, op_exec_io},

        // op_exec_lexical
        {NULL, 0, 0, TYPE_NONE, OP_READ_SEXP, op_exec_lexical},
        {NULL, 0, 0, TYPE_NONE, OP_READ_LIST, op_exec_lexical},
        {NULL, 0, 0, TYPE_NONE, OP_READ_DOT, op_exec_lexical},
        {NULL, 0, 0, TYPE_NONE, OP_READ_QUOTE, op_exec_lexical},
        {NULL, 0, 0, TYPE_NONE, OP_READ_UNQUOTE, op_exec_lexical},
        {NULL, 0, 0, TYPE_NONE, OP_READ_QUASIQUOTE, op_exec_lexical},
        {NULL, 0, 0, TYPE_NONE, OP_READ_QUASIQUOTE_VECTOR, op_exec_lexical},
        {NULL, 0, 0, TYPE_NONE, OP_READ_UNQUOTE_SPLICING, op_exec_lexical},
        {NULL, 0, 0, TYPE_NONE, OP_READ_VECTOR, op_exec_lexical},
        {NULL, 0, 0, TYPE_NONE, OP_PRINT_OBJECT, op_exec_lexical},
        {NULL, 0, 0, TYPE_NONE, OP_PRINT_LIST, op_exec_lexical},
        {NULL, 0, 0, TYPE_NONE, OP_PRINT_VECTOR, op_exec_lexical},

        // op_exec_predicate
        {"defined?", 1, 1, TYPE_NONE, OP_DEFINE_P, op_exec_predicate},
        {"closure?", 1, 1, TYPE_NONE, OP_CLOSURE_P, op_exec_predicate},
        {"macro?", 1, 1, TYPE_NONE, OP_MACRO_P, op_exec_predicate},
        {"boolean?", 1, 1, TYPE_NONE, OP_BOOLEAN_P, op_exec_predicate},
        {"eof-object?", 1, 1, TYPE_NONE, OP_EOF_OBJ_P, op_exec_predicate},
        {"null?", 1, 1, TYPE_NONE, OP_NULL_P, op_exec_predicate},
        {"symbol?", 1, 1, TYPE_ANY, OP_SYMBOL_P, op_exec_predicate},
        {"number?", 1, 1, TYPE_ANY, OP_NUMBER_P, op_exec_predicate},
        {"string?", 1, 1, TYPE_ANY, OP_STRING_P, op_exec_predicate},
        {"integer?", 1, 1, TYPE_ANY, OP_INTEGER_P, op_exec_predicate},
        {"real?", 1, 1, TYPE_ANY, OP_REAL_P, op_exec_predicate},
        {"char?", 1, 1, TYPE_ANY, OP_CHAR_P, op_exec_predicate},
        {"char-alphabetic?", 1, 1, TYPE_CHAR, OP_CHAR_ALPHABETIC_P, op_exec_predicate},
        {"char-numeric?", 1, 1, TYPE_CHAR, OP_CHAR_NUMERIC_P, op_exec_predicate},
        {"char-whitespace?", 1, 1, TYPE_CHAR, OP_CHAR_WHITESPACE_P, op_exec_predicate},
        {"char-upper-case?", 1, 1, TYPE_CHAR, OP_CHAR_UPPER_CASE_P, op_exec_predicate},
        {"char-lower-case?", 1, 1, TYPE_CHAR, OP_CHAR_LOWER_CASE_P, op_exec_predicate},
        {"port?", 1, 1, TYPE_ANY, OP_PORT_P, op_exec_predicate},
        {"input-port?", 1, 1, TYPE_ANY, OP_INPUT_PORT_P, op_exec_predicate},
        {"output-port?", 1, 1, TYPE_ANY, OP_OUT_PUT_P, op_exec_predicate},
        {"procedure?", 1, 1, TYPE_ANY, OP_PROCEDURE_P, op_exec_predicate},
        {"promise?", 1, 1, TYPE_ANY, OP_PROMISE_P, op_exec_predicate},
        {"pair?", 1, 1, TYPE_ANY, OP_PAIR_P, op_exec_predicate},
        {"list?", 1, 1, TYPE_ANY, OP_LIST_P, op_exec_predicate},
        {"environment?", 1, 1, TYPE_ANY, OP_ENVIRONMENT_P, op_exec_predicate},
        {"vector?", 1, 1, TYPE_ANY, OP_VECTOR_P, op_exec_predicate},
        {"eq?", 2, 2, TYPE_ANY, OP_EQ, op_exec_predicate},
        {"eqv?", 2, 2, TYPE_ANY, OP_EQV, op_exec_predicate},

        // op_exec_builtin_function
        {"quit", 0, 1, TYPE_INTEGER, OP_QUIT, op_exec_builtin_function},
        {"debug", 1, 1, TYPE_INTEGER, OP_DEBUG, op_exec_builtin_function},
        {"gc", 0, 0, TYPE_INTEGER, OP_GC, op_exec_builtin_function},
        {"error", 1, ARGS_INF, TYPE_NONE, OP_ERROR, op_exec_builtin_function},
        {NULL, 1, ARGS_INF, TYPE_NONE, OP_ERROR_PRINT_OBJECT, op_exec_builtin_function},
        {"display", 1, 2, TYPE_ANY TYPE_OUTPUT_PORT, OP_PROC_DISPlAY, op_exec_builtin_function},
        {"newline", 0, 1, TYPE_OUTPUT_PORT, OP_PROC_NEWLINE, op_exec_builtin_function},
        {"eval", 1, 2, TYPE_ANY TYPE_ENVIRONMENT, OP_PROC_EVAL, op_exec_builtin_function},
        {"apply", 1, ARGS_INF, TYPE_NONE, OP_PROC_APPLY, op_exec_builtin_function},
        {"interaction-environment", 0, 0, TYPE_NONE, OP_INTERACTION_ENV, op_exec_builtin_function},
        {"current-environment", 0, 0, TYPE_NONE, OP_CURRENT_ENV, op_exec_builtin_function},
#ifdef DEBUG_ONLY
        {"tracing", 1, 1, TYPE_INTEGER, OP_TRACING_OPCODE, op_exec_builtin_function},
#endif
};

/**
 * op_exec_ �������ͼ���
 */

static GC void init_opcode(context_t context) {
    for (size_t i = 0; i < sizeof(internal_dispatch_table) / sizeof(op_code_info); i++) {
        op_code_info const *info = &internal_dispatch_table[i];
        int op = info->op;
        context->dispatch_table[op].func = info->func;
        context->dispatch_table[op].min_args_length = info->min_args_length;
        context->dispatch_table[op].max_args_length = info->max_args_length;
        context->dispatch_table[op].args_type_check_table = info->args_type_check_table;
        context->dispatch_table[op].name = info->name;
        context->dispatch_table[op].op = info->op;

        if (info->name != NULL) {
            // �����ڲ� proc
            assign_proc(context, info->name, info->op);
        }
    }
}


/******************************************************************************
                                ��������ʼ��
******************************************************************************/
/**
 * ��ʼ������
 * @param context
 * @return 0: ��ʼ��ʧ��; 1: ��ʼ���ɹ�
 */
static int interpreter_default_env_init(context_t context) {
    gc_var1(context, tmp);

    context->debug = 0;
#ifdef DEBUG_ONLY
    context->tracing_opcode = 0;
#endif
    context->repl_mode = 0;

    // ȫ�ַ��ű� global_symbol_table ������ hashset
    context->global_symbol_table =
            weak_hashset_make_op(context, GLOBAL_SYMBOL_TABLE_INIT_SIZE, DEFAULT_HASH_SET_MAP_LOAD_FACTOR);
    context->gensym_count = 0;
    // ȫ�� environment
    tmp = pair_make_op(context, IMM_UNIT, IMM_UNIT);
    pair_car(tmp) = hashmap_make_op(context, GLOBAL_ENVIRONMENT_INIT_SIZE, DEFAULT_HASH_SET_MAP_LOAD_FACTOR);
    context->global_environment = tmp;
    // ���� env ���ͱ��
    set_ext_type_environment(context->global_environment);
    set_immutable(context->global_environment);

    context->current_env = context->global_environment;
    context->scheme_stack = IMM_UNIT;

    // syntax_table
    // 30 ���������ʺ�, 30 > 17/0.75
    context->syntax_table = hashmap_make_op(context, 30, DEFAULT_HASH_SET_MAP_LOAD_FACTOR);

    context->load_stack = stack_make_op(context, MAX_LOAD_FILE_DEEP);
    // ÿ����һ���ļ�, Ҫ����һ���ʷ���������������ȼ�¼
    context->bracket_level_stack = stack_make_op(context, MAX_LOAD_FILE_DEEP);
    context->bracket_level = 0;

    // opcode ��ʼ��Ϊ 0, ��Ҫ������ȷ��ʼ��
    context->opcode = 0;
    context->value = IMM_UNIT;
    context->args = IMM_UNIT;
    context->code = IMM_UNIT;

    context->print_flag = 0;

    // Ĭ���������
    context->in_port = stdio_port_from_file_op(context, stdin, PORT_INPUT);
    context->out_port = stdio_port_from_file_op(context, stdout, PORT_OUTPUT);
    context->err_out_port = stdio_port_from_file_op(context, stderr, PORT_OUTPUT);
    context->load_port = IMM_UNIT;
    context->save_port = IMM_UNIT;

    // ��ʼ�� opcode dispatch_table ���ڲ� proc
    context->dispatch_table = raw_alloc(sizeof(op_code_info) * MAX_OP_CODE);
    memset(context->dispatch_table, 0, sizeof(op_code_info) * MAX_OP_CODE);
    notnull_or_return(context->dispatch_table, "[ERROR] context->dispatch_table alloc failed.", 0);
    init_opcode(context);

    // ��ʼ���ؼ���
    // todo ��ӹؼ��� (syntax) �ǵ��޸�����
    assign_syntax(context, "lambda", OP_LAMBDA0);
    assign_syntax(context, "quote", OP_QUOTE);
    assign_syntax(context, "define", OP_DEFINE0);
    assign_syntax(context, "if", OP_IF0);
    assign_syntax(context, "begin", OP_BEGIN);
    assign_syntax(context, "set!", OP_SET0);
    assign_syntax(context, "let", OP_LET0);
    assign_syntax(context, "let*", OP_LET_AST0);
    assign_syntax(context, "letrec", OP_LET_REC0);
    assign_syntax(context, "cond", OP_COND0);
    assign_syntax(context, "delay", OP_DELAY);
    assign_syntax(context, "and", OP_AND0);
    assign_syntax(context, "or", OP_OR0);
    assign_syntax(context, "cons-stream", OP_CONS_STREAM0);
    assign_syntax(context, "macro", OP_MACRO0);
    assign_syntax(context, "case", OP_CASE0);

    // else hack
    tmp = symbol_make_from_cstr_op(context, "else");
    set_immutable(tmp);
    new_slot_in_spec_env(context, tmp, IMM_TRUE, context->global_environment);

    // ��ʼ���ڲ��﷨����
    context->LAMBDA = symbol_make_from_cstr_op(context, "lambda");
    context->QUOTE = symbol_make_from_cstr_op(context, "quote");
    context->QUASIQUOTE = symbol_make_from_cstr_op(context, "quasiquote");
    context->UNQUOTE = symbol_make_from_cstr_op(context, "unquote");
    context->UNQUOTE_SPLICING = symbol_make_from_cstr_op(context, "unquote-splicing");
    context->FEED_TO = symbol_make_from_cstr_op(context, "=>");

    set_immutable(context->LAMBDA);
    set_immutable(context->FEED_TO);

    // ��ʼ���ڲ� hook ����
    context->ERROR_HOOK = symbol_make_from_cstr_op(context, "*error-hook*");
    context->COMPILE_HOOK = symbol_make_from_cstr_op(context, "*compile-hook*");

    // ������ʼ������
    context->init_done = 1;
    gc_release_var(context);
    return 1;
}

/**
 * ����������
 * @param heap_init_size
 * @param heap_growth_scale
 * @param heap_max_size
 * @return
 */
EXPORT_API context_t interpreter_create(size_t heap_init_size, size_t heap_growth_scale, size_t heap_max_size) {
    context_t context = context_make(heap_init_size, heap_growth_scale, heap_max_size);
    notnull_or_return(context, "interpreter_create failed", NULL);

    int ret = interpreter_default_env_init(context);
    if (ret) {
        return context;
    } else {
        interpreter_destroy(context);
        notnull_or_return(NULL, "interpreter_create failed", NULL);
    }
}

/**
 * ����������
 * @param context
 */
EXPORT_API void interpreter_destroy(context_t context) {
    // todo �޸� context ���޸� interpreter_destroy
    if (context == NULL) return;

#ifdef DEBUG_ONLY
    context->tracing_opcode = 0;
#endif
    context->debug = 0;
    context->repl_mode = 0;

    context->args = IMM_UNIT;
    context->code = IMM_UNIT;

    context->current_env = IMM_UNIT;
    context->scheme_stack = IMM_UNIT;

    context->opcode = 0;
    context->value = IMM_UNIT;

    context->load_stack = IMM_UNIT;

    context->syntax_table = IMM_UNIT;
    context->global_symbol_table = IMM_UNIT;
    context->global_environment = IMM_UNIT;
    context->gensym_count = 0;

    for (size_t i = 0; i < context->global_type_table_len; i++) {
        context->global_type_table[i].name = IMM_UNIT;
        context->global_type_table[i].getter = IMM_UNIT;
        context->global_type_table[i].setter = IMM_UNIT;
        context->global_type_table[i].to_string = IMM_UNIT;
    }

    context->LAMBDA = IMM_UNIT;
    context->QUOTE = IMM_UNIT;
    context->QUASIQUOTE = IMM_UNIT;
    context->UNQUOTE = IMM_UNIT;
    context->UNQUOTE_SPLICING = IMM_UNIT;
    context->FEED_TO = IMM_UNIT;

    context->ERROR_HOOK = IMM_UNIT;
    context->COMPILE_HOOK = IMM_UNIT;

    context->in_port = IMM_UNIT;
    context->out_port = IMM_UNIT;
    context->err_out_port = IMM_UNIT;

    context->load_port = IMM_UNIT;
    context->save_port = IMM_UNIT;

    context->saves = NULL;

    if (context->dispatch_table != NULL) {
        raw_free(context->dispatch_table);
    }

    // ȫ������, ��Ϊ�����ö���������
    gc_collect(context);

    context->init_done = 0;
    context_destroy(context);
}

/******************************************************************************
                         global_symbol_table ����
******************************************************************************/
/**
 * ���ɵ�ǰ������Ψһ symbol
 * @param context
 * @return symbol
 */
EXPORT_API OUT NOTNULL GC object gensym(REF NOTNULL context_t context) {
    assert(context != NULL);
    gc_var1(context, new_sym);

    char sym[30] = {0};
    for (; context->gensym_count < UINT64_MAX; context->gensym_count++) {
        snprintf(sym, sizeof(sym), "gensym_0x%016"PRIx64, context->gensym_count);
        new_sym = symbol_make_from_cstr_untracked_op(context, sym);
        if (is_imm_true(global_symbol_found(context, new_sym))) {
            // ��ͻ
            continue;
        } else {
            // ȫ�ַ��ű���û�ж�Ӧ����
            new_sym = global_symbol_add_from_symbol_obj(context, new_sym);
            gc_release_var(context);
            return new_sym;
        }
    }

    gc_release_var(context);
    return IMM_UNIT;
}
/**
 * ��ȫ�ַ��ű���� symbol ��������� symbol
 * @param context
 * @param symbol symbol ����
 * @param ��Ӻ�� symbol ����, ��������򷵻�ԭʼ����
 */
EXPORT_API OUT NOTNULL GC object
global_symbol_add_from_symbol_obj(REF NOTNULL context_t context, REF NOTNULL object symbol) {
    assert(context != NULL);
    assert(is_symbol(symbol));
    assert(is_weak_hashset(context->global_symbol_table));
    gc_param1(context, symbol);

    symbol = weak_hashset_put_op(context, context->global_symbol_table, symbol);
    gc_release_param(context);
    return symbol;
}

/**
 * ��ȫ�ַ��ű��в����Ƿ���ڴ˷���
 * <p>���ᴥ�� GC</p>
 * @param context
 * @param symbol
 * @return ���ڷ��� IMM_TRUE, ���򷵻� IMM_FALSE
 */
EXPORT_API OUT NOTNULL object
global_symbol_found(REF NOTNULL context_t context, REF NOTNULL object symbol) {
    assert(context != NULL);
    assert(is_symbol(symbol));

    return weak_hashset_contains_op(context, context->global_symbol_table, symbol);
}

/**
 * ����ȫ�ַ��ű�ȫ������
 * @param context
 * @return vector, vector �п��ܴ��� IMM_UNIT
 */
EXPORT_API OUT NOTNULL GC object
global_symbol_all_symbol(REF NOTNULL context_t context) {
    assert(context != NULL);
    return weak_hashset_to_vector_op(context, context->global_symbol_table);
}

/**
 * ���� symbol ���Ͷ���
 * <p>symbol_len() ������������ '\0', ���� object->symbol.len ���� '\0', ����Ϊ�˷�������ʱ��������С</p>
 * <p>�÷������ symbol ��ӽ�ȫ�ַ��ű�</p>
 * @param context
 * @param cstr C�ַ���, '\0'��β
 * @return
 */
EXPORT_API OUT NOTNULL GC object
symbol_make_from_cstr_op(REF NOTNULL context_t context, COPY char *cstr) {
    assert(context != NULL);

    gc_var1(context, symbol);
    symbol = symbol_make_from_cstr_untracked_op(context, cstr);
    symbol = global_symbol_add_from_symbol_obj(context, symbol);
    gc_release_var(context);
    return symbol;
}

/**
 * string ת symbol
 * <p>�÷������ symbol ��ӽ�ȫ�ַ��ű�</p>
 * @param context
 * @param string
 * @return
 */
EXPORT_API OUT NOTNULL GC object
string_to_symbol_op(REF NOTNULL context_t context, NOTNULL COPY object str) {
    assert(context != NULL);

    gc_param1(context, str);
    gc_var1(context, symbol);
    symbol = string_to_symbol_untracked_op(context, str);
    symbol = global_symbol_add_from_symbol_obj(context, symbol);
    gc_release_param(context);
    return symbol;
}

/**
 * string_buffer ת��Ϊ symbol, ���
 * <p>�÷������ symbol ��ӽ�ȫ�ַ��ű�</p>
 * @param context
 * @param str_buffer string_buffer
 * @return symbol
 */
EXPORT_API OUT NOTNULL GC object
string_buffer_to_symbol_op(REF NOTNULL context_t context, NOTNULL COPY object str_buffer) {
    assert(context != NULL);

    gc_param1(context, str_buffer);
    gc_var1(context, symbol);
    symbol = string_buffer_to_symbol_untracked_op(context, str_buffer);
    symbol = global_symbol_add_from_symbol_obj(context, symbol);
    gc_release_param(context);
    return symbol;
}


/******************************************************************************
                             scheme_stack ����
******************************************************************************/
EXPORT_API void scheme_stack_reset(context_t context) {
    assert(context != NULL);
    context->scheme_stack = IMM_UNIT;
}

EXPORT_API GC void scheme_stack_save(context_t context, enum opcode_e op, object args, object code) {
    assert(context != NULL);

    gc_param2(context, args, code);
    gc_var1(context, tmp);

    // ����ջ֡
    tmp = stack_frame_make_op(context, op, args, code, context->current_env);
    // ��ջջ
    context->scheme_stack = pair_make_op(context, tmp, context->scheme_stack);

    gc_release_param(context);
}

EXPORT_API object scheme_stack_return(context_t context, object value) {
    assert(context != NULL);
    assert(is_pair(context->scheme_stack) || context->scheme_stack == IMM_UNIT);

    // ����ֵ��ֵ
    context->value = value;

    if (context->scheme_stack == IMM_UNIT) {
        // ջ��
        // ���Ǹ�����, Ӧ����Ԫѭ������
        return IMM_UNIT;
    }

    // ȡ������ջ֡
    object stack_frame = pair_car(context->scheme_stack);
    // �ָ�����
    context->opcode = stack_frame_op(stack_frame);
    context->args = stack_frame_args(stack_frame);
    context->code = stack_frame_code(stack_frame);
    context->current_env = stack_frame_env(stack_frame);

    context->scheme_stack = pair_cdr(context->scheme_stack);
    // �˴���Ӧ�� return value
    return IMM_TRUE;
}

/******************************************************************************
                              environment ����
******************************************************************************/
/**
 * �� context->current_env ���ϲ��� env_slot
 * <p>���ᴥ�� GC</p>
 * @param context
 * @param symbol
 * @param all 1: �������ҵ�ȫ�� env; 0: ֻ���ҵ�ǰ env
 * @return env_slot / IMM_UNIT (δ�ҵ�)
 */
EXPORT_API object find_slot_in_current_env(REF NOTNULL context_t context, object symbol, int all) {
    return find_slot_in_spec_env(context, context->current_env, symbol, all);
}
/**
 * ���ض� env ���ϲ��� env_slot
 * <p>���ᴥ�� GC</p>
 * @param context
 * @param symbol
 * @param all 1: �������ҵ�ȫ�� env; 0: ֻ���ҵ�ǰ environment
 * @return env_slot / IMM_UNIT (δ�ҵ�)
 */
EXPORT_API object find_slot_in_spec_env(REF NOTNULL context_t context, object env, object symbol, int all) {
    assert(context != NULL);

    int first_env = 1;
    int found = 0;
    object ret_slot = IMM_UNIT;

    for (object cur = env; cur != IMM_UNIT; cur = pair_cdr(cur)) {

        if (is_hashmap(pair_car(cur))) {
            ret_slot = hashmap_get_op(context, pair_car(cur), symbol);
            // �Ҳ����Ļ� �պ÷��� IMM_UNIT
            break;
        } else {

            // ����Ļ�Ϊ���� env frame ��
            for (object slot = pair_car(cur); slot != IMM_UNIT; slot = env_slot_next(slot)) {
                assert(is_env_slot(slot));
                assert(is_symbol(env_slot_var(slot)));
                if (symbol_equals(context, env_slot_var(slot), symbol)) {
                    found = 1;
                    ret_slot = slot;
                    break;
                }
            }
        }

        // ֻ����һ��
        if (first_env) {
            first_env = 0;
            if (!all) break;
        }

        // �ҵ��Ļ�����
        if (found == 1) break;
    }

    return ret_slot;
}
/**
 * �ڵ�ǰ context->current_env ������ env_slot
 * @param context
 * @param symbol
 * @param value
 */
EXPORT_API GC void new_slot_in_current_env(context_t context, object symbol, object value) {
    new_slot_in_spec_env(context, symbol, value, context->current_env);
}
/**
 * ���ض� env frame ������ env_slot
 * @param context
 * @param symbol
 * @param value
 * @param env
 */
EXPORT_API GC void new_slot_in_spec_env(context_t context, object symbol, object value, object env) {
    assert(context != NULL);
    assert(is_symbol(symbol));
    assert(env != IMM_UNIT);

    gc_param3(context, symbol, value, env);
    gc_var1(context, slot);

    slot = env_slot_make_op(context, symbol, value, IMM_UNIT);
    set_immutable(slot);

    if (is_hashmap(pair_car(env))) {
        hashmap_put_op(context, pair_car(env), symbol, slot);
    } else {
        env_slot_next(slot) = pair_car(env);
        pair_car(env) = slot;
    }

    gc_release_param(context);
}
/**
 * �� context->current_env ��Ϊ�ϲ�, ������ frame, ��ֵ�� context->current_env
 * @param context
 * @param old_env һ���� context->current_env
 */
EXPORT_API GC void new_frame_push_current_env(context_t context) {
    new_frame_push_spec_env(context, context->current_env);
}
/**
 * �� old_env ��Ϊ�ϲ�, ������ frame, ��ֵ�� context->current_env
 * @param context
 * @param old_env һ���� context->current_env
 */
EXPORT_API GC void new_frame_push_spec_env(context_t context, object old_env) {
    assert(context != NULL);
    assert(old_env != IMM_UNIT);

    gc_param1(context, old_env);
    context->current_env = pair_make_op(context, IMM_UNIT, old_env);
    set_ext_type_environment(context->current_env);
    set_immutable(context->current_env);
    gc_release_param(context);
}


/******************************************************************************
                              load_stack ����
******************************************************************************/
/**
 * (load "") �������� port
 * @param context
 * @param file_name
 * @return �ɹ����� IMM_TRUE, ���� IMM_FALSE
 */
static GC object file_push(context_t context, object file_name) {
    assert(context != NULL);
    gc_param1(context, file_name);
    gc_var3(context, port, name, bracket_level);

    port = stdio_port_from_filename_op(context, file_name, PORT_INPUT);
    if (port != IMM_UNIT) {
        // load stack ���㽫�Զ�����
        context->load_stack = stack_push_auto_increase_op(context, context->load_stack, port, 50);
        bracket_level = i64_make_op(context, 0);
        context->bracket_level_stack = stack_push_auto_increase_op(context, context->bracket_level_stack, bracket_level,
                                                                   50);
        context->load_port = port;
    }

    gc_release_param(context);
    if (port != IMM_UNIT) return IMM_TRUE;
    else return IMM_FALSE;
}

static void file_pop(context_t context) {
    if (!stack_empty(context->load_stack)) {
        object port = stack_peek_op(context->load_stack);

        // ����ϴ��ļ����� �����Ƿ�ƥ��
        context->bracket_level = i64_getvalue(stack_peek_op(context->bracket_level_stack));

        // ��ջ
        stack_pop_op(context->load_stack);
        stack_pop_op(context->bracket_level_stack);

        assert(port != NULL);
        // ��ȷ�ر��ļ� (�����Ҫ�رյĻ�)
        if (is_stdio_port(port)) {
            stdio_finalizer(context, port);
        }
        // �ָ�֮ǰ�� port
        context->load_port = stack_peek_op(context->load_stack);
    }
}

/******************************************************************************
                                  ��ӡ�������
******************************************************************************/
/**
 * <p>���Ʒ����� #\ ��ʽ���</p>
 * <p>from tiny_scheme</p>
 * @param context
 * @param str string
 * @return string
 */
static GC object print_slash_string(context_t context, object str) {
    assert(context != NULL);
    gc_param1(context, str);
    gc_var2(context, ret, buffer);
    buffer = string_buffer_make_op(context, string_len(str) * 2);

    string_buffer_append_char_op(context, buffer, '"');
    for (size_t i = 0; i < string_len(str); i++) {
        unsigned char ch = (unsigned char) string_index(str, i);
        if (ch == 0xff || ch == '"' || ch < ' ' || ch == '\\') {
            string_buffer_append_char_op(context, buffer, '\\');
            switch (ch) {
                case '"':
                    string_buffer_append_char_op(context, buffer, '"');
                    break;
                case '\n':
                    string_buffer_append_char_op(context, buffer, 'n');
                    break;
                case '\t':
                    string_buffer_append_char_op(context, buffer, 't');
                    break;
                case '\r':
                    string_buffer_append_char_op(context, buffer, 'r');
                    break;
                case '\\':
                    string_buffer_append_char_op(context, buffer, '\\');
                    break;
                default: {
                    int d = ch / 16;
                    string_buffer_append_char_op(context, buffer, 'x');
                    if (d < 10) {
                        string_buffer_append_char_op(context, buffer, (char) (d + '0'));
                    } else {
                        string_buffer_append_char_op(context, buffer, (char) (d - 10 + 'A'));
                    }
                    d = ch % 16;
                    if (d < 10) {
                        string_buffer_append_char_op(context, buffer, (char) (d + '0'));
                    } else {
                        string_buffer_append_char_op(context, buffer, (char) (d - 10 + 'A'));
                    }
                }
            }
        } else {
            string_buffer_append_char_op(context, buffer, (char) ch);
        }
    }
    string_buffer_append_char_op(context, buffer, '"');

    ret = string_buffer_to_string_op(context, buffer);
    gc_release_param(context);
    return ret;
}

/**
 * atom->str
 * <p>from tiny_scheme</p>
 * @param context
 * @param obj
 * @param flag
 * <li>number   2, 8, 16: ���ֵĻ����ն�Ӧ�������, ������ 10 ����</li>
 * <li>char:    0: ֱ�����; ������� #\ ��ʽ</li>
 * <li>string:  0: ֱ�����; ������� #\ ��ʽ</li>
 * @return
 */
static GC object atom_to_string(context_t context, object obj, int flag) {
    assert(context != NULL);
    gc_param1(context, obj);
    gc_var1(context, str);

    char *p;

    if (is_ext_type(obj)) {
        if (is_ext_type_environment(context->args)) {
            str = string_make_from_cstr_op(context, "#<ENVIRONMENT>");
            gc_release_param(context);
            return str;
        } else if (is_ext_type_closure(context->args)) {
            str = string_make_from_cstr_op(context, "#<CLOSURE>");
            gc_release_param(context);
            return str;
        } else if (is_ext_type_macro(context->args)) {
            str = string_make_from_cstr_op(context, "#<MACRO>");
            gc_release_param(context);
            return str;
        } else if (is_ext_type_continuation(context->args)) {
            str = string_make_from_cstr_op(context, "#<CONTINUATION>");
            gc_release_param(context);
            return str;
        }
    }

    if (obj == IMM_UNIT) {
        p = "()";
    } else if (obj == IMM_TRUE) {
        p = "#t";
    } else if (obj == IMM_FALSE) {
        p = "#f";
    } else if (obj == IMM_EOF) {
        p = "#<EOF>";
    } else if (is_port(obj)) {
        p = "#<PORT>";
    } else if (is_number(obj)) {
        p = context->str_buffer;
        if (flag <= 1 || flag == 10) /* flag is the base for numbers if > 1 */ {
            if (is_i64(obj)) {
                snprintf(p, INTERNAL_STR_BUFFER_SIZE, "%"PRId64"", i64_getvalue(obj));
            } else {
                snprintf(p, INTERNAL_STR_BUFFER_SIZE, "%.10g", doublenum_getvalue(obj));
                /* r5rs says there must be a '.' (unless 'e'?) */
                size_t pos = strcspn(p, ".e");
                if (p[pos] == 0) {
                    p[pos] = '.'; /* not found, so add '.0' at the end */
                    p[pos + 1] = '0';
                    p[pos + 2] = 0;
                }
            }
        } else {
            int64_t v = i64_getvalue(obj);
            if (flag == 16) {
                if (v >= 0)
                    snprintf(p, INTERNAL_STR_BUFFER_SIZE, "%"PRIx64"", v);
                else
                    snprintf(p, INTERNAL_STR_BUFFER_SIZE, "-%"PRIx64"", -v);
            } else if (flag == 8) {
                if (v >= 0)
                    snprintf(p, INTERNAL_STR_BUFFER_SIZE, "%"PRIo64"", v);
                else
                    snprintf(p, INTERNAL_STR_BUFFER_SIZE, "-%"PRIo64"", -v);
            } else if (flag == 2) {
                uint64_t b = (uint64_t) ((v < 0) ? -v : v);
                p = &p[INTERNAL_STR_BUFFER_SIZE - 1];
                *p = 0;
                do {
                    *--p = (b & 1u) ? '1' : '0';
                    b >>= 1u;
                } while (b != 0);
                if (v < 0) *--p = '-';
            }
        }
    } else if (is_string(obj)) {
        if (!flag) {
            gc_release_param(context);
            return obj;
        } else {
            str = print_slash_string(context, obj);
            gc_release_param(context);
            return str;
        }
    } else if (is_imm_char(obj)) {
        int c = char_imm_getvalue(obj);
        p = context->str_buffer;
        if (!flag) {
            p[0] = (char) c;
            p[1] = 0;
        } else {
            switch (c) {
                case ' ':
                    p = "#\\space";
                    break;
                case '\n':
                    p = "#\\newline";
                    break;
                case '\r':
                    p = "#\\return";
                    break;
                case '\t':
                    p = "#\\tab";
                    break;
                default:
                    if (c < 32) {
                        snprintf(p, INTERNAL_STR_BUFFER_SIZE, "#\\x%x", c);
                        break;
                    }
                    snprintf(p, INTERNAL_STR_BUFFER_SIZE, "#\\%c", c);
                    break;
            }
        }
    } else if (is_symbol(obj)) {
        str = symbol_to_string_op(context, obj);
        gc_release_param(context);
        return str;
    } else if (is_syntax(obj)) {
        p = context->str_buffer;
        snprintf(p, INTERNAL_STR_BUFFER_SIZE, "#<%s SYNTAX %d>", symbol_get_cstr(syntax_get_symbol(obj)),
                 syntax_get_opcode(obj));
    } else if (is_promise(obj)) {
        if (promise_forced(obj)) {
            p = "#<forced PROMISE>";
        } else {
            p = "#<PROMISE>";
        }
    } else if (is_proc(obj)) {
        p = context->str_buffer;
        snprintf(p, INTERNAL_STR_BUFFER_SIZE, "#<%s PROCEDURE %d>", symbol_get_cstr(proc_get_symbol(obj)),
                 proc_get_opcode(obj));
    } else {
        // todo ��������� �ǵ��޸� atom_to_str
        p = "#<ERROR>";
    }

    str = string_make_from_cstr_op(context, p);
    assert(context->saves == &__gc_var_dream1__);
    gc_release_param(context);
    return str;
}

static GC void print_atom(context_t context, object obj, int flag) {
    gc_param1(context, obj);
    gc_var1(context, str);
    str = atom_to_string(context, obj, flag);
    port_put_string(context, context->out_port, str);
    gc_release_param(context);
}


/******************************************************************************
                                    Ԫѭ��
******************************************************************************/

/**
 * ���� OP_ERROR �Ĳ�������ת�� ERROR
 * <p>�쳣�ṹ</p>
 * <p>   '(error_message_str)</p>
 * <p>�� '(error_message_str object)</p>
 * @param context
 * @param message
 * @param obj
 * @return
 */
static object error_throw(context_t context, const char *message, object obj) {
    assert(context != 0);
    gc_param1(context, obj);
    gc_var5(context, strbuff, str, err_hook, tmp1, code);

    char format_buff[__Format_buff_size__];

    // �����쳣��Ϣ
    strbuff = string_buffer_make_op(context, 512);
    if (!stack_empty(context->load_stack) && is_stdio_port(stack_peek_op(context->load_stack)) &&
        stdio_port_get_file(stack_peek_op(context->load_stack)) != stdin) {
        // ��ʾ��������
        tmp1 = stack_peek_op(context->load_stack);
        string_buffer_append_cstr_op(context, strbuff, "(");
        string_buffer_append_string_op(context, strbuff, stdio_port_get_filename(tmp1));
        string_buffer_append_cstr_op(context, strbuff, " : ");
        snprintf(format_buff, __Format_buff_size__, "%zu", stdio_port_get_line(tmp1));
        string_buffer_append_cstr_op(context, strbuff, format_buff);
        string_buffer_append_cstr_op(context, strbuff, ") ");
    }
    string_buffer_append_cstr_op(context, strbuff, message);

    // *error-hook*
    err_hook = find_slot_in_current_env(context, context->ERROR_HOOK, 1);
    if (err_hook != IMM_UNIT) {
        // ���� error-hook ���ʽ����
        // '("message" (quote obj))
        if (obj == NULL) {
            context->code = IMM_UNIT;
        } else {
            tmp1 = pair_make_op(context, obj, IMM_UNIT);
            code = pair_make_op(context, context->QUOTE, tmp1);
            code = pair_make_op(context, code, IMM_UNIT);
        }
        str = string_buffer_to_string_op(context, strbuff);
        set_immutable(str);
        context->code = pair_make_op(context, str, code);

        // ������ʽ
        context->code = pair_make_op(context, env_slot_value(err_hook), context->code);
        // ��ֵ
        context->opcode = OP_EVAL;
        gc_release_param(context);
        return IMM_TRUE;
    }
    // *error-hook* end

    // Ĭ���쳣������
    if (obj == NULL) {
        context->args = IMM_UNIT;
    } else {
        context->args = pair_make_op(context, obj, IMM_UNIT);
    }

    str = string_buffer_to_string_op(context, strbuff);
    set_immutable(str);
    context->args = pair_make_op(context, str, context->args);
    context->opcode = OP_ERROR;

    gc_release_param(context);
    return IMM_TRUE;
}

#define Error_Throw_0_without_release(ctx, msg)       return error_throw((ctx), (msg), (NULL))
#define Error_Throw_1_without_release(ctx, msg, obj)    return error_throw((ctx), (msg), (obj))

/**
 * ����ڽ����̲���
 * @param context
 * @param vptr
 */
static object builtin_function_args_type_check(context_t context, op_code_info *vptr) {
    int ok_flag = 1;
    int64_t n = list_length(context->args);
    if (n == -1) {
        ok_flag = 0;
        snprintf(context->str_buffer, INTERNAL_STR_BUFFER_SIZE, "argument(s) not a list");
    }
    if (ok_flag && n < vptr->min_args_length) {
        // �������Ȳ���
        ok_flag = 0;
        snprintf(context->str_buffer, INTERNAL_STR_BUFFER_SIZE, "%s: needs%s %"PRId64" argument(s)",
                 vptr->name,
                 vptr->min_args_length == vptr->max_args_length ? "" : " at least",
                 vptr->min_args_length);
    }
    // max_args_length == ARGS_INF ��ʱ���������������ȼ��
    if (ok_flag && vptr->max_args_length != ARGS_INF && n > vptr->max_args_length) {
        // ��������
        ok_flag = 0;
        snprintf(context->str_buffer, INTERNAL_STR_BUFFER_SIZE, "%s: needs%s %"PRId64" argument(s)",
                 vptr->name,
                 vptr->min_args_length == vptr->max_args_length ? "" : " at most",
                 vptr->max_args_length);
    }
    if (ok_flag && vptr->args_type_check_table != NULL) {
        const char *type_check = vptr->args_type_check_table;

        object args = context->args;
        for (int64_t i = 0; i < n; i++) {
            int index = (int) (*type_check);
            type_test_func test = type_test_table[index].test;
            if (!test(pair_car(args))) {
                ok_flag = 0;
                snprintf(context->str_buffer, INTERNAL_STR_BUFFER_SIZE, "%s: argument %"PRId64" must be: %s",
                         vptr->name,
                         i + 1,
                         type_test_table[index].type_kind);
                break;
            }

            if (type_check[1] != 0) {
                // �ظ����һ������
                type_check++;
            }

            args = pair_cdr(args);
        }
    }

    if (!ok_flag) {
        Error_Throw_0_without_release(context, context->str_buffer);
    } else {
        return IMM_TRUE;
    }
}

static int is_file_interactive(context_t context) {
    return stack_len(context->load_stack) == 1 &&
           stdio_port_get_file(stack_peek_op(context->load_stack)) == stdin &&
           is_stdio_port(context->in_port);
}

EXPORT_API GC void eval_apply_loop(context_t context, enum opcode_e opcode) {
    context->opcode = opcode;
    while (1) {
        // ���� opcode ȡ�ö�Ӧ����
        assert(context->opcode >= 0 && context->opcode <= MAX_OP_CODE);
        op_code_info *vptr = &context->dispatch_table[context->opcode];
        assert(vptr != NULL);

#ifdef DEBUG_ONLY
        if (context->tracing_opcode) {
            snprintf(context->str_buffer, INTERNAL_STR_BUFFER_SIZE,
                     "OPCODE %d: %s\n",
                     context->opcode,
                     (vptr->name == NULL ? "unknown" : vptr->name));
            port_put_cstr(context, context->out_port, context->str_buffer);
        }
#endif

        // �ڽ�������������Ƿ���ȷ
        if (vptr->name != NULL) {
            builtin_function_args_type_check(context, vptr);
        }

        // ִ��һ��ѭ��
        vptr = &context->dispatch_table[context->opcode];
        if (vptr->func(context, context->opcode) == IMM_UNIT) {
            // ִ�н�������ִ���ʱ����
            // ���н���� context->ret
            return;
        }
    }
}

#define s_save(context, op, args, code) scheme_stack_save((context), (op), (args), (code))

#define s_goto(context, op) \
    do { \
        (context)->opcode = (op); \
        gc_release_var((context)); \
        return IMM_TRUE; \
    } while(0)
#define s_goto_without_release(context, op) \
    do { \
        (context)->opcode = (op); \
        gc_release_var((context)); \
        return IMM_TRUE; \
    } while(0)

#define s_return(context, value) \
    do { \
        gc_release_var((context)); \
        return scheme_stack_return((context), (value)); \
    } while(0)
#define s_return_without_release(context, value)  return scheme_stack_return((context), (value))

#define Error_Throw_0(ctx, msg) \
    do { \
        gc_release_var((context)); \
        return error_throw((ctx), (msg), (NULL)); \
    } while(0)
#define Error_Throw_1(ctx, msg, obj) \
    do { \
        gc_release_var((context)); \
        return error_throw((ctx), (msg), (obj)); \
    } while(0)


static object op_exec_repl(context_t context, enum opcode_e opcode) {
    gc_var2(context, tmp1, tmp2);

    switch (opcode) {
        case OP_LOAD:
            // (load "")
            // args: ("filename", IMM_UNIT)
            assert(is_pair(context->args));
            assert(is_imm_unit(pair_cdr(context->args)));

            if (is_file_interactive(context)) {
                port_put_cstr(context, context->out_port, "Loading ");
                port_put_string(context, context->out_port, pair_car(context->args));
                port_put_char(context, context->out_port, char_imm_make('\n'));
            }
            if (is_imm_false(file_push(context, pair_car(context->args)))) {
                Error_Throw_1(context, "unable to open", pair_car(context->args));
            } else {
                context->args = IMM_UNIT;
                s_goto(context, OP_TOP_LEVEL_SETUP);
            }
        case OP_TOP_LEVEL_SETUP:
            // args: IMM_UNIT

            // ��������ļ���β�����ѭ��
            // ���������� stdin, ��Զ�������� EOF, ��� repl �������ѭ��
            if (is_port_eof(context->load_port)) {
                // ��� load_stack ���һ���ļ����� EOF, ���� load_stack Ϊ��
                // ��ʱ����������ѭ��
                if (stack_len(context->load_stack) == 1) {
                    file_pop(context);
                    context->args = IMM_UNIT;
                    s_goto(context, OP_QUIT);
                } else if (stack_empty(context->load_stack)) {
                    // ��̫�������е�����; �����Է�֧
                    context->args = IMM_UNIT;
                    s_goto(context, OP_QUIT);
                } else {
                    file_pop(context);
                    s_return(context, context->value);
                }
            }

            // ��� port �� stdin ����ʾ��ʾ��
            if (is_file_interactive(context)) {
                context->current_env = context->global_environment;
                scheme_stack_reset(context);
                port_put_cstr(context, context->out_port, "\n"USER_OVERTURE);
            }

            // �����µ� repl ѭ��
            // �������Ų㼶���
            stack_set_top_op(context->bracket_level_stack, i64_imm_make(0));
            context->bracket_level = 0;
            // �� OP_READ_INTERNAL ����ǰ��ʱ���� context->in_port
            context->save_port = context->in_port;
            context->in_port = context->load_port;
            s_save(context, OP_TOP_LEVEL_SETUP, IMM_UNIT, IMM_UNIT);
            s_save(context, OP_VALUE_PRINT, IMM_UNIT, IMM_UNIT);
            s_save(context, OP_TOP_LEVEL, IMM_UNIT, IMM_UNIT);

            s_goto(context, OP_READ_INTERNAL);

        case OP_TOP_LEVEL:
            // OP_READ_INTERNAL ������, ����ֵ�� sexp
            context->code = context->value;
            context->in_port = context->save_port;
            // OP_EVAL Ӧ������ context->code == IMM_EOF �����
            s_goto(context, OP_EVAL);

        case OP_READ_INTERNAL:
            // return: sexp
            context->token = token(context);
            if (context->token == TOKEN_EOF) {
                // ֱ�ӵ����ļ���β, ���� OP_TOP_LEVEL
                s_return(context, IMM_EOF);
            }
            s_goto(context, OP_READ_SEXP);

        case OP_VALUE_PRINT:
            if (context->debug) {
                port_put_cstr(context, context->out_port, "\nReturn: ");
            }
            if (is_file_interactive(context)) {
                context->print_flag = 1;
                context->args = context->value;
                s_goto(context, OP_PRINT_OBJECT);
            }
        case OP_EVAL:
            if (context->debug) {
                s_save(context, OP_EVAL_REAL, context->args, context->code);
                context->args = context->code;
                port_put_cstr(context, context->out_port, "\nEval: ");
                s_goto(context, OP_PRINT_OBJECT);
            }
        case OP_EVAL_REAL:
            if (is_symbol(context->code)) {
                if (is_imm_true(symbol_is_syntax(context, context->code))) {
                    tmp1 = syntax_get_by_symbol(context, context->code);
                    s_return(context, tmp1);
                }
                tmp1 = find_slot_in_current_env(context, context->code, 1);
                if (tmp1 != IMM_UNIT) {
                    s_return(context, env_slot_value(tmp1));
                } else {
                    Error_Throw_1(context, "eval: unbound variable:", context->code);
                }
            } else if (is_pair(context->code) && !is_ext_type(context->code)) {
                tmp1 = pair_car(context->code);
                if (is_symbol(tmp1) && is_imm_true(symbol_is_syntax(context, tmp1))) {
                    context->code = pair_cdr(context->code);
                    tmp1 = syntax_get_by_symbol(context, tmp1);
                    s_goto(context, syntax_get_opcode(tmp1));
                } else {
                    // ��� sexp ��һ��Ԫ����ֵ����Ǻ�, ��Ժ�չ��������ֵ
                    s_save(context, OP_EXPAND_MACRO, IMM_UNIT, context->code);
                    context->code = pair_car(context->code);
                    // �ȶ� sexp ��һ��Ԫ����ֵ, ֮����ֵʣ�����ֱ����������
                    s_goto(context, OP_EVAL);
                }
            } else {
                s_return(context, context->code);
            }
        case OP_EXPAND_MACRO:
            if (is_ext_type_macro(context->value)) {
                // ��չ��
                // macro ʵ�����Ǹ� closure
                s_save(context, OP_EVAL_EXPANDED_MACRO, IMM_UNIT, IMM_UNIT);
                context->args = pair_make_op(context, context->code, IMM_UNIT);
                context->code = context->value;
                // macro չ��
                s_goto(context, OP_APPLY);
            } else {
                context->code = pair_cdr(context->code);
                s_goto(context, OP_EVAL_ARGS);
            }
        case OP_EVAL_ARGS:
            context->args = pair_make_op(context, context->value, context->args);
            if (is_pair(context->code)) {
                // ������Ҫ��ֵ�Ĳ���
                s_save(context, OP_EVAL_ARGS, context->args, pair_cdr(context->code));
                context->code = pair_car(context->code);
                context->args = IMM_UNIT;
                s_goto(context, OP_EVAL);
            } else {
                // ��ֵ����, args ���һ��ֵ�� procedure ����
                // ���ʹ�� reverse_in_place �ᵼ�� call/cc ����, ��Ϊ��Ť���˼�¼
                // OP_LET1, OP_LET1REC ͬ
                context->args = reverse(context, context->args);
                context->code = pair_car(context->args);
                context->args = pair_cdr(context->args);
                s_goto(context, OP_APPLY);
            }
        case OP_APPLY:
            if (context->debug) {
                s_save(context, OP_APPLY_REAL, context->args, context->code);
                context->print_flag = 1;
                port_put_cstr(context, context->out_port, "\nApply to: ");
                s_goto(context, OP_PRINT_OBJECT);
            }
        case OP_APPLY_REAL:
            // TODO ffi ʵ��
            if (is_proc(context->code)) {
                s_goto(context, proc_get_opcode(context->code));
            } else if (is_ext_type_closure(context->code) ||
                       is_ext_type_macro(context->code)) {

                new_frame_push_spec_env(context, closure_get_env(context->code));
                // ���� closure �����б�� context->args ����
                // ��� closure ����Ϊ��˵�����޲αհ�
                for (tmp1 = closure_get_args(context->code), tmp2 = context->args;
                     is_pair(tmp1);
                     tmp1 = pair_cdr(tmp1), tmp2 = pair_cdr(tmp2)) {
                    if (tmp2 == IMM_UNIT) {
                        Error_Throw_0(context, "apply: not enough arguments");
                    } else if (!is_pair(tmp2)) {
                        Error_Throw_0(context, "apply: argument 2 must be pair");
                    } else {
                        if (is_symbol(pair_car(tmp1))) {
                            new_slot_in_current_env(context, pair_car(tmp1), pair_car(tmp2));
                        } else {
                            Error_Throw_1(context, "syntax error in closure: not a symbol:", pair_car(tmp1));
                        }
                    }
                }

                // ��ʱ closure �Ĳ����ṹ�� list* ������ list, ������һ��
                if (tmp1 == IMM_UNIT) {
                    if (tmp2 != IMM_UNIT) {
                        Error_Throw_0(context, "apply: too many arguments");
                    }
                } else if (is_symbol(tmp1)) {
                    new_slot_in_current_env(context, tmp1, tmp2);
                } else if (tmp1 != IMM_UNIT) {
                    Error_Throw_1(context, "syntax error in closure: not a symbol:", tmp1);
                }

                context->code = pair_cdr(closure_get_code(context->code));
                // �����Ѿ��������
                context->args = IMM_UNIT;
                s_goto(context, OP_BEGIN);
            } else if (is_ext_type_continuation(context->code)) {
                // continuation �ٴα�����ʱ, ���ò�����Ϊ����ֵ
                context->scheme_stack = continuation_get_stack(context->code);
                if (context->args != IMM_UNIT) {
                    s_return(context, pair_car(context->args));
                } else {
                    s_return(context, IMM_UNIT);
                }
            } else {
                Error_Throw_0(context, "illegal function");
            }
        case OP_GENSYM:
            s_return(context, gensym(context));
        default:
            snprintf(context->str_buffer, INTERNAL_STR_BUFFER_SIZE, "opcode %d: illegal operator", opcode);
            Error_Throw_0(context, context->str_buffer);
    }

    gc_release_var(context);
    return IMM_TRUE;
}

static object op_exec_syntax(context_t context, enum opcode_e opcode) {
    assert(context != NULL);
    gc_var2(context, tmp1, tmp2);

    switch (opcode) {
        case OP_EVAL_EXPANDED_MACRO:
            // ��չ����ĺ���ֵ
            context->code = context->value;
            s_goto(context, OP_EVAL);
        case OP_LAMBDA0: {
            tmp1 = find_slot_in_current_env(context, context->COMPILE_HOOK, 1);
            if (tmp1 == IMM_UNIT) {
                // û�б���ʱ hook
                context->value = context->code;
                // �䵽 OP_LAMBDA1
            } else {
                // ����ҵ� *compile-hook*, (apply *compile-hook* `,@'code)
                s_save(context, OP_LAMBDA1, context->args, context->code);
                context->args = pair_make_op(context, context->code, IMM_UNIT);
                context->code = env_slot_value(tmp1);
                s_goto(context, OP_APPLY);
            }
        }
        case OP_LAMBDA1:
            // closure: (sexp env)
            tmp1 = closure_make_op(context, context->value, context->current_env);
            s_return(context, tmp1);
        case OP_MAKE_CLOSURE:
            // (make-closure sexp)     use current_env
            // (make-closure sexp env)
            tmp1 = pair_car(context->args);
            if (symbol_equals(context, pair_car(tmp1), context->LAMBDA)) {
                tmp1 = pair_cdr(context->args);
            }
            if (pair_cdr(context->args) == IMM_UNIT) {
                tmp2 = context->current_env;
            } else {
                tmp2 = pair_cadr(context->args);
            }
            tmp1 = closure_make_op(context, tmp1, tmp2);
            s_return(context, tmp1);
        case OP_GET_CLOSURE_CODE:
            context->args = pair_car(context->args);
            if (context->args == IMM_UNIT) {
                s_return(context, IMM_FALSE);
            } else if (is_ext_type_closure(context->args) || is_ext_type_macro(context->args)) {
                tmp1 = pair_make_op(context, context->LAMBDA, closure_get_code(context->args));
                s_return(context, tmp1);
            } else {
                s_return(context, IMM_FALSE);
            }
        case OP_QUOTE:
            s_return(context, pair_car(context->code));
        case OP_DEFINE0:
            if (is_pair(pair_car(context->code))) {
                // ���庯��
                // ������
                tmp1 = pair_caar(context->code);
                // ���� lambda ���ʽ
                tmp2 = pair_make_op(context, pair_cdar(context->code), pair_cdr(context->code));
                context->code = pair_make_op(context, context->LAMBDA, tmp2);
            } else {
                // �������
                // ������
                tmp1 = pair_car(context->code);
                // ���ʽ
                if (!is_pair(pair_cdr(context->code))) {
                    context->code = IMM_UNIT;
                } else {
                    context->code = pair_cadr(context->code);
                }
            }

            if (!is_symbol(tmp1)) {
                Error_Throw_0(context, "variable is not a symbol");
            }
            if (is_immutable(tmp1)) {
                Error_Throw_1(context, "define: unable to alter immutable", tmp1);
            }

            // symbol ������ context->code
            s_save(context, OP_DEFINE1, IMM_UNIT, tmp1);
            // �ȶ� define body ��ֵ, �ٻص� OP_DEFINE1 ��ֵ
            s_goto(context, OP_EVAL);
        case OP_DEFINE1:
            tmp1 = find_slot_in_current_env(context, context->code, 0);
            if (tmp1 != IMM_UNIT) {
                env_slot_value(tmp1) = context->value;
            } else {
                new_slot_in_current_env(context, context->code, context->value);
            }
            // ���ر����� context->code �� symbol
            s_return(context, context->code);
        case OP_MACRO0:
            if (is_pair(pair_car(context->code))) {
                // ((macro_name form) body...)
                tmp1 = pair_caar(context->code);
                // ���� lambda ���ʽ
                tmp2 = pair_make_op(context, pair_cdar(context->code), pair_cdr(context->code));
                context->code = pair_make_op(context, context->LAMBDA, tmp2);
            } else {
                // (macro_name (lambda (form) body...)
                tmp1 = pair_car(context->code);
                context->code = pair_cadr(context->code);
            }

            if (!is_symbol(tmp1)) {
                Error_Throw_0(context, "macro: variable is not a symbol");
            }
            s_save(context, OP_MACRO1, IMM_UNIT, tmp1);
            // �� lambda ���ʽ��ֵ, �õ��հ�
            s_goto(context, OP_EVAL);
        case OP_MACRO1:
            // OP_EVAL ����ֵ�� macro �հ�
            set_ext_type_macro(context->value);
            tmp1 = find_slot_in_current_env(context, context->code, 0);
            if (tmp1 != IMM_UNIT) {
                env_slot_value(tmp1) = context->value;
            } else {
                new_slot_in_current_env(context, context->code, context->value);
            }
            s_return(context, context->code);
        case OP_BEGIN:
            // (begin value)
            if (!is_pair(context->code)) {
                s_return(context, context->code);
            }
            // (begin sexp1 sexp2 ...)
            if (pair_cdr(context->code) != IMM_UNIT) {
                // �ݹ���ֵ
                s_save(context, OP_BEGIN, IMM_UNIT, pair_cdr(context->code));
            }
            // begin ���ʽ���� sexp ��ֵ�Ƿ���ֵ
            context->code = pair_car(context->code);
            s_goto(context, OP_EVAL);
        case OP_IF0:
            // ���� ����������֧
            s_save(context, OP_IF1, IMM_UNIT, pair_cdr(context->code));
            // �Բ���������ֵ
            context->code = pair_car(context->code);
            s_goto(context, OP_EVAL);
        case OP_IF1:
            // OP_EVAL ��ֵ����� context->value
            if (context->value != IMM_FALSE) {
                context->code = pair_car(context->code);
            } else {
                if (!is_pair(pair_cdr(context->code))) {
                    // (if #f 1) => '()
                    context->code = pair_cdr(context->code);
                } else {
                    context->code = pair_cadr(context->code);
                }
            }
            s_goto(context, OP_EVAL);
        case OP_LET0:
            context->args = IMM_UNIT;
            context->value = context->code;
            context->code = is_symbol(pair_car(context->code)) ? pair_cadr(context->code) : pair_car(context->code);
            s_goto(context, OP_LET1);
        case OP_LET1:
            // ���ʽ��ֵ������� args
            context->args = pair_make_op(context, context->value, context->args);
            if (is_pair(context->code)) {
                // �����Բ�����ֵ
                // code = ((var1 sexp1) (var2 sexp2) ...)
                // car(code)  =>  (var1 sexp1)
                // cdar(code) =>  (sexp1 . '())
                // cdr(code)  =>  ((var2 sexp2) ...)
                if (!is_pair(pair_car(context->code)) || !is_pair(pair_cdar(context->code))) {
                    Error_Throw_1(context, "Bad syntax of binding spec in let:", pair_car(context->code));
                }
                s_save(context, OP_LET1, context->args, pair_cdr(context->code));
                // code = sexp1
                context->code = pair_cadar(context->code);
                context->args = IMM_UNIT;
                // �Ա��ʽ��ֵ
                s_goto(context, OP_EVAL);
            } else {
                // ������ֵ����, �Բ�������
                // ����� named let, (car args) => let_name => #<symbol>
                context->args = reverse(context, context->args);
                context->code = pair_car(context->args);
                context->args = pair_cdr(context->args);
                s_goto(context, OP_LET2);
            }
        case OP_LET2:
            // ����ֵ�Ĳ������� env
            new_frame_push_current_env(context);
            // ������������ֵ, ֱ�����������
            for (tmp1 = is_symbol(pair_car(context->code)) ?
                        pair_cadr(context->code) : pair_car(context->code), tmp2 = context->args;
                 tmp2 != IMM_UNIT;
                 tmp1 = pair_cdr(tmp1), tmp2 = pair_cdr(tmp2)) {
                new_slot_in_current_env(context, pair_caar(tmp1), pair_car(tmp2));
            }
            // ����� named symbol, ���� closure
            if (is_symbol(pair_car(context->code))) {
                // ���� let �ṹ, �� let �󶨵ķ��������� closure
                // let  => (let let_name ((var1 sexp1) (var2 sexp2) ...) body...)
                // code => (let_name ((var1 sexp1) (var2 sexp2) ...))
                // tmp1 => ((var1 sexp1) (var2 sexp2) ...)
                for (tmp1 = pair_cadr(context->code), context->args = IMM_UNIT;
                     tmp1 != IMM_UNIT;
                     tmp1 = pair_cdr(tmp1)) {
                    if (!is_pair(tmp1)) {
                        Error_Throw_1(context, "Bad syntax of binding in let :", tmp1);
                    }
                    if (list_length(tmp1) < 0) {
                        // �Ƿ� list
                        Error_Throw_1(context, "Bad syntax of binding in let: ", tmp1);
                    }
                    // pair_caar(tmp1) => var1
                    context->args = pair_make_op(context, pair_caar(tmp1), context->args);
                }
                // let �󶨷�����
                // tmp1 => (var1 var2 ...)
                tmp1 = reverse_in_place(context, IMM_UNIT, context->args);
                // tmp2 => (body...)
                tmp2 = pair_cddr(context->code);
                // closure code
                tmp1 = pair_make_op(context, tmp1, tmp2);
                // ���� closure
                tmp2 = closure_make_op(context, tmp1, context->current_env);
                // ����
                new_slot_in_current_env(context, pair_car(context->code), tmp2);
                // body... ���浽 code
                context->code = pair_cddr(context->code);
                context->args = IMM_UNIT;
            } else {
                // �� named let
                context->code = pair_cdr(context->code);
                context->args = IMM_UNIT;
            }
            // ��ת�� OP_BEGIN ��ʼִ�� body...
            s_goto(context, OP_BEGIN);
        case OP_LET_AST0:
            // let* => (let* ((var1 sexp1) (var2 sexp2) ...) body...)
            // code => (((var1 sexp1) (var2 sexp2) ...) body...)
            if (pair_car(context->code) == IMM_UNIT) {
                // û�ж���Ļ�
                new_frame_push_current_env(context);
                context->code = pair_cdr(context->code);
                // ֱ����ת OP_BEGIN
                s_goto(context, OP_BEGIN);
            }
            // ����Ļ���� let* ���ʽ�ṹ
            // car(code)    =>  ((var1 sexp1) (var2 sexp2) ...)
            // caar(code)   =>  (var1 sexp1)
            // cdaar(code)  =>  (sexp1)
            if (!is_pair(pair_car(context->code)) || !is_pair(pair_caar(context->code)) ||
                !is_pair(pair_cdaar(context->code))) {
                Error_Throw_1(context, "Bad syntax of binding spec in let* :", pair_car(context->code));
            }
            // �� sexp1 ��ֵ�� push new env_frame
            s_save(context, OP_LET_AST1, pair_cdr(context->code), pair_car(context->code));
            // cadaar(code) =>  sexp1
            context->code = pair_cadaar(context->code);
            s_goto(context, OP_EVAL);
        case OP_LET_AST1:
            new_frame_push_current_env(context);
            s_goto(context, OP_LET_AST2);
        case OP_LET_AST2:
            // ���� OP_LET_AST0
            // args         => body...
            // code         => ((var1 sexp1) (var2 sexp2) ...)
            // (caar code)  => var1
            // value        => (eval sexp1)
            new_slot_in_current_env(context, pair_caar(context->code), context->value);
            // code ����
            // code         => sexp2
            context->code = pair_cdr(context->code);

            // ��� code û�н���, �����ݹ���ֵ
            if (is_pair(context->code)) {
                s_save(context, OP_LET_AST2, context->args, context->code);
                // (cadar code) => sexp2
                context->code = pair_cadar(context->code);
                context->args = IMM_UNIT;
                s_goto(context, OP_EVAL);
            } else {
                // ȫ�� sexp_n ��ֵ����
                // code     => body...
                context->code = context->args;
                context->args = IMM_UNIT;
                // ��ת�� OP_BEGIN
                s_goto(context, OP_BEGIN);
            }
        case OP_LET_REC0:
            // letrec   =>  (letrec ((var1 sexp1) (var2 sexp2) ...) body...)
            new_frame_push_current_env(context);
            context->args = IMM_UNIT;
            context->value = context->code;
            context->code = pair_car(context->code);
            s_goto(context, OP_LET_REC1);
        case OP_LET_REC1:
            // args     =>  ()
            // code     =>  ((var1 sexp1) (var2 sexp2) ...)
            // value    =>  (((var1 sexp1) (var2 sexp2) ...) body...)

            context->args = pair_make_op(context, context->value, context->args);
            // args     =>  ((((var1 sexp1) (var2 sexp2) ...) body...) . ())
            if (is_pair(context->code)) { /* continue */
                // (car code)   =>  (var1 sexp1)
                // (cdar code)  =>  (sexp1)
                if (!is_pair(pair_car(context->code)) || !is_pair(pair_cdar(context->code))) {
                    Error_Throw_1(context, "Bad syntax of binding spec in letrec :",
                                  pair_car(context->code));
                }
                // args =>  (((var1 sexp1) (var2 sexp2) ...) . ())
                // code =>  ((var2 sexp2) ...)
                // ���� (cdr code) ������һ�εݹ���ֵ
                s_save(context, OP_LET_REC1, context->args, pair_cdr(context->code));
                // (cadar code) => sexp1
                // �� (var1 sexp1) �е� sexp1 ������ֵ
                context->code = pair_cadar(context->code);
                context->args = IMM_UNIT;
                s_goto(context, OP_EVAL);
            } else {
                // ��ֵ����
                // args =>  ((((var1 sexp1) (var2 sexp2) ...) body...) . (eval_sexp1 eval_sexp2 ...))
                context->args = reverse(context, context->args);
                // code =>  (((var1 sexp1) (var2 sexp2) ...) body...)
                context->code = pair_car(context->args);
                // args =>  (eval_sexp1 eval_sexp2 ...)
                context->args = pair_cdr(context->args);
                s_goto(context, OP_LET_REC2);
            }
        case OP_LET_REC2:
            // ���� OP_LET_REC1
            // args         =>  (eval_sexp1 eval_sexp2 ...)
            // code         =>  (((var1 sexp1) (var2 sexp2) ...) body...)
            // (caar code)  =>  var1
            // ����������, ��� env
            // tmp1         =>  ((var1 sexp1) (var2 sexp2) ...)
            // tmp2         =>  args
            for (tmp1 = pair_car(context->code), tmp2 = context->args;
                 tmp2 != IMM_UNIT;
                 tmp1 = pair_cdr(tmp1), tmp2 = pair_cdr(tmp2)) {
                // (caar tmp1)  =>  var1
                // (car tmp2)   =>  eval_sexp1
                new_slot_in_current_env(context, pair_caar(tmp1), pair_car(tmp2));
            }
            // code =>  (body...)
            context->code = pair_cdr(context->code);
            context->args = IMM_UNIT;
            s_goto(context, OP_BEGIN);
        case OP_SET0:
            if (!is_symbol(pair_car(context->code))) {
                Error_Throw_1(context, "set!: variable cannot be", pair_car(context->code));
            }
            if (is_immutable(pair_car(context->code))) {
                Error_Throw_1(context, "set!: unable to alter immutable variable", pair_car(context->code));
            }
            s_save(context, OP_SET1, IMM_UNIT, pair_car(context->code));
            context->code = pair_cadr(context->code);
            s_goto(context, OP_EVAL);
        case OP_SET1:
            tmp1 = find_slot_in_current_env(context, context->code, 1);
            if (tmp1 != IMM_UNIT) {
                env_slot_value(tmp1) = context->value;
                s_return(context, context->value);
            } else {
                Error_Throw_1(context, "set!: unbound variable:", context->code);
            }
        case OP_COND0:
            // (cond (cond1 sexp1) (cond2 sexp2) ... (else sexp_n))
            // code         =>  ((cond1 sexp1) (cond2 sexp2) ... (else sexp_n))
            // (car code)   =>  (cond1 sexp1)
            if (!is_pair(context->code) || !is_pair(pair_car(context->code))) {
                Error_Throw_0(context, "syntax error in cond");
            }
            s_save(context, OP_COND1, IMM_UNIT, context->code);
            context->code = pair_caar(context->code);
            s_goto(context, OP_EVAL);
        case OP_COND1:
            // code =>  ((cond1 sexp1) (cond2 sexp2) ... (else sexp_n))
            if (is_imm_true(context->value)) {
                // code => (sexp1)
                //        | (=> (lambda (arg) ...))
                context->code = pair_cdar(context->code);
                if (context->code == IMM_UNIT) {
                    s_return(context, context->value);
                }
                if (symbol_equals(context, pair_car(context->code), context->FEED_TO)) {
                    if (!is_pair(pair_cdr(context->code))) {
                        Error_Throw_0(context, "syntax error in cond");
                    }
                    tmp1 = pair_make_op(context, context->value, IMM_UNIT);
                    tmp2 = pair_make_op(context, context->QUOTE, tmp1);
                    tmp2 = pair_make_op(context, tmp2, IMM_UNIT);
                    // code =>  ((lambda (x) ...) (quote value))
                    context->code = pair_make_op(context, pair_cadr(context->code), tmp2);
                    s_goto(context, OP_EVAL);
                }
                s_goto(context, OP_BEGIN);
            } else {
                // û����
                context->code = pair_cdr(context->code);
                if (context->code == IMM_UNIT) {
                    s_return(context, IMM_UNIT);
                } else {
                    if (!is_pair(pair_car(context->code))) {
                        Error_Throw_0(context, "syntax error in cond");
                    }
                    // ������� (condition [=>] ...)
                    s_save(context, OP_COND1, IMM_UNIT, context->code);
                    context->code = pair_caar(context->code);
                    s_goto(context, OP_EVAL);
                }
            }
        case OP_AND0:
            if (context->code == IMM_UNIT) {
                s_return(context, IMM_TRUE);
            }
            s_save(context, OP_AND1, IMM_UNIT, pair_cdr(context->code));
            context->code = pair_car(context->code);
            s_goto(context, OP_EVAL);
        case OP_AND1:
            if (is_imm_false(context->value)) {
                s_return(context, context->value);
            } else if (context->code == IMM_UNIT) {
                s_return(context, context->value);
            } else {
                s_save(context, OP_AND1, IMM_UNIT, pair_cdr(context->code));
                context->code = pair_car(context->code);
                s_goto(context, OP_EVAL);
            }
        case OP_OR0:
            if (context->code == IMM_UNIT) {
                s_return(context, IMM_FALSE);
            }
            s_save(context, OP_OR1, IMM_UNIT, pair_cdr(context->code));
            context->code = pair_car(context->code);
            s_goto(context, OP_EVAL);
        case OP_OR1:
            if (is_imm_true(context->value)) {
                s_return(context, context->value);
            } else if (context->code == IMM_UNIT) {
                s_return(context, context->value);
            } else {
                s_save(context, OP_OR1, IMM_UNIT, pair_cdr(context->code));
                context->code = pair_car(context->code);
                s_goto(context, OP_EVAL);
            }
        case OP_DELAY:
            // ����Ϊ�յıհ�
            // ('() context->code)
            tmp1 = pair_make_op(context, IMM_UNIT, context->code);
            tmp2 = closure_make_op(context, tmp1, context->current_env);
            tmp2 = promise_make_op(context, tmp2);
            s_return(context, tmp2);
        case OP_FORCE:
            context->code = pair_car(context->args);
            if (is_promise(context->code)) {
                if (promise_forced(context->code)) {
                    s_return(context, promise_get_value(context->code));
                }
                s_save(context, OP_SAVE_FORCED, IMM_UNIT, context->code);
                // ���� closure
                context->code = promise_get_value(context->code);
                context->args = IMM_UNIT;
                s_goto(context, OP_APPLY);
            } else {
                s_return(context, context->code);
            }
        case OP_FORCED_P:
            if (is_promise(pair_car(context->args))) {
                s_return(context, setbool(promise_forced(pair_car(context->args))));
            } else {
                Error_Throw_1(context, "variable is not a promise", pair_car(context->args));
            }
        case OP_SAVE_FORCED:
            promise_get_value(context->code) = context->value;
            promise_forced(context->code) = 1;
            s_return(context, context->value);
        case OP_CONS_STREAM0:
            // (cons-stream a b)    =>  (cons a (delay b))
            s_save(context, OP_CONS_STREAM1, IMM_UNIT, pair_cdr(context->code));
            context->code = pair_car(context->code);
            s_goto(context, OP_EVAL);
        case OP_CONS_STREAM1:
            // �޲αհ�
            tmp1 = pair_make_op(context, IMM_UNIT, context->code);
            tmp2 = closure_make_op(context, tmp1, context->current_env);
            // promise
            tmp2 = promise_make_op(context, tmp2);
            tmp2 = pair_make_op(context, context->value, tmp2);
            s_return(context, tmp2);
        case OP_CASE0:
            // (case test-expr case-clause ...)
            s_save(context, OP_CASE1, IMM_UNIT, pair_cdr(context->code));
            context->code = pair_car(context->code);
            // �ȶ� test-expr ��ֵ
            s_goto(context, OP_EVAL);
        case OP_CASE1: {
            // value    =>  eval_test_expr
            // code     =>  ( [(1 2 3) ...begin]
            //                [(4 5 6) ...begin] ...)
            // ���� case-clause
            for (tmp1 = context->code; tmp1 != IMM_UNIT; tmp1 = pair_cdr(tmp1)) {
                // tmp2 =>  (1 2 3)
                tmp2 = pair_caar(tmp1);
                if (!is_pair(tmp2)) {
                    break;
                }
                for (; tmp2 != IMM_UNIT; tmp2 = pair_cdr(tmp2)) {
                    if (eqv(context, pair_car(tmp2), context->value)) {
                        break;
                    }
                }
                if (tmp2 != IMM_UNIT) {
                    break;
                }
            }
            if (tmp1 != IMM_UNIT) {
                if (is_pair(pair_caar(tmp1))) {
                    context->code = pair_cdar(tmp1);
                    s_goto(context, OP_BEGIN);
                } else {
                    s_save(context, OP_CASE2, IMM_UNIT, pair_cdar(tmp1));
                    context->code = pair_caar(tmp1);
                    s_goto(context, OP_EVAL);
                }
            } else {
                s_return(context, IMM_UNIT);
            }
        }
        case OP_CASE2:
            if (is_imm_true(context->value)) {
                s_goto(context, OP_BEGIN);
            } else {
                s_return(context, IMM_UNIT);
            }
        case OP_CONTINUATION:
            // call-with-current-continuation
            // (call/cc procedure)
            // value ��Ϊ����ֵ
            context->code = pair_car(context->args);
            tmp1 = continuation_make_op(context, context->scheme_stack);
            tmp2 = pair_make_op(context, tmp1, IMM_UNIT);
            // ����ǰջָ�� (continuation) ���ݸ� procedure
            context->args = tmp2;
            // (apply procedure continuation)
            // procedure �ķ���ֵ��Ϊ call/cc ����ֵ
            s_goto(context, OP_APPLY);
        default:
            snprintf(context->str_buffer, INTERNAL_STR_BUFFER_SIZE, "opcode %d: illegal operator", opcode);
            Error_Throw_0(context, context->str_buffer);
    }

    gc_release_var(context);
    return IMM_TRUE;
}

static int is_zero(object num) {
    if (is_i64(num)) {
        return num == ZERO;
    } else {
        double x = doublenum_getvalue(num);
        return x > -DBL_MIN && x < DBL_MIN;
    }
}

static object op_exec_compute(context_t context, enum opcode_e opcode) {
    assert(context != NULL);
    gc_var2(context, tmp1, tmp2);
    number_compare_func compare = NULL;

    switch (opcode) {
        case OP_EXP:
            tmp1 = pair_car(context->args);
            if (is_i64(tmp1)) {
                tmp2 = doublenum_make_op(context, exp((double) i64_getvalue(tmp1)));
            } else {
                tmp2 = doublenum_make_op(context, exp(doublenum_getvalue(tmp1)));
            }
            s_return(context, tmp2);
        case OP_LOG:
            tmp1 = pair_car(context->args);
            if (is_i64(tmp1)) {
                tmp2 = doublenum_make_op(context, log((double) i64_getvalue(tmp1)));
            } else {
                tmp2 = doublenum_make_op(context, log(doublenum_getvalue(tmp1)));
            }
            s_return(context, tmp2);
        case OP_SIN:
            tmp1 = pair_car(context->args);
            if (is_i64(tmp1)) {
                tmp2 = doublenum_make_op(context, sin((double) i64_getvalue(tmp1)));
            } else {
                tmp2 = doublenum_make_op(context, sin(doublenum_getvalue(tmp1)));
            }
            s_return(context, tmp2);
        case OP_COS:
            tmp1 = pair_car(context->args);
            if (is_i64(tmp1)) {
                tmp2 = doublenum_make_op(context, cos((double) i64_getvalue(tmp1)));
            } else {
                tmp2 = doublenum_make_op(context, cos(doublenum_getvalue(tmp1)));
            }
            s_return(context, tmp2);
        case OP_TAN:
            tmp1 = pair_car(context->args);
            if (is_i64(tmp1)) {
                tmp2 = doublenum_make_op(context, tan((double) i64_getvalue(tmp1)));
            } else {
                tmp2 = doublenum_make_op(context, tan(doublenum_getvalue(tmp1)));
            }
            s_return(context, tmp2);
        case OP_ASIN:
            tmp1 = pair_car(context->args);
            if (is_i64(tmp1)) {
                tmp2 = doublenum_make_op(context, asin((double) i64_getvalue(tmp1)));
            } else {
                tmp2 = doublenum_make_op(context, asin(doublenum_getvalue(tmp1)));
            }
            s_return(context, tmp2);
        case OP_ACOS:
            tmp1 = pair_car(context->args);
            if (is_i64(tmp1)) {
                tmp2 = doublenum_make_op(context, acos((double) i64_getvalue(tmp1)));
            } else {
                tmp2 = doublenum_make_op(context, acos(doublenum_getvalue(tmp1)));
            }
            s_return(context, tmp2);
        case OP_ATAN:
            tmp1 = pair_car(context->args);
            if (is_i64(tmp1)) {
                tmp2 = doublenum_make_op(context, atan((double) i64_getvalue(tmp1)));
            } else {
                tmp2 = doublenum_make_op(context, atan(doublenum_getvalue(tmp1)));
            }
            s_return(context, tmp2);
        case OP_SQRT:
            tmp1 = pair_car(context->args);
            if (is_i64(tmp1)) {
                tmp2 = doublenum_make_op(context, sqrt((double) i64_getvalue(tmp1)));
            } else {
                tmp2 = doublenum_make_op(context, sqrt(doublenum_getvalue(tmp1)));
            }
            s_return(context, tmp2);
        case OP_EXPT: {
            double result;
            int real_result = 1;
            tmp1 = pair_car(context->args);
            tmp2 = pair_cadr(context->args);
            if (is_i64(tmp1) && is_i64(tmp2)) {
                real_result = 0;
            }
            double left, right;
            if (is_i64(tmp1)) {
                left = (double) i64_getvalue(tmp1);
            } else {
                left = doublenum_getvalue(tmp1);
            }
            if (is_i64(tmp2)) {
                right = (double) i64_getvalue(tmp2);
            } else {
                right = doublenum_getvalue(tmp2);
            }
            // R5RS ����
            // �Ƴ��� if Ϊ R6RS ����
            if (left == 0 && right < 0) {
                result = 0.0;
            } else {
                result = pow(left, right);
            }
            // ȷ�� int64_t �����������
            if (!real_result) {
                int64_t result_as_int64 = (int64_t) result;
                if (result != (double) result_as_int64) real_result = 1;
            }
            if (real_result) {
                s_return(context, doublenum_make_op(context, result));
            } else {
                s_return(context, i64_make_op(context, (int64_t) result));
            }
        }
        case OP_FLOOR:
            tmp1 = pair_car(context->args);
            if (is_i64(tmp1)) {
                tmp2 = doublenum_make_op(context, floor((double) i64_getvalue(tmp1)));
            } else {
                tmp2 = doublenum_make_op(context, floor(doublenum_getvalue(tmp1)));
            }
            s_return(context, tmp2);
        case OP_CEILING:
            tmp1 = pair_car(context->args);
            if (is_i64(tmp1)) {
                tmp2 = doublenum_make_op(context, ceil((double) i64_getvalue(tmp1)));
            } else {
                tmp2 = doublenum_make_op(context, ceil(doublenum_getvalue(tmp1)));
            }
            s_return(context, tmp2);
        case OP_TRUNCATE: {
            double x;
            tmp1 = pair_car(context->args);
            x = doublenum_getvalue(tmp1);
            if (x > 0) {
                s_return(context, doublenum_make_op(context, floor(x)));
            } else {
                s_return(context, doublenum_make_op(context, ceil(x)));
            }
        }
        case OP_ROUND:
            tmp1 = pair_car(context->args);
            if (is_i64(tmp1)) {
                s_return(context, tmp1);
            } else {
                s_return(context, doublenum_make_op(context, round_per_R5RS(doublenum_getvalue(tmp1))));
            }
        case OP_ADD: {
            tmp1 = ZERO;
            for (tmp2 = context->args; tmp2 != IMM_UNIT; tmp2 = pair_cdr(tmp2)) {
                tmp1 = number_add(context, tmp1, pair_car(tmp2));
            }
            s_return(context, tmp1);
        }
        case OP_SUB: {
            if (pair_cdr(context->args) == IMM_UNIT) {
                tmp1 = ZERO;
                tmp2 = context->args;
            } else {
                tmp1 = pair_car(context->args);
                tmp2 = pair_cdr(context->args);
            }
            for (; tmp2 != IMM_UNIT; tmp2 = pair_cdr(tmp2)) {
                tmp1 = number_sub(context, tmp1, pair_car(tmp2));
            }
            s_return(context, tmp1);
        }
        case OP_MUL: {
            tmp1 = ONE;
            for (tmp2 = context->args; tmp2 != IMM_UNIT; tmp2 = pair_cdr(tmp2)) {
                tmp1 = number_mul(context, tmp1, pair_car(tmp2));
            }
            s_return(context, tmp1);
        }
        case OP_DIV: {
            if (pair_cdr(context->args) == IMM_UNIT) {
                tmp1 = ONE;
                tmp2 = context->args;
            } else {
                tmp1 = pair_car(context->args);
                tmp2 = pair_cdr(context->args);
            }
            for (; tmp2 != IMM_UNIT; tmp2 = pair_cdr(tmp2)) {
                if (!is_zero(pair_car(tmp2))) {
                    tmp1 = number_sub(context, tmp1, pair_car(tmp2));
                } else {
                    Error_Throw_0(context, "/: division by zero");
                }
            }
            s_return(context, tmp1);
        }
        case OP_INT_DIV: {
            if (pair_cdr(context->args) == IMM_UNIT) {
                tmp1 = ONE;
                tmp2 = context->args;
            } else {
                tmp1 = pair_car(context->args);
                tmp2 = pair_cdr(context->args);
            }
            for (; tmp2 != IMM_UNIT; tmp2 = pair_cdr(tmp2)) {
                if (!is_zero(pair_car(tmp2))) {
                    tmp1 = number_int_div(context, tmp1, pair_car(tmp2));
                } else {
                    Error_Throw_0(context, "/: division by zero");
                }
            }
            s_return(context, tmp1);
        }
        case OP_REM:
            tmp1 = pair_car(context->args);
            tmp2 = pair_cadr(context->args);
            if (!is_zero(tmp2)) {
                s_return(context, number_rem(context, tmp1, tmp2));
            } else {
                Error_Throw_0(context, "remainder: division by zero");
            }
        case OP_MOD:
            tmp1 = pair_car(context->args);
            tmp2 = pair_cadr(context->args);
            if (!is_zero(tmp2)) {
                s_return(context, number_mod(context, tmp1, tmp2));
            } else {
                Error_Throw_0(context, "remainder: division by zero");
            }
        case OP_NUMBER_EQ:
        case OP_LESS:
        case OP_GRE:
        case OP_LEQ:
        case OP_GEQ:
            switch (opcode) {
                case OP_NUMBER_EQ:
                    compare = number_eq;
                    break;
                case OP_LESS:
                    compare = number_lt;
                    break;
                case OP_GRE:
                    compare = number_gt;
                    break;
                case OP_LEQ:
                    compare = number_le;
                    break;
                case OP_GEQ:
                    compare = number_ge;
                    break;
                default:
                    snprintf(context->str_buffer, INTERNAL_STR_BUFFER_SIZE, "opcode %d: illegal operator", opcode);
                    Error_Throw_0(context, context->str_buffer);
            }
            tmp1 = pair_car(context->args);
            for (tmp2 = pair_cdr(context->args); tmp2 != IMM_UNIT; tmp2 = pair_cdr(tmp2)) {
                if (!compare(tmp1, pair_car(tmp2))) {
                    s_return(context, IMM_FALSE);
                }
                tmp1 = pair_car(tmp2);
            }
            s_return(context, IMM_TRUE);
        case OP_CAR:
            // args     =>  (sexp1 sexp2 ...)
            // sexp1    =>  (a b ...)
            s_return(context, pair_caar(context->args));
        case OP_CDR:
            s_return(context, pair_cdar(context->args));
        case OP_CONS:
            // args     =>  (a . b)
            // a        =>  a
            // b        =>  (b)
            pair_cdr(context->args) = pair_cadr(context->args);
            s_return(context, context->args);
        case OP_SET_CAR:
            if (!is_immutable(pair_car(context->args))) {
                // args:    =>  (sexp1 value)
                // sexp1:   =>  (a . (b . ...)
                // value:   =>  (v . ())
                // caar:    =>  a
                // cadr:    =>  v
                pair_caar(context->args) = pair_cadr(context->args);
                s_return(context, pair_car(context->args));
            } else {
                Error_Throw_0(context, "set-car!: unable to alter immutable pair");
            }
        case OP_SET_CDR:
            if (!is_immutable(pair_car(context->args))) {
                // args:    =>  (sexp1 value)
                // sexp1:   =>  (a . (b . ...)
                // value:   =>  (v . ())
                // cdar:    =>  b
                // cadr:    =>  v
                pair_cdar(context->args) = pair_cadr(context->args);
                s_return(context, pair_car(context->args));
            } else {
                Error_Throw_0(context, "set-car!: unable to alter immutable pair");
            }
        default:
            snprintf(context->str_buffer, INTERNAL_STR_BUFFER_SIZE, "opcode %d: illegal operator", opcode);
            Error_Throw_0(context, context->str_buffer);
    }

    gc_release_var(context);
    return IMM_TRUE;
}

// �����������
static object op_exec_object_operation(context_t context, enum opcode_e opcode) {
    assert(context != NULL);
    gc_var2(context, tmp1, tmp2);

    switch (opcode) {
        case OP_INEXACT_TO_EXACT: {
            tmp1 = pair_car(context->args);
            double d;
            if (is_i64(tmp1)) {
                s_return(context, tmp1);
            } else if (modf(doublenum_getvalue(tmp1), &d) == 0.0) {
                s_return(context, i64_make_op(context, (int64_t) d));
            } else {
                Error_Throw_1(context, "inexact->exact: not integral:", tmp1);
            }
        }
        case OP_CHAR_TO_INT:
            tmp1 = i64_make_op(context, char_imm_getvalue(pair_car(context->args)));
            s_return(context, tmp1);
        case OP_INT_TO_CHAR:
            tmp1 = pair_car(context->args);
            s_return(context, char_imm_make((char) i64_getvalue(tmp1)));
        case OP_CHAR_UPPER:
            s_return(context, char_imm_make(toupper(char_imm_getvalue(pair_car(context->args)))));
        case OP_CHAR_LOWER:
            s_return(context, char_imm_make(tolower(char_imm_getvalue(pair_car(context->args)))));
        case OP_SYMBOL_TO_STRING:
            tmp1 = pair_car(context->args);
            tmp2 = symbol_to_string_op(context, tmp1);
            s_return(context, tmp2);
        case OP_STRING_TO_SYMBOL:
            tmp1 = pair_car(context->args);
            tmp2 = string_to_symbol_op(context, tmp1);
            s_return(context, tmp2);
        case OP_ATOM_TO_STRING: {
            int64_t base = 0;
            tmp1 = pair_car(context->args);
            if (pair_cdr(context->args) != IMM_UNIT) {
                /* cadr(context->args) => natural number */
                /* base 2, 8, 10, or 16, or error */
                base = i64_getvalue(pair_cadr(context->args));
                if (is_number(tmp1) && (base == 16 || base == 10 || base == 8 || base == 2)) {
                    /* base is OK */
                } else {
                    base = -1;
                }
            }
            if (base < 0) {
                Error_Throw_1(context, "atom->string: bad base:", pair_cadr(context->args));
            } else if (is_number(tmp1) || is_imm_char(tmp1) || is_string(tmp1) || is_symbol(tmp1)) {
                tmp2 = atom_to_string(context, tmp1, (int) base);
                s_return(context, tmp2);
            } else {
                Error_Throw_1(context, "atom->string: not an atom:", tmp1);
            }
        }
        case OP_STRING_TO_ATOM: {
            int64_t base = 0;
            tmp1 = pair_car(context->args);
            assert(is_string(tmp1));
            if (pair_cdr(context->args) != IMM_UNIT) {
                /* cadr(context->args) => natural number */
                /* base 2, 8, 10, or 16, or error */
                base = i64_getvalue(pair_cadr(context->args));
                if (base == 16 || base == 10 || base == 8 || base == 2) {
                    /* base is OK */
                } else {
                    base = -1;
                }
            }
            if (base < 0) {
                Error_Throw_1(context, "string->atom: bad base:", pair_cadr(context->args));
            } else if (string_index(tmp1, 0) == '#') {
                // sharp �������� base
                snprintf(context->str_buffer, INTERNAL_STR_BUFFER_SIZE, "%s", string_get_cstr(tmp1));
                // ���� '#'
                tmp1 = string_make_from_cstr_op(context, context->str_buffer + 1);
                s_return(context, sharp_const_make_op(context, tmp1));
            } else {
                if (base == 0 || base == 10) {
                    s_return(context, atom_make_op(context, tmp1));
                } else {
                    char *ep;
                    int64_t iv = strtoll(string_get_cstr(tmp1), &ep, (int) base);
                    if (*ep == 0) {
                        s_return(context, i64_make_op(context, iv));
                    } else {
                        s_return(context, IMM_FALSE);
                    }
                }
            }
        }
        case OP_MAKE_STRING: {
            char fill = ' ';
            int64_t len = i64_getvalue(pair_car(context->args));
            if (pair_cdr(context->args) != IMM_UNIT) {
                fill = char_imm_getvalue(pair_cadr(context->args));
            }
            tmp1 = string_make_empty(context, len, fill);
            s_return(context, tmp1);
        }
        case OP_STRING_LENGTH: {
            size_t len = string_len(pair_car(context->args));
            tmp1 = i64_make_op(context, (int64_t) len);
            s_return(context, tmp1);
        }
        case OP_STRING_REF: {
            tmp1 = pair_car(context->args);
            size_t index = (size_t) i64_getvalue(pair_cadr(context->args));
            if (index >= string_len(tmp1)) {
                Error_Throw_1(context, "string-ref: out of bounds:", pair_car(context->args));
            } else {
                s_return(context, char_imm_make(string_index(tmp1, index)));
            }
        }
        case OP_STRING_SET: {
            size_t index;
            char ch;

            tmp1 = pair_car(context->args);
            if (is_immutable(tmp1)) {
                Error_Throw_1(context, "string-set!: unable to alter immutable string:", tmp1);
            }

            index = (size_t) i64_getvalue(pair_cadr(context->args));
            if (index >= string_len(tmp1)) {
                Error_Throw_1(context, "string-set!: out of bounds:", pair_cadr(context->args));
            }

            ch = char_imm_getvalue(pair_caddr(context->args));
            string_index(tmp1, index) = ch;
            set_modified(tmp1);
            s_return(context, tmp1);
        }
        case OP_STRING_APPEND: {
            int64_t len = 0;
            char *pos;

            // �����ַ����ܳ�
            for (tmp1 = context->args; tmp1 != IMM_UNIT; tmp1 = pair_cdr(tmp1)) {
                len += string_len(pair_car(tmp1));
            }
            tmp2 = string_make_empty(context, len, ' ');

            // �����ַ���
            for (pos = string_get_cstr(tmp2), tmp1 = context->args;
                 tmp1 != IMM_UNIT;
                 pos += string_len(pair_car(tmp1)), tmp1 = pair_cdr(tmp1)) {
                memcpy(pos, string_get_cstr(pair_car(tmp1)), string_len(pair_car(tmp1)));
            }
            s_return(context, tmp2);
        }
        case OP_SUBSTRING: {
            int64_t index_start, index_end, length;
            tmp1 = pair_car(context->args);
            index_start = i64_getvalue(pair_cadr(context->args));
            if (index_start > (int64_t) string_len(tmp1)) {
                Error_Throw_1(context, "substring: start out of bounds:", pair_cadr(context->args));
            }
            if (pair_cddr(context->args) != IMM_UNIT) {
                index_end = i64_getvalue(pair_caddr(context->args));
                if (index_end > (int64_t) string_len(tmp1) || index_start > index_end) {
                    Error_Throw_1(context, "substring: end out of bounds:", pair_caddr(context->args));
                }
            } else {
                index_end = string_len(tmp1);
            }
            length = index_end - index_start;
            tmp2 = string_make_empty(context, length, ' ');
            memcpy(string_get_cstr(tmp2), string_get_cstr(tmp1) + index_start, (size_t) length);
            string_get_cstr(tmp2)[length] = 0;
            s_return(context, tmp2);
        }
        case OP_LIST_STAR:
            tmp1 = list_star(context, context->args);
            s_return(context, tmp1);
        case OP_LIST_APPEND:
            tmp1 = IMM_UNIT;
            tmp2 = context->args;
            if (tmp2 == IMM_UNIT) {
                s_return(context, tmp1);
            }

            // ʹ�� cdr() ����Ϊ���ʹ�� car(), (append '() 'a) �Ľ�������
            while (pair_cdr(tmp2) != IMM_UNIT) {
                tmp1 = reverse_append(context, tmp1, pair_car(tmp2));
                tmp2 = pair_cdr(tmp2);
                if (tmp1 == IMM_FALSE) {
                    Error_Throw_0(context, "non-list argument to append");
                }
            }
            s_return(context, reverse_in_place(context, pair_car(tmp2), tmp1));
        case OP_LIST_REVERSE:
            s_return(context, reverse(context, pair_car(context->args)));
        case OP_LIST_LENGTH: {
            int64_t len = list_length(pair_car(context->args));
            if (len < 0) {
                Error_Throw_1(context, "length: not a list:", pair_car(context->args));
            } else {
                tmp1 = i64_make_op(context, len);
                s_return(context, tmp1);
            }
        }
        case OP_ASSQ:
            tmp1 = pair_car(context->args);
            for (tmp2 = pair_cadr(context->args); is_pair(tmp2); tmp2 = pair_cdr(tmp2)) {
                if (!is_pair(pair_car(tmp2))) {
                    Error_Throw_0(context, "unable to handle non pair element");
                }
                if (tmp1 == pair_caar(tmp2))
                    break;
            }
            if (is_pair(tmp2)) {
                s_return(context, pair_car(tmp2));
            } else {
                s_return(context, IMM_FALSE);
            }
        case OP_VECTOR: {
            int64_t len = list_length(context->args);
            int64_t i;
            if (len < 0) {
                Error_Throw_1(context, "vector: not a proper list:", context->args);
            }
            tmp1 = vector_make_op(context, (size_t) len);
            for (i = 0, tmp2 = context->args; is_pair(tmp2); tmp2 = pair_cdr(tmp2), i++) {
                vector_set(tmp1, i, pair_car(tmp2));
            }
            s_return(context, tmp1);
        }
        case OP_MAKE_VECTOR: {
            tmp1 = IMM_UNIT;
            int64_t len;

            len = i64_getvalue(pair_car(context->args));

            if (pair_cdr(context->args) != IMM_UNIT) {
                tmp1 = pair_cadr(context->args);
            }
            // ������鱣֤ len ������
            tmp2 = vector_make_op(context, (size_t) len);
            if (tmp1 != IMM_UNIT) {
                // ��� vector
                for (size_t i = 0; i < vector_len(tmp2); i++) {
                    vector_ref(tmp2, i) = tmp1;
                }
            }
            s_return(context, tmp2);
        }
        case OP_VECTOR_LENGTH: {
            size_t len = vector_len(pair_car(context->args));
            tmp1 = i64_make_op(context, (int64_t) len);
            s_return(context, tmp1);
        }
        case OP_VECTOR_REF: {
            size_t index = (size_t) i64_getvalue(pair_cadr(context->args));
            if (index >= vector_len(pair_car(context->args))) {
                Error_Throw_1(context, "vector-ref: out of bounds:", pair_cadr(context->args));
            }
            s_return(context, vector_ref(pair_car(context->args), index));
        }
        case OP_VECTOR_SET: {
            if (is_immutable(pair_car(context->args))) {
                Error_Throw_1(context, "vector-set!: unable to alter immutable vector:", pair_car(context->args));
            }

            size_t index = (size_t) i64_getvalue(pair_cadr(context->args));
            if (index >= vector_len(pair_car(context->args))) {
                Error_Throw_1(context, "vector-set!: out of bounds:", pair_cadr(context->args));
            }
            vector_set(pair_car(context->args), index, pair_caddr(context->args));
            s_return(context, pair_car(context->args));
        }
        case OP_NOT:
            s_return(context, setbool(pair_car(context->args) == IMM_FALSE));
        default:
            snprintf(context->str_buffer, INTERNAL_STR_BUFFER_SIZE, "opcode %d: illegal operator", opcode);
            Error_Throw_0(context, context->str_buffer);
    }

    gc_release_var(context);
    return IMM_TRUE;
}

// I/O ����
static object op_exec_io(context_t context, enum opcode_e opcode) {
    assert(context != NULL);
    gc_var2(context, tmp1, tmp2);

    switch (opcode) {
        case OP_CURRENT_INPUT_PORT:
            s_return(context, context->in_port);
        case OP_CURRENT_OUTPUT_PORT:
            s_return(context, context->out_port);
        case OP_OPEN_INPUT_FILE:
        case OP_OPEN_OUTPUT_FILE:
        case OP_OPEN_INPUT_OUTPUT_FILE: {
            enum port_kind k;
            switch (opcode) {
                case OP_OPEN_INPUT_FILE:
                    k = PORT_INPUT;
                    break;
                case OP_OPEN_OUTPUT_FILE:
                    k = PORT_OUTPUT;
                    break;
                case OP_OPEN_INPUT_OUTPUT_FILE:
                    k = PORT_INPUT | PORT_OUTPUT;
                    break;
                default:
                    snprintf(context->str_buffer, INTERNAL_STR_BUFFER_SIZE, "opcode %d: illegal operator", opcode);
                    Error_Throw_0(context, context->str_buffer);
            }
            tmp1 = stdio_port_from_filename_op(context, pair_car(context->args), k);
            if (tmp1 == IMM_UNIT) {
                s_return(context, IMM_FALSE);
            } else {
                s_return(context, tmp1);
            }
        }
        case OP_OPEN_INPUT_STRING:
            tmp1 = string_port_input_from_string_op(context, pair_car(context->args));
            s_return(context, tmp1);
        case OP_OPEN_OUTPUT_STRING:
            if (pair_car(context->args) == IMM_UNIT) {
                tmp1 = string_port_output_use_buffer_op(context);
                s_return(context, tmp1);
            } else {
                tmp1 = string_port_in_out_put_from_string_use_buffer_op(context, pair_car(context->args));
                s_return(context, tmp1);
            }
        case OP_OPEN_INPUT_OUTPUT_STRING:
            if (pair_car(context->args) == IMM_UNIT) {
                tmp1 = string_port_in_out_put_use_buffer_op(context);
                s_return(context, tmp1);
            } else {
                tmp1 = string_port_in_out_put_from_string_use_buffer_op(context, pair_car(context->args));
                s_return(context, tmp1);
            }
        case OP_GET_OUTPUT_STRING:
            tmp1 = pair_car(context->args);
            if (is_srfi6_port(tmp1)) {
                tmp1 = tmp1->value.string_port.string_buffer_data;
                tmp2 = string_buffer_to_string_op(context, tmp1);
                s_return(context, tmp2);
            } else if (is_string_port(tmp1)) {
                tmp1 = tmp1->value.string_port.string_buffer_data;
                tmp2 = string_make_empty(context, string_len(tmp1), ' ');
                memcpy(string_get_cstr(tmp2), string_get_cstr(tmp1), string_len(tmp1));
                s_return(context, tmp2);
            } else {
                s_return(context, IMM_FALSE);
            }
        case OP_CLOSE_INPUT_PORT:
            tmp1 = pair_car(context->args);
            if (is_stdio_port(tmp1) && is_stdio_port_input(tmp1)) {
                stdio_finalizer(context, tmp1);
                s_return(context, IMM_TRUE);
            } else if (is_string_port_input(tmp1)) {
                s_return(context, IMM_TRUE);
            } else {
                s_return(context, IMM_FALSE);
            }
        case OP_CLOSE_OUTPUT_PORT:
            tmp1 = pair_car(context->args);
            if (is_stdio_port(tmp1) && is_stdio_port_output(tmp1)) {
                stdio_finalizer(context, tmp1);
                s_return(context, IMM_TRUE);
            } else if (is_string_port_output(tmp1)) {
                s_return(context, IMM_TRUE);
            } else {
                s_return(context, IMM_FALSE);
            }
        case OP_READ:
            if (!is_pair(context->args)) {
                s_goto(context, OP_READ_INTERNAL);
            }
            tmp1 = pair_car(context->args);
            if (!is_port_input(tmp1)) {
                Error_Throw_1(context, "read: not an input port:", tmp1);
            }
            if (tmp1 == context->in_port) {
                s_goto(context, OP_READ_INTERNAL);
            }
            tmp2 = context->in_port;
            tmp2 = pair_make_op(context, tmp2, IMM_UNIT);
            s_save(context, OP_SET_INPUT_PORT, tmp2, IMM_UNIT);
            context->in_port = tmp1;
            s_goto(context, OP_READ_INTERNAL);
        case OP_READ_CHAR:
            if (is_pair(context->args)) {
                tmp1 = pair_car(context->args);
            } else {
                tmp1 = context->in_port;
            }
            tmp2 = port_get_char(tmp1);
            s_return(context, IMM_EOF);
        case OP_WRITE:
        case OP_WRITE_CHAR:
            if (is_pair(pair_cdr(context->args))) {
                if (pair_cadr(context->args) != context->out_port) {
                    tmp1 = pair_make_op(context, context->out_port, IMM_UNIT);
                    s_save(context, OP_SET_OUTPUT_PORT, tmp1, IMM_UNIT);
                    context->out_port = pair_cadr(context->args);
                }
            }
            context->args = pair_car(context->args);
            if (opcode == OP_WRITE) {
                context->print_flag = 1;
            }
            s_goto(context, OP_PRINT_OBJECT);
        case OP_PEEK_CHAR:
            if (is_pair(context->args)) {
                tmp1 = pair_car(context->args);
            } else {
                tmp1 = context->in_port;
            }
            tmp2 = port_get_char(tmp1);
            port_unget_char(tmp1, tmp2);
            s_return(context, IMM_EOF);
        case OP_CHAR_READY_P: {
            tmp1 = context->in_port;
            int res;
            if (is_pair(context->args)) {
                tmp1 = pair_car(context->args);
            }
            res = is_string_port(tmp1);
            s_return(context, setbool(res));
        }
        case OP_SET_INPUT_PORT:
            context->in_port = pair_car(context->args);
            s_return(context, context->in_port);
        case OP_SET_OUTPUT_PORT:
            context->out_port = pair_car(context->args);
            s_return(context, context->out_port);
        default:
            snprintf(context->str_buffer, INTERNAL_STR_BUFFER_SIZE, "opcode %d: illegal operator", opcode);
            Error_Throw_0(context, context->str_buffer);
    }

    gc_release_var(context);
    return IMM_TRUE;
}

static object op_exec_lexical(context_t context, enum opcode_e opcode) {
    assert(context != NULL);
    gc_var2(context, tmp1, tmp2);

    if (context->bracket_level != 0) {
        context->ret = ERROR_PARENTHESES_NOT_MATCH;
        int64_t deep = context->bracket_level;
        context->bracket_level = 0;
        tmp1 = i64_make_op(context, deep);
        Error_Throw_1(context, "unmatched parentheses:", tmp1);
    }

    // �ʷ�����
    switch (opcode) {
        case OP_READ_SEXP:
            switch (context->token) {
                case TOKEN_EOF:
                    // �ļ�����
                    s_return(context, IMM_EOF);
                case TOKEN_VECTOR:
                    // "#(...)", ��ʱ token() �Ѿ�ȡ�� "#("
                    s_save(context, OP_READ_VECTOR, IMM_UNIT, IMM_UNIT);
                    // ���Ŷ��������Ž����ŵ�����
                    // û�� break, û�� return
                    // ���� bug
                case TOKEN_LEFT_PAREN:
                    context->token = token(context);
                    if (context->token == TOKEN_RIGHT_PAREN) {
                        // ()
                        s_return(context, IMM_UNIT);
                    } else if (context->token == TOKEN_DOT) {
                        // (.)
                        Error_Throw_0(context, "syntax error: illegal dot expression");
                    } else {
                        // �������, ������� +1
                        int64_t deep = i64_getvalue(stack_peek_op(context->bracket_level_stack));
                        deep += 1;
                        tmp1 = i64_make_op(context, deep);
                        stack_set_top_op(context->bracket_level_stack, tmp1);

                        s_save(context, OP_READ_LIST, IMM_UNIT, IMM_UNIT);
                        // �ݹ���� sexp
                        s_goto(context, OP_READ_SEXP);
                    }
                case TOKEN_QUOTE:
                    // ' ����
                    s_save(context, OP_READ_QUOTE, IMM_UNIT, IMM_UNIT);
                    context->token = token(context);
                    // ���� sexp ���� OP_READ_QUOTE ��������
                    s_goto(context, OP_READ_SEXP);
                case TOKEN_BACK_QUOTE:
                    // ` ������
                    // `(1 2 3)         =>  '(1 2 3)
                    // `(1 ,(+ 2 3) 3)  =>  '(1 5 3)
                    // `#(1 2)          =>  '(1 2)
                    // `#(1 ,(+ 1 2))   =>  '#(1 3)
                    context->token = token(context);

                    if (context->token == TOKEN_VECTOR) {
                        s_save(context, OP_READ_QUASIQUOTE_VECTOR, IMM_UNIT, IMM_UNIT);
                        // ��Ϊ token() ���̵� "#()" �� "#("
                        context->token = TOKEN_LEFT_PAREN;
                        s_goto(context, OP_READ_SEXP);
                    } else {
                        s_save(context, OP_READ_QUASIQUOTE, IMM_UNIT, IMM_UNIT);
                    }

                    // �ݹ���� sexp
                    s_goto(context, OP_READ_SEXP);
                case TOKEN_COMMA:
                    // , ���� ���������� ","
                    // �ڷ�������Ҫ����ֵ������
                    s_save(context, OP_READ_UNQUOTE, IMM_UNIT, IMM_UNIT);
                    context->token = token(context);

                    // �ݹ������Ҫ����ֵ�� sexp
                    s_goto(context, OP_READ_SEXP);
                case TOKEN_AT_MART:
                    // @: ���������� ",@", �� list ƴ�ӽ�����
                    // `(1 ,@(list 1 2) 4)  =>  '(1 1 2 4)
                    // `#(1 ,@(list 1 2) 4) =>  '#(1 1 2 4)
                    s_save(context, OP_READ_UNQUOTE_SPLICING, IMM_UNIT, IMM_UNIT);
                    context->token = token(context);
                    // �ݹ����Ҫ��չ���� list
                    s_goto(context, OP_READ_SEXP);
                case TOKEN_SHARP_CONST:
                    tmp1 = read_upto(context, DELIMITERS);
                    tmp2 = sharp_const_make_op(context, tmp1);
                    if (tmp2 == IMM_UNIT) {
                        Error_Throw_0(context, "unknown sharp expression");
                    } else {
                        s_return(context, tmp2);
                    }
                case TOKEN_ATOM:
                    tmp1 = read_upto(context, DELIMITERS);
                    tmp2 = atom_make_op(context, tmp1);
                    if (tmp2 == IMM_UNIT) {
                        Error_Throw_0(context, "unknown atom expression");
                    } else {
                        s_return(context, tmp2);
                    }
                case TOKEN_DOUBLE_QUOTE:
                    // "..."  �ַ���
                    tmp1 = read_string_expr(context);
                    if (tmp1 == IMM_UNIT) {
                        Error_Throw_0(context, "Error reading string");
                    }
                    set_immutable(tmp1);
                    s_return(context, tmp1);
                case TOKEN_ILLEGAL:
                default:
                    Error_Throw_0(context, "syntax error: illegal token");
            }
        case OP_READ_LIST:
            // OP_READ_SEXP �� OP_READ_LIST �ݹ����
            // value: ���� OP_READ_LIST �ķ���ֵ
            // args: (list ... obj2 obj1 '()) ����
            context->args = pair_make_op(context, context->value, context->args);
            context->token = token(context);

            if (context->token == TOKEN_EOF) {
                // �ļ�����
                s_return(context, IMM_EOF);
            } else if (context->token == TOKEN_RIGHT_PAREN) {
                // ����������, �ݹ�������
                object ch = port_get_char(context->in_port);
                // ��ǰ����һ���ַ�, ��Ϊ�����Ž��������ǻ���
                if (ch != char_imm_make('\n')) {
                    port_unget_char(context->in_port, ch);
                } else if (is_stdio_port(context->in_port)) {
                    stdio_port_get_line(context->in_port)++;
                }
                // ���Ų㼶 -1
                int64_t deep = i64_getvalue(stack_peek_op(context->bracket_level_stack));
                tmp1 = i64_make_op(context, deep - 1);
                stack_set_top_op(context->bracket_level_stack, tmp1);
                // ��󷵻ض���Ľ��
                s_return(context, reverse_in_place(context, IMM_UNIT, context->args));
            } else if (context->token == TOKEN_DOT) {
                // ���� .
                s_save(context, OP_READ_DOT, context->args, IMM_UNIT);
                context->token = token(context);
                // ����ʣ��Ĳ������ list
                s_goto(context, OP_READ_SEXP);
            } else {
                // ��������ݹ���� list ����
                s_save(context, OP_READ_LIST, context->args, IMM_UNIT);
                s_goto(context, OP_READ_SEXP);
            }
        case OP_READ_DOT:
            // (a b c . d)
            // d ���������������, �������
            if (token(context) != TOKEN_RIGHT_PAREN) {
                Error_Throw_0(context, "syntax error: illegal dot expression");
            } else {
                // ���Ų㼶 -1
                int64_t deep = i64_getvalue(stack_peek_op(context->bracket_level_stack));
                tmp1 = i64_make_op(context, deep - 1);
                stack_set_top_op(context->bracket_level_stack, tmp1);
                // value ��ʣ�����е� object ��ɵ� list
                // args ����� β���� value
                s_return(context, reverse_in_place(context, context->value, context->args));
            }
        case OP_READ_QUOTE:
            tmp1 = pair_make_op(context, context->value, IMM_UNIT);
            tmp2 = pair_make_op(context, context->QUOTE, tmp1);
            s_return(context, tmp2);
        case OP_READ_QUASIQUOTE:
            tmp1 = pair_make_op(context, context->value, IMM_UNIT);
            tmp2 = pair_make_op(context, context->QUASIQUOTE, tmp1);
            s_return(context, tmp2);
        case OP_READ_QUASIQUOTE_VECTOR:
            // ,#(...)
            tmp1 = pair_make_op(context, context->value, IMM_UNIT);
            tmp2 = pair_make_op(context, context->QUASIQUOTE, tmp1);
            tmp2 = pair_make_op(context, tmp2, IMM_UNIT);
            // (apply (vector `,#(...)))
            tmp1 = symbol_make_from_cstr_op(context, "vector");
            tmp2 = pair_make_op(context, tmp1, tmp2);
            tmp1 = symbol_make_from_cstr_op(context, "apply");
            tmp2 = pair_make_op(context, tmp1, tmp2);
            s_return(context, tmp2);
        case OP_READ_UNQUOTE_SPLICING:
            tmp1 = pair_make_op(context, context->value, IMM_UNIT);
            tmp2 = pair_make_op(context, context->UNQUOTE_SPLICING, tmp1);
            s_return(context, tmp2);
        case OP_READ_UNQUOTE:
            tmp1 = pair_make_op(context, context->value, IMM_UNIT);
            tmp2 = pair_make_op(context, context->UNQUOTE, tmp1);
            s_return(context, tmp2);
        case OP_READ_VECTOR:
            context->args = context->value;
            s_goto(context, OP_VECTOR);

        case OP_PRINT_OBJECT:
            // print ����
            // ext type ����ͨ�����ϴ�������ʱ���, ��Ҫ���ȴ���
            if (is_ext_type_environment(context->args)) {
                port_put_cstr(context, context->out_port, "#<ENVIRONMENT>");
                s_return(context, IMM_TRUE);
            } else if (is_ext_type_closure(context->args)) {
                port_put_cstr(context, context->out_port, "#<CLOSURE>");
                s_return(context, IMM_TRUE);
            } else if (is_ext_type_macro(context->args)) {
                port_put_cstr(context, context->out_port, "#<MACRO>");
                s_return(context, IMM_TRUE);
            } else if (is_ext_type_continuation(context->args)) {
                port_put_cstr(context, context->out_port, "#<CONTINUATION>");
                s_return(context, IMM_TRUE);

                // �����ǳ�������
            } else if (is_vector(context->args)) {
                port_put_cstr(context, context->out_port, "#(");
                tmp1 = i64_make_op(context, 0);
                context->args = pair_make_op(context, context->args, tmp1);
                s_goto(context, OP_PRINT_VECTOR);
            } else if (!is_pair(context->args)) {
                // ��� atom, �ڴ˷�֧֮ǰӦ���ų����г��� pair ���������
                print_atom(context, context->args, context->print_flag);
                s_return(context, IMM_TRUE);

                // ���涼�� pair ����
            } else if (symbol_equals(context, pair_car(context->args), context->QUOTE) &&
                       ok_abbrev(pair_cdr(context->args))) {
                port_put_cstr(context, context->out_port, "'");
                context->args = pair_cadr(context->args);
                s_goto(context, OP_PRINT_OBJECT);
            } else if (symbol_equals(context, pair_car(context->args), context->QUASIQUOTE) &&
                       ok_abbrev(pair_cdr(context->args))) {
                port_put_cstr(context, context->out_port, "`");
                context->args = pair_cadr(context->args);
                s_goto(context, OP_PRINT_OBJECT);
            } else if (symbol_equals(context, pair_car(context->args), context->UNQUOTE) &&
                       ok_abbrev(pair_cdr(context->args))) {
                port_put_cstr(context, context->out_port, ",");
                context->args = pair_cadr(context->args);
                s_goto(context, OP_PRINT_OBJECT);
            } else if (symbol_equals(context, pair_car(context->args), context->UNQUOTE_SPLICING) &&
                       ok_abbrev(pair_cdr(context->args))) {
                port_put_cstr(context, context->out_port, ",@");
                context->args = pair_cadr(context->args);
                s_goto(context, OP_PRINT_OBJECT);
            } else {
                // list
                port_put_cstr(context, context->out_port, "(");
                s_save(context, OP_PRINT_LIST, pair_cdr(context->args), IMM_UNIT);
                // ��ȡ�� (pair_car args) ���, ֮����� OP_PRINT_LIST �ݹ����
                context->args = pair_car(context->args);
                s_goto(context, OP_PRINT_OBJECT);
            }
        case OP_PRINT_LIST:
            if (is_pair(context->args)) {
                s_save(context, OP_PRINT_LIST, pair_cdr(context->args), IMM_UNIT);
                port_put_cstr(context, context->out_port, " ");
                context->args = pair_car(context->args);
                s_goto(context, OP_PRINT_OBJECT);
            } else {
                if (context->args != IMM_UNIT) {
                    port_put_cstr(context, context->out_port, " . ");
                    print_atom(context, context->args, context->print_flag);
                }
                port_put_cstr(context, context->out_port, ")");
                s_return(context, IMM_TRUE);
            }
        case OP_PRINT_VECTOR: {
            size_t i = (size_t) i64_getvalue(pair_cdr(context->args));
            tmp1 = pair_car(context->args);
            size_t len = vector_len(tmp1);
            if (i >= len) {
                port_put_cstr(context, context->out_port, ")");
                s_return(context, IMM_TRUE);
            } else {
                pair_cdr(context->args) = i64_make_op(context, i + 1);
                s_save(context, OP_PRINT_VECTOR, context->args, IMM_UNIT);
                context->args = vector_ref(tmp1, i);
                if (i != 0) {
                    // "#(" ���治�ÿո�
                    port_put_cstr(context, context->out_port, " ");
                }
                s_goto(context, OP_PRINT_OBJECT);
            }
        }
        default:
            snprintf(context->str_buffer, INTERNAL_STR_BUFFER_SIZE, "opcode %d: illegal operator", opcode);
            Error_Throw_0(context, context->str_buffer);
    }

    gc_release_var(context);
    return IMM_TRUE;
}

static object op_exec_predicate(context_t context, enum opcode_e opcode) {
    assert(context != NULL);
    gc_var2(context, tmp1, tmp2);

    switch (opcode) {
        case OP_DEFINE_P:
            tmp1 = context->current_env;
            if (pair_cdr(context->args) != IMM_UNIT) {
                tmp1 = pair_cadr(context->args);
            }
            assert(is_ext_type_environment(tmp1));
            tmp2 = find_slot_in_spec_env(context, tmp1, pair_car(context->args), 1);
            s_return(context, setbool(tmp2 == IMM_UNIT));
        case OP_CLOSURE_P: {
            // macro Ҳ�Ǳհ�
            tmp1 = pair_car(context->args);
            s_return(context, setbool(is_ext_type_closure(tmp1) || is_ext_type_macro(tmp1)));
        }
        case OP_MACRO_P:
            s_return(context, setbool(is_ext_type_macro(pair_car(context->args))));
        case OP_BOOLEAN_P: {
            int f = (pair_car(context->args) == IMM_TRUE) || (pair_car(context->args) == IMM_FALSE);
            s_return(context, setbool(f));
        }
        case OP_EOF_OBJ_P:
            s_return(context, setbool(pair_car(context->args) == IMM_EOF));
        case OP_NULL_P:
            s_return(context, setbool(pair_car(context->args) == IMM_UNIT));
        case OP_SYMBOL_P:
            s_return(context, setbool(is_symbol(pair_car(context->args))));
        case OP_NUMBER_P:
            s_return(context, setbool(is_number(pair_car(context->args))));
        case OP_STRING_P:
            s_return(context, setbool(is_string(pair_car(context->args))));
        case OP_INTEGER_P:
            tmp1 = pair_car(context->args);
            if (is_i64(tmp1)) {
                s_return(context, IMM_TRUE);
            } else if (is_doublenum(tmp1)) {
                int64_t i = (int64_t) (doublenum_getvalue(tmp1));
                if ((double) i == doublenum_getvalue(tmp1)) {
                    s_return(context, IMM_TRUE);
                } else {
                    s_return(context, IMM_FALSE);
                }
            } else {
                s_return(context, IMM_FALSE);
            }
        case OP_REAL_P:
            s_return(context, setbool(is_number(pair_car(context->args))));
        case OP_CHAR_P:
            s_return(context, setbool(is_imm_char(pair_car(context->args))));
        case OP_CHAR_ALPHABETIC_P: {
            object ch = pair_car(context->args);
            if (is_imm_char(ch)) {
                char c = char_imm_getvalue(ch);
                int f = isascii(c) && isalpha(c);
                s_return(context, setbool(f));
            } else {
                s_return(context, IMM_FALSE);
            }
        }
        case OP_CHAR_NUMERIC_P: {
            object ch = pair_car(context->args);
            if (is_imm_char(ch)) {
                char c = char_imm_getvalue(ch);
                int f = isascii(c) && isdigit(c);
                s_return(context, setbool(f));
            } else {
                s_return(context, IMM_FALSE);
            }
        }
        case OP_CHAR_WHITESPACE_P: {
            object ch = pair_car(context->args);
            if (is_imm_char(ch)) {
                char c = char_imm_getvalue(ch);
                int f = isascii(c) && isspace(c);
                s_return(context, setbool(f));
            } else {
                s_return(context, IMM_FALSE);
            }
        }
        case OP_CHAR_UPPER_CASE_P: {
            object ch = pair_car(context->args);
            if (is_imm_char(ch)) {
                char c = char_imm_getvalue(ch);
                int f = isascii(c) && isupper(c);
                s_return(context, setbool(f));
            } else {
                s_return(context, IMM_FALSE);
            }
        }
        case OP_CHAR_LOWER_CASE_P: {
            object ch = pair_car(context->args);
            if (is_imm_char(ch)) {
                char c = char_imm_getvalue(ch);
                int f = isascii(c) && islower(c);
                s_return(context, setbool(f));
            } else {
                s_return(context, IMM_FALSE);
            }
        }
        case OP_PORT_P: {
            object port = pair_car(context->args);
            int f = is_port(port);
            s_return(context, setbool(f));
        }
        case OP_INPUT_PORT_P: {
            object port = pair_car(context->args);
            int f = is_port_input(port);
            s_return(context, setbool(f));
        }
        case OP_OUT_PUT_P: {
            object port = pair_car(context->args);
            int f = is_port_output(port);
            s_return(context, setbool(f));
        }
        case OP_PROCEDURE_P: {
            // TODO ffi ʵ��
            // continuation Ҳ�� procedure
            object p = pair_car(context->args);
            int f = is_proc(p) || is_ext_type_closure(p) || is_ext_type_continuation(p);
            s_return(context, setbool(f));
        }
        case OP_PROMISE_P: {
            object p = pair_car(context->args);
            int f = is_promise(p);
            s_return(context, setbool(f));
        }
        case OP_PAIR_P:
            tmp1 = pair_car(context->args);
            s_return(context, setbool(is_pair(tmp1) && !is_ext_type(tmp1)));
        case OP_LIST_P:
            s_return(context, setbool(list_length(pair_car(context->args)) >= 0));
        case OP_ENVIRONMENT_P:
            s_return(context, setbool(is_ext_type_environment(pair_car(context->args))));
        case OP_VECTOR_P:
            s_return(context, setbool(is_vector(pair_car(context->args))));
        case OP_EQ:
            s_return(context, setbool(pair_car(context->args) == pair_cadr(context->args)));
        case OP_EQV:
            s_return(context, setbool(eqv(context, pair_car(context->args), pair_cadr(context->args))));
        default:
            snprintf(context->str_buffer, INTERNAL_STR_BUFFER_SIZE, "opcode %d: illegal operator", opcode);
            Error_Throw_0(context, context->str_buffer);
    }

    gc_release_var(context);
    return IMM_TRUE;
}

static object op_exec_builtin_function(context_t context, enum opcode_e opcode) {
    assert(context != NULL);
    gc_var2(context, tmp1, tmp2);

    switch (opcode) {
        case OP_QUIT:
            if (is_pair(context->args)) {
                context->ret = (int) i64_getvalue(pair_car(context->args));
            } else {
                // �޲�����Ĭ�������˳�
                context->ret = 0;
            }
            gc_release_var(context);
            return IMM_UNIT;
        case OP_GC:
            gc_collect(context);
            s_return(context, IMM_TRUE);
        case OP_DEBUG: {
            int64_t flag = i64_getvalue(pair_car(context->args));
            if (flag) {
                context->debug = 1;
                s_return(context, IMM_TRUE);
            } else {
                context->debug = 0;
                s_return(context, IMM_FALSE);
            }
        }
        case OP_ERROR:
            // args: (error "message" . object)
            assert(is_pair(context->args));

            context->ret = -1;
            // ��� message
            port_put_cstr(context, context->out_port, "Error: ");
            if (!is_string(pair_car(context->args))) {
                port_put_cstr(context, context->out_port, "--");

            }
            port_put_string(context, context->out_port, pair_car(context->args));

            // ���ʣ��Ĳ���
            context->args = pair_cdr(context->args);
            s_goto(context, OP_ERROR_PRINT_OBJECT);
        case OP_ERROR_PRINT_OBJECT:
            // args: (. object)

            port_put_cstr(context, context->out_port, " ");

            if (context->args != IMM_UNIT) {
                // ����������в���
                context->print_flag = 1;
                s_save(context, OP_ERROR_PRINT_OBJECT, pair_cdr(context->args), IMM_UNIT);
                context->args = pair_car(context->args);
                s_goto(context, OP_PRINT_OBJECT);
            } else {
                // ����Ϊ����������Ĳ���
                port_put_cstr(context, context->out_port, "\n");
                if (context->repl_mode) {
                    // repl ģʽ��ص� OP_TOP_LEVEL_SETUP
                    s_goto(context, OP_TOP_LEVEL_SETUP);
                } else {
                    // ����ģʽֱ�ӽ�������������
                    // context->ret ���׳��쳣ǰӦ��������
                    gc_release_var(context);
                    return IMM_UNIT;
                }
            }
        case OP_PROC_DISPlAY:
            if (is_pair(pair_cdr(context->args))) {
                if (pair_cadr(context->args) != context->out_port) {
                    tmp1 = pair_make_op(context, context->out_port, IMM_UNIT);
                    s_save(context, OP_SET_OUTPUT_PORT, tmp1, IMM_UNIT);
                    context->out_port = pair_cadr(context->args);
                }
            }
            context->args = pair_car(context->args);
            context->print_flag = 0;
            s_goto(context, OP_PRINT_OBJECT);
        case OP_PROC_NEWLINE:
            if (is_pair(context->args)) {
                if (pair_car(context->args) != context->out_port) {
                    tmp1 = pair_make_op(context, context->out_port, IMM_UNIT);
                    s_save(context, OP_SET_OUTPUT_PORT, tmp1, IMM_UNIT);
                    context->out_port = pair_car(context->args);
                }
            }
            port_put_cstr(context, context->out_port, "\n");
            s_return(context, IMM_TRUE);
        case OP_PROC_EVAL:
            if (pair_cdr(context->args) != IMM_UNIT) {
                context->current_env = pair_cadr(context->args);
            }
            context->code = pair_car(context->args);
            s_goto(context, OP_EVAL);
        case OP_PROC_APPLY:
            // list *
            if (pair_cdr(context->args) != IMM_UNIT) {
                tmp1 = pair_cadr(context->args);
                if (!is_object(tmp1) && tmp1 != IMM_UNIT) {
                    Error_Throw_0(context, "apply: argument 2 must be list");
                }
            }
            context->code = pair_car(context->args);
            context->args = list_star(context, pair_cdr(context->args));
            s_goto(context, OP_APPLY);
        case OP_INTERACTION_ENV:
            s_return(context, context->global_environment);
        case OP_CURRENT_ENV:
            s_return(context, context->current_env);
#ifdef DEBUG_ONLY
        case OP_TRACING_OPCODE: {
            int64_t flag = i64_getvalue(pair_car(context->args));
            if (flag) {
                context->tracing_opcode = 1;
                s_return(context, IMM_TRUE);
            } else {
                context->tracing_opcode = 0;
                s_return(context, IMM_FALSE);
            }
        }
#endif
        default:
            snprintf(context->str_buffer, INTERNAL_STR_BUFFER_SIZE, "opcode %d: illegal operator", opcode);
            Error_Throw_0(context, context->str_buffer);
    }

    gc_release_var(context);
    return IMM_TRUE;
}

/******************************************************************************
                                  �ļ�����
******************************************************************************/
EXPORT_API GC int interpreter_load_cstr(context_t context, const char *cstr) {
    assert(context != NULL);
    assert(cstr != NULL);

    gc_var2(context, port, str);
    scheme_stack_reset(context);
    context->current_env = context->global_environment;
    stack_clean(context->load_stack);
    stack_clean(context->bracket_level_stack);

    // �����ļ�
    str = string_make_from_cstr_op(context, cstr);
    port = string_port_input_from_string_op(context, str);
    context->load_stack = stack_push_auto_increase_op(context, context->load_stack, port, 50);
    context->bracket_level_stack = stack_push_auto_increase_op(context, context->bracket_level_stack, i64_imm_make(0),
                                                               50);
    context->bracket_level = 0;
    context->load_port = port;
    context->ret = 0;
    context->repl_mode = 0;

    context->in_port = port;

    // ��ʼ����
    eval_apply_loop(context, OP_TOP_LEVEL_SETUP);

    gc_release_var(context);
    if (context->ret == 0 && context->bracket_level != 0) {
        context->ret = ERROR_PARENTHESES_NOT_MATCH;
    }
    return context->ret;
}

EXPORT_API GC int interpreter_load_file(context_t context, FILE *file) {
    if (file == stdin) {
        return interpreter_load_file_with_name(context, file, "<stdin>");

    } else {
        return interpreter_load_file_with_name(context, file, "<unknown>");
    }
}

EXPORT_API GC int interpreter_load_file_with_name(context_t context, FILE *file, const char *file_name) {
    assert(context != NULL);
    gc_var2(context, port, str);

    if (file == NULL) {
        gc_release_var(context);
        return context->ret;
    }
    if (file_name == NULL) {
        file_name = "<unknown>";
    }
    str = string_make_from_cstr_op(context, file_name);

    scheme_stack_reset(context);
    context->current_env = context->global_environment;
    stack_clean(context->load_stack);
    stack_clean(context->bracket_level_stack);

    // �����ļ�
    port = stdio_port_from_file_op(context, file, PORT_INPUT);
    stdio_port_get_filename(port) = str;
    context->load_stack = stack_push_auto_increase_op(context, context->load_stack, port, 50);
    context->bracket_level_stack = stack_push_auto_increase_op(context, context->bracket_level_stack, i64_imm_make(0),
                                                               50);
    context->bracket_level = 0;
    context->load_port = port;
    context->in_port = port;
    context->ret = 0;
    if (file == stdin) {
        context->repl_mode = 1;
    } else {
        context->repl_mode = 0;
    }

    // ��ʼ����
    eval_apply_loop(context, OP_TOP_LEVEL_SETUP);

    gc_release_var(context);

    if (context->ret == 0 && context->bracket_level != 0) {
        context->ret = ERROR_PARENTHESES_NOT_MATCH;
    }
    return context->ret;
}
