#include "paper-scheme/interpreter.h"

/******************************************************************************
                            关键字和内部过程初始化
******************************************************************************/
/**
 * 检查给定的 symbol 是否是 syntax
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
 * 从 symbol 获取 syntax
 * @param context
 * @param symbol
 * @return syntax, 不存在则返回 IMM_UNIT
 */
EXPORT_API object syntax_get_by_symbol(context_t context, object symbol) {
    assert(context != NULL);
    assert(is_symbol(symbol));

    return hashmap_get_op(context, context->syntax_table, symbol);
}

static GC void assign_syntax(context_t context, char *name, enum opcode_e opcode) {
    assert(context != NULL);
    assert(name != NULL);
    assert(strlen(name) != 0);
    gc_var2(context, sym, syntax);

    sym = symbol_make_from_cstr_op(context, name);
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
                              词法分析器相关
******************************************************************************/

static int64_t binary_decode(const char *s) {
    int64_t x = 0;

    while (*s != 0 && (*s == '1' || *s == '0')) {
        x <<= 1u;
        x += *s - '0';
        s++;
    }

    return x;
}
/**
 * 根据输入字符串构造 sharp 常量
 * @param context
 * @param str
 * @return sharp 常量或 IMM_UNIT (如果输入非法)
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
            // 是的, 这里有类型转换的安全问题, 但是实现有符号数太麻烦了.
            // 与其实现有符号数不如实现大数运算
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
        // char 立即数
        obj = IMM_UNIT;
        if (!strcmp(string_get_cstr(str) + 1, "space")) {
            obj = char_imm_make(' ');
        } else if (!strcmp(string_get_cstr(str) + 1, "newline")) {
            obj = char_imm_make('\n');
        } else if (!strcmp(string_get_cstr(str) + 1, "return")) {
            obj = char_imm_make('\r');
        } else if (!strcmp(string_get_cstr(str) + 1, "tab")) {
            obj = char_imm_make('\t');
        } else if (string_index(str, 1) == 'x' && string_index(str, 2) != '\0') {
            int hex = 0;
            if (sscanf(string_get_cstr(str) + 2, "%x", &hex) == 1) {
                if (hex >= 0 && hex <= UCHAR_MAX) {
                    obj = char_imm_make((char) hex);
                }
            }
        } else if (string_index(str, 2) != '\0') {
            obj = char_imm_make(string_index(str, 1));
        } else {
            obj = IMM_UNIT;
        }
    } else {
        obj = IMM_UNIT;
    }

    // 不符合全部情况, 返回 UNIT
    gc_release_param(context);
    return obj;
}

/**
 * <p>如果 str 是 syntax, 则返回 syntax</p>
 * <p>否则返回 原始 symbol</p>
 * @param context
 * @param str string
 * @return syntax/symbol
 */
static object str_to_syntax_or_symbol(context_t context, object str) {
    assert(context != NULL);
    assert(is_string(str));
    gc_param1(context, str);
    gc_var1(context, ret);

    ret = string_to_symbol_op(context, str);
    if (symbol_is_syntax(context, ret)) {
        ret = syntax_get_by_symbol(context, ret);
        assert(is_syntax(ret));
    }
    gc_release_param(context);
    return ret;
}

/**
 * 从字符串构造 symbol, syntax 或者 数字
 * @param context
 * @param str
 * @return
 */
EXPORT_API GC object atom_make_op(context_t context, object str) {
    assert(context != NULL);
    assert(is_string(str));
    assert(string_len(str) != 0);
    gc_param1(context, str);
    gc_var2(context, sym, syntax);

    char *cstr = string_get_cstr(str);
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
            return str_to_syntax_or_symbol(context, str);
        }
    } else if (ch == '.') {
        has_dec_point = 1;
        ch = *cstr_cursor++;
        if (!isdigit(ch)) {
            gc_release_param(context);
            return str_to_syntax_or_symbol(context, str);
        }
    } else if (!isdigit(ch)) {
        gc_release_param(context);
        return str_to_syntax_or_symbol(context, str);
    }

    for (; (ch = *cstr_cursor) != 0; ++cstr_cursor) {
        if (!isdigit(ch)) {
            if (ch == '.') {
                if (!has_dec_point) {
                    // 指数部分不能有小数点
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
                          opcode procedure 类型检查
******************************************************************************/
// 没有检查
#define TYPE_NONE         NULL
// 任何类型
#define TYPE_ANY            "\001"
#define TYPE_INTEGER        "\002"
#define TYPE_REAL           "\003"
#define TYPE_NATURAL        "\004"
#define TYPE_CHAR           "\005"
#define TYPE_STRING         "\006"
#define TYPE_SYMBOL         "\007"
#define TYPE_PAIR           "\010"
#define TYPE_LIST           "\011"
#define TYPE_VECTOR         "\012"
#define TYPE_PORT           "\013"
#define TYPE_INPUT_PORT     "\014"
#define TYPE_OUTPUT_PORT    "\015"
#define TYPE_ENVIRONMENT    "\016"

// 最大参数数量无限制
#define ARGS_INF            (INT64_MAX-1)

// 001
static int is_any_test(object obj) { return 1; }

// 003
static int is_real_test(object obj) { return is_doublenum(obj); }

// 004
static int is_natural_test(object obj) { return is_i64(obj) && i64_getvalue(obj) >= 0; }

// 005
static int is_char_test(object obj) { return is_imm_char(obj); }

// 006
static int is_string_test(object obj) { return is_string(obj); }

// 007
static int is_symbol_test(object obj) { return is_symbol(obj); }

// 010
static int is_pair_test(object obj) { return is_pair(obj); }

// 011
static int is_list_test(object obj) { return obj == IMM_UNIT && is_pair(obj); }

// 012
static int is_vector_test(object obj) { return is_vector(obj); }

// 013
static int is_port_test(object obj) { return is_port(obj); }

// 014
static int is_input_port_test(object obj) { return is_port_input(obj); }

// 015
static int is_output_port_test(object obj) { return is_port_output(obj); }

// 016
static int is_environment_test(object obj) { return is_ext_type_environment(obj); }

typedef int (*type_test_func)(object);

struct type_test_t {
    type_test_func test;
    const char *type_kind;
} type_test_table[] = {
        {.test = NULL, .type_kind = NULL},                              // NULL 未使用: 不做类型检查
        {.test = is_any_test, .type_kind = "any"},                      // 001
        {.test = is_i64, .type_kind = "integer"},                       // 002
        {.test = is_real_test, .type_kind = "real number"},             // 003
        {.test = is_natural_test, .type_kind = "non-negative integer"}, // 004
        {.test = is_char_test, .type_kind = "char"},                    // 005
        {.test = is_string_test, .type_kind = "string"},                // 006
        {.test = is_symbol_test, .type_kind = "symbol"},                // 007
        {.test = is_pair_test, .type_kind = "pair"},                    // 010
        {.test = is_list_test, .type_kind = "list"},                    // 011
        {.test = is_vector_test, .type_kind = "vector"},                // 012
        {.test = is_port_test, .type_kind = "port"},                    // 013
        {.test = is_input_port_test, .type_kind = "input port"},        // 014
        {.test = is_output_port_test, .type_kind = "output port"},      // 015
        {.test = is_environment_test, .type_kind = "environment"},      // 016
};


/******************************************************************************
                         内部 opcode dispatch table
******************************************************************************/
// repl 控制
static object op_exec_repl(context_t context, enum opcode_e opcode);

// 语法实现
static object op_exec_syntax(context_t context, enum opcode_e opcode);

// 基础运算
static object op_exec_compute(context_t context, enum opcode_e opcode);

// 词法分析和字符串输出
static object op_exec_lexical(context_t context, enum opcode_e opcode);

// 谓词函数
static object op_exec_predicate(context_t context, enum opcode_e opcode);

// 内建函数
static object op_exec_builtin_function(context_t context, enum opcode_e opcode);

/**
 * op_exec_ 分发表内部定义
 */
op_code_info internal_dispatch_table[] = {
        {NULL,    0, 0,        TYPE_NONE, OP_READ_SEXP,          op_exec_lexical},
        {"error", 0, 0,        TYPE_NONE, OP_ERROR,              op_exec_builtin_function},
        {NULL,    1, ARGS_INF, TYPE_NONE, OP_ERROR_PRINT_OBJECT, op_exec_builtin_function},
};

/**
 * op_exec_ 参数类型检查表
 */

static GC void init_opcode(context_t context) {
    for (size_t i = 0; i < sizeof(internal_dispatch_table) / sizeof(op_code_info); i++) {
        op_code_info *info = &internal_dispatch_table[i];
        int op = info->op;
        context->dispatch_table[op].op = info->op;
        context->dispatch_table[op].func = info->func;
        context->dispatch_table[op].min_args_length = info->min_args_length;
        context->dispatch_table[op].max_args_length = info->max_args_length;
        context->dispatch_table[op].op = info->op;

        if (info->name != NULL) {
            // 构造内部 proc
            assign_proc(context, info->name, info->op);
        }
    }
}


/******************************************************************************
                                解释器初始化
******************************************************************************/
/**
 * 初始化环境
 * @param context
 * @return 0: 初始化失败; 1: 初始化成功
 */
static int interpreter_default_env_init(context_t context) {
    gc_var1(context, tmp);

    context->debug = 0;
    context->repl_mode = 0;

    // 全局符号表 global_symbol_table 弱引用 hashset
    context->global_symbol_table =
            weak_hashset_make_op(context, GLOBAL_SYMBOL_TABLE_INIT_SIZE, DEFAULT_HASH_SET_MAP_LOAD_FACTOR);
    context->gensym_count = 0;
    // 全局 environment
    tmp = pair_make_op(context, IMM_UNIT, IMM_UNIT);
    pair_car(tmp) = hashmap_make_op(context, GLOBAL_ENVIRONMENT_INIT_SIZE, DEFAULT_HASH_SET_MAP_LOAD_FACTOR);
    context->global_environment = tmp;
    // 设置 env 类型标记
    set_ext_type_environment(context->global_environment);

    // syntax_table
    // 30 看起来很适合, 30 > 17/0.75
    context->syntax_table = hashmap_make_op(context, 30, DEFAULT_HASH_SET_MAP_LOAD_FACTOR);

    context->load_stack = stack_make_op(context, MAX_LOAD_FILE_DEEP);
    // 每加载一次文件, 要新增一个词法分析器的括号深度记录
    context->bracket_level_stack = stack_make_op(context, MAX_LOAD_FILE_DEEP);
    context->bracket_level = 0;

    // opcode 初始化为 0, 需要后续正确初始化
    context->opcode = 0;
    context->value = IMM_UNIT;
    context->args = IMM_UNIT;
    context->code = IMM_UNIT;

    context->current_env = context->global_environment;
    context->scheme_stack = IMM_UNIT;

    // 默认输入输出
    context->in_port = stdio_port_from_file_op(context, stdin, PORT_INPUT);
    context->out_port = stdio_port_from_file_op(context, stdout, PORT_OUTPUT);
    context->err_out_port = stdio_port_from_file_op(context, stderr, PORT_OUTPUT);
    context->load_port = IMM_UNIT;
    context->save_port = IMM_UNIT;

    // 初始化 opcode dispatch_table 和内部 proc
    context->dispatch_table = raw_alloc(sizeof(op_code_info) * MAX_OP_CODE);
    notnull_or_return(context->dispatch_table, "[ERROR] context->dispatch_table alloc failed.", 0);
    init_opcode(context);

    // TODO 填写 opcode 初始化关键字
    // todo 添加关键字 (syntax) 记得修改这里
    assign_syntax(context, "lambda", -1);
    assign_syntax(context, "quote", -1);
    assign_syntax(context, "define", -1);
    assign_syntax(context, "if", -1);
    assign_syntax(context, "else", -1);
    assign_syntax(context, "begin", -1);
    assign_syntax(context, "set!", -1);
    assign_syntax(context, "let", -1);
    assign_syntax(context, "let*", -1);
    assign_syntax(context, "letrec", -1);
    assign_syntax(context, "cond", -1);
    assign_syntax(context, "delay", -1);
    assign_syntax(context, "and", -1);
    assign_syntax(context, "or", -1);
    assign_syntax(context, "cons-stream", -1);
    assign_syntax(context, "macro", -1);
    assign_syntax(context, "case", -1);

    // 初始化内部 hook 变量
    context->ERROR_HOOK = symbol_make_from_cstr_op(context, "*error-hook*");
    context->COMPILE_HOOK = symbol_make_from_cstr_op(context, "*compile-hook*");

    // 环境初始化结束
    context->init_done = 1;
    gc_release_var(context);
    return 1;
}

/**
 * 创建解释器
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
 * 析构解释器
 * @param context
 */
EXPORT_API void interpreter_destroy(context_t context) {
    // todo 修改 context 后修改 interpreter_destroy
    if (context == NULL) return;

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

    // 初始化内部 hook 变量
    context->ERROR_HOOK = IMM_UNIT;
    context->COMPILE_HOOK = IMM_UNIT;

    context->in_port = IMM_UNIT;
    context->out_port = IMM_UNIT;
    context->err_out_port = IMM_UNIT;

    context->load_port = IMM_UNIT;
    context->save_port = IMM_UNIT;

    context->saves = NULL;

    // 全局清理, 因为根引用都被清理了
    gc_collect(context);

    context->init_done = 0;
    context_destroy(context);
}

/******************************************************************************
                         global_symbol_table 操作
******************************************************************************/
/**
 * 生成当前环境下唯一 symbol
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
            // 冲突
            continue;
        } else {
            // 全局符号表中没有对应符号
            new_sym = global_symbol_add_from_symbol_obj(context, new_sym);
            gc_release_var(context);
            return new_sym;
        }
    }

    gc_release_var(context);
    return IMM_UNIT;
}
/**
 * 向全局符号表添加 symbol 并返回这个 symbol
 * @param context
 * @param symbol symbol 对象
 * @param 添加后的 symbol 对象, 如果存在则返回原始对象
 */
EXPORT_API OUT NOTNULL GC object
global_symbol_add_from_symbol_obj(REF NOTNULL context_t context, REF NOTNULL object symbol) {
    assert(context != NULL);
    assert(is_symbol(symbol));
    assert(is_weak_hashset(context->global_symbol_table));
    gc_param1(context, symbol);

    symbol = weak_hashset_put_op(context, context->global_symbol_table, symbol);
    set_immutable(symbol);
    gc_release_param(context);
    return symbol;
}

/**
 * 从全局符号表中查找是否存在此符号
 * <p>不会触发 GC</p>
 * @param context
 * @param symbol
 * @return 存在返回 IMM_TRUE, 否则返回 IMM_FALSE
 */
EXPORT_API OUT NOTNULL object
global_symbol_found(REF NOTNULL context_t context, REF NOTNULL object symbol) {
    assert(context != NULL);
    assert(is_symbol(symbol));

    return weak_hashset_contains_op(context, context->global_symbol_table, symbol);
}

/**
 * 返回全局符号表全部内容
 * @param context
 * @return vector, vector 中可能存在 IMM_UNIT
 */
EXPORT_API OUT NOTNULL GC object
global_symbol_all_symbol(REF NOTNULL context_t context) {
    assert(context != NULL);
    return weak_hashset_to_vector_op(context, context->global_symbol_table);
}

/**
 * 构造 symbol 类型对象
 * <p>symbol_len() 运算结果不包括 '\0', 但是 object->symbol.len 包括 '\0', 这是为了方便运行时计算对象大小</p>
 * <p>该方法会把 symbol 添加进全局符号表</p>
 * @param context
 * @param cstr C字符串, '\0'结尾
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
 * string 转 symbol
 * <p>该方法会把 symbol 添加进全局符号表</p>
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
 * string_buffer 转换为 symbol, 深拷贝
 * <p>该方法会把 symbol 添加进全局符号表</p>
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
                             scheme_stack 操作
******************************************************************************/
EXPORT_API void scheme_stack_reset(context_t context) {
    assert(context != NULL);
    context->scheme_stack = IMM_UNIT;
}

EXPORT_API GC void scheme_stack_save(context_t context, enum opcode_e op, object args, object code) {
    assert(context != NULL);
    assert(is_pair(args) || args == IMM_UNIT);
    assert(is_pair(code) || code == IMM_UNIT);

    gc_param2(context, args, code);
    gc_var1(context, tmp);

    // 保存栈帧
    tmp = stack_frame_make_op(context, op, args, code, context->current_env);
    // 入栈栈
    context->scheme_stack = pair_make_op(context, tmp, context->scheme_stack);

    gc_release_param(context);
}

EXPORT_API object scheme_stack_return(context_t context, object value) {
    assert(context != NULL);
    assert(is_pair(context->scheme_stack) || context->scheme_stack == IMM_UNIT);

    // 返回值赋值
    context->value = value;

    if (context->scheme_stack == IMM_UNIT) {
        // 栈空
        // TODO 这是个错误, 应该由元循环处理
        return IMM_UNIT;
    }

    // 取出顶层栈帧
    object stack_frame = pair_car(context->scheme_stack);
    // 恢复环境
    context->opcode = stack_frame_op(stack_frame);
    context->args = stack_frame_args(stack_frame);
    context->code = stack_frame_code(stack_frame);
    context->current_env = stack_frame_env(stack_frame);

    context->scheme_stack = pair_cdr(context->scheme_stack);
    return value;
}

/******************************************************************************
                              environment 操作
******************************************************************************/
/**
 * 从 context->current_env 向上查找 env_slot
 * <p>不会触发 GC</p>
 * @param context
 * @param symbol
 * @param all 1: 持续查找到全局 env; 0: 只查找当前 env
 * @return env_slot / IMM_UNIT (未找到)
 */
EXPORT_API object find_slot_in_current_env(REF NOTNULL context_t context, object symbol, int all) {
    return find_slot_in_spec_env(context, context->current_env, symbol, all);
}
/**
 * 从特定 env 向上查找 env_slot
 * <p>不会触发 GC</p>
 * @param context
 * @param symbol
 * @param all 1: 持续查找到全局 env; 0: 只查找当前 environment
 * @return env_slot / IMM_UNIT (未找到)
 */
EXPORT_API object find_slot_in_spec_env(REF NOTNULL context_t context, object env, object symbol, int all) {
    assert(context != NULL);

    int first_env = 1;
    int found = 0;
    object ret_slot = IMM_UNIT;

    for (object cur = env; cur != IMM_UNIT; cur = pair_cdr(cur)) {

        if (is_hashmap(pair_car(cur))) {
            ret_slot = hashmap_get_op(context, pair_car(cur), symbol);
            // 找不到的话 刚好返回 IMM_UNIT
            break;
        } else {

            // 否则的话为单个 env frame 链
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

        // 只查找一次
        if (first_env) {
            first_env = 0;
            if (!all) break;
        }

        // 找到的话结束
        if (found == 1) break;
    }

    return ret_slot;
}
/**
 * 在当前 context->current_env 插入新 env_slot
 * @param context
 * @param symbol
 * @param value
 */
EXPORT_API GC void new_slot_in_current_env(context_t context, object symbol, object value) {
    new_slot_in_spec_env(context, symbol, value, context->current_env);
}
/**
 * 从特定 env frame 插入新 env_slot
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
 * 以 context->current_env 作为上层, 创建新 frame, 赋值给 context->current_env
 * @param context
 * @param old_env 一般是 context->current_env
 */
EXPORT_API GC void new_frame_push_current_env(context_t context) {
    new_frame_push_spec_env(context, context->current_env);
}
/**
 * 以 old_env 作为上层, 创建新 frame, 赋值给 context->current_env
 * @param context
 * @param old_env 一般是 context->current_env
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
                              load_stack 操作
******************************************************************************/
/**
 * (load "") 函数推入 port
 * @param context
 * @param file_name
 * @return 成功返回 IMM_TRUE, 否则 IMM_FALSE
 */
static GC object file_push(context_t context, object file_name) {
    assert(context != NULL);
    gc_param1(context, file_name);
    gc_var3(context, port, name, bracket_level);

    port = stdio_port_from_filename_op(context, file_name, PORT_INPUT);
    if (port != IMM_UNIT) {
        // load stack 不足将自动增长
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
        stack_pop_op(context->load_stack);
        stack_pop_op(context->bracket_level_stack);

        assert(port != NULL);
        stdio_finalizer(context, port);
        context->load_port = stack_peek_op(context->load_stack);
        context->bracket_level = i64_getvalue(stack_peek_op(context->bracket_level_stack));
    }
}


/******************************************************************************
                                    元循环
******************************************************************************/

/**
 * 构造 OP_ERROR 的参数并跳转到 ERROR
 * <p>异常结构</p>
 * <p>   '(error_message_str)</p>
 * <p>或 '(error_message_str object)</p>
 * @param context
 * @param message
 * @param obj
 * @return
 */
static object error_throw(context_t context, const char *message, object obj) {
    assert(context != 0);
    gc_param1(context, obj);
    gc_var2(context, strbuff, str);

    char format_buff[__Format_buff_size__];

    strbuff = string_buffer_make_op(context, 512);
    if (is_stdio_port(context->in_port) && stdio_port_get_file(context->in_port) != stdin) {
        // 显示错误行数
        string_buffer_append_cstr_op(context, strbuff, "(");
        string_buffer_append_string_op(context, strbuff, stdio_port_get_filename(context->in_port));
        string_buffer_append_cstr_op(context, strbuff, " : ");
        snprintf(format_buff, __Format_buff_size__, "%zu", stdio_port_get_line(context->in_port));
        string_buffer_append_cstr_op(context, strbuff, format_buff);
        string_buffer_append_cstr_op(context, strbuff, ") ");
        string_buffer_append_cstr_op(context, strbuff, message);
    }

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

#define Error_Throw_0(ctx, msg)       return error_throw(ctx, msg, NULL)
#define Error_Throw_1(ctx, msg, obj)    return error_throw(ctx, msg, obj)

/**
 * 检查内建过程参数
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
        // 参数长度不足
        ok_flag = 0;
        snprintf(context->str_buffer, INTERNAL_STR_BUFFER_SIZE, "%s: needs%s %"PRId64" argument(s)",
                 vptr->name,
                 vptr->min_args_length == vptr->max_args_length ? "" : " at least",
                 vptr->min_args_length);
    }
    // max_args_length == ARGS_INF 的时候跳过最大参数长度检查
    if (ok_flag && vptr->max_args_length != ARGS_INF && n > vptr->max_args_length) {
        // 参数过多
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

            if (type_check[1] != '\0') {
                // 重复最后一个测试
                type_check++;
            }

            args = pair_cdr(args);
        }
    }

    if (!ok_flag) {
        Error_Throw_0(context, context->str_buffer);
    } else {
        return IMM_TRUE;
    }
}

EXPORT_API GC void eval_apply_loop(context_t context, enum opcode_e opcode) {
    context->opcode = opcode;
    while (1) {
        // 根据 opcode 取得对应函数
        assert(context->opcode >= 0 && context->opcode <= MAX_OP_CODE);
        op_code_info *vptr = &context->dispatch_table[context->opcode];

        // 内建函数则检查参数是否正确
        if (vptr->name != NULL) {
            builtin_function_args_type_check(context, vptr);
        }

        // 执行一次循环
        if (vptr->func(context, context->opcode) == IMM_UNIT) {
            // 执行结束或出现错误时返回
            // 运行结果见 context->ret
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

#define s_return(context, value)    scheme_stack_return((context), (value))

static int is_file_interactive(context_t context) {
    return stack_len(context->load_stack) == 1 &&
           stdio_port_get_file(stack_peek_op(context->load_stack)) &&
           is_stdio_port(context->in_port);
}

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

            // 如果到达文件结尾则结束循环
            // 如果载入的是 stdin, 永远不会遇到 EOF, 因此 repl 不会结束循环
            if (is_port_eof(context->load_port)) {
                // 如果 load_stack 最后一个文件遇到 EOF, 或者 load_stack 为空
                // 此时结束解释器循环
                if (stack_len(context->load_stack) <= 1) {
                    context->args = IMM_UNIT;
                    s_goto(context, OP_QUIT);
                } else {
                    file_pop(context);
                    s_return(context, context->value);
                }
            }

            // 如果 port 是 stdin 则显示提示符
            if (is_file_interactive(context)) {
                context->current_env = context->global_environment;
                scheme_stack_reset(context);
                port_put_cstr(context, context->out_port, "\n"USER_OVERTURE);
            }

            // 设置新的 repl 循环
            // 重设括号层级深度
            stack_set_top_op(context->bracket_level_stack, i64_imm_make(0));
            context->bracket_level = 0;
            // 在 OP_READ_INTERNAL 结束前临时保存 context->in_port
            context->save_port = context->in_port;
            context->in_port = context->load_port;
            s_save(context, OP_TOP_LEVEL_SETUP, IMM_UNIT, IMM_UNIT);
            s_save(context, OP_VALUE_PRINT, IMM_UNIT, IMM_UNIT);
            s_save(context, OP_TOP_LEVEL, IMM_UNIT, IMM_UNIT);

            s_goto(context, OP_READ_INTERNAL);

        case OP_TOP_LEVEL:
            // OP_READ_INTERNAL 结束后, 返回值是 sexp
            context->code = context->value;
            context->in_port = context->save_port;
            // OP_EVAL 应当处理 context->code == IMM_EOF 的情况
            s_goto(context, OP_EVAL);

        case OP_READ_INTERNAL:
            // return: sexp
            context->token = token(context);
            if (context->token == TOKEN_EOF) {
                // 直接到达文件结尾, 返回 OP_TOP_LEVEL
                s_return(context, IMM_EOF);
            }
            s_goto(context, OP_READ_SEXP);

        case OP_VALUE_PRINT:
        case OP_EVAL:
        case OP_APPLY:
        default:
            snprintf(context->str_buffer, INTERNAL_STR_BUFFER_SIZE, "%d: illegal operator", opcode);
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

    // 词法分析
    switch (opcode) {
        case OP_READ_SEXP:
            switch (context->token) {
                case TOKEN_EOF:
                    // 文件结束
                    s_return(context, IMM_EOF);
                case TOKEN_VECTOR:
                    // "#(...)", 此时 token() 已经取走 "#("
                    s_save(context, OP_READ_VECTOR, IMM_UNIT, IMM_UNIT);
                    // 接着读入左括号紧接着的内容
                    // 没有 break, 没有 return
                    // 不是 bug
                case TOKEN_LEFT_PAREN:
                    context->token = token(context);
                    if (context->token == TOKEN_RIGHT_PAREN) {
                        // ()
                        s_return(context, IMM_UNIT);
                    } else if (context->token == TOKEN_DOT) {
                        // (.)
                        Error_Throw_0(context, "syntax error: illegal dot expression");
                    } else {
                        // 其他情况, 括号深度 +1
                        int64_t deep = i64_getvalue(stack_peek_op(context->bracket_level_stack));
                        deep += 1;
                        tmp1 = i64_make_op(context, deep);
                        stack_set_top_op(context->bracket_level_stack, tmp1);

                        s_save(context, OP_READ_LIST, IMM_UNIT, IMM_UNIT);
                        // 递归读入 sexp
                        s_goto(context, OP_READ_SEXP);
                    }
                case TOKEN_QUOTE:
                    // ' 引用, 再递归读入 sexp 后由 OP_READ_QUOTE 构造引用
                    s_save(context, OP_READ_QUOTE, IMM_UNIT, IMM_UNIT);
                    context->token = token(context);
                    s_goto(context, OP_READ_SEXP);
                case TOKEN_BACK_QUOTE:
                    // ` 反引用
                    // `(1 2 3)         =>  '(1 2 3)
                    // `(1 ,(+ 2 3) 3)  =>  '(1 5 3)
                    // `#(1 2)          =>  '(1 2)
                    // `#(1 ,(+ 1 2))   =>  '#(1 3)
                    context->token = token(context);

                    if (context->token == TOKEN_VECTOR) {
                        s_save(context, OP_READ_QUASIQUOTE_VECTOR, IMM_UNIT, IMM_UNIT);
                        // 因为 token() 会吞掉 "#()" 的 "#("
                        context->token = TOKEN_LEFT_PAREN;
                        s_goto(context, OP_READ_SEXP);
                    } else {
                        s_save(context, OP_READ_QUASIQUOTE, IMM_UNIT, IMM_UNIT);
                    }

                    // 递归读入 sexp
                    s_goto(context, OP_READ_SEXP);
                case TOKEN_COMMA:
                    // , 逗号 反引用遇到 ","
                    // 在反引用中要先求值后被引用
                    s_save(context, OP_READ_UNQUOTE, IMM_UNIT, IMM_UNIT);
                    context->token = token(context);

                    // 递归读入需要被求值的 sexp
                    s_goto(context, OP_READ_SEXP);
                case TOKEN_AT_MART:
                    // @: 反引用遇到 ",@", 将 list 拼接进引用
                    // `(1 ,@(list 1 2) 4)  =>  '(1 1 2 4)
                    // `#(1 ,@(list 1 2) 4) =>  '#(1 1 2 4)
                    s_save(context, OP_READ_UNQUOTE_SPLICING, IMM_UNIT, IMM_UNIT);
                    context->token = token(context);
                    // 递归读入要被展开的 list
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
                    s_return(context, tmp2);
                case TOKEN_DOUBLE_QUOTE:
                    // "..."  字符串
                    tmp1 = read_string_expr(context);
                    if (tmp1 == IMM_UNIT) {
                        Error_Throw_0(context, "Error reading string");
                    }
                    set_immutable(tmp1);
                    s_return(context, tmp1);
                default:
                    Error_Throw_0(context, "syntax error: illegal token");
            }
        case OP_READ_LIST:
        case OP_READ_DOT:
        case OP_READ_QUOTE:
        case OP_READ_QUASIQUOTE:
        case OP_READ_VECTOR:
        case OP_PRINT_OBJECT:
        case OP_PRINT_LIST:
        case OP_PRINT_VECTOR:
        default:
            snprintf(context->str_buffer, INTERNAL_STR_BUFFER_SIZE, "%d: illegal operator", opcode);
            Error_Throw_0(context, context->str_buffer);
    }

    gc_release_var(context);
    return IMM_TRUE;
}

static object op_exec_syntax(context_t context, enum opcode_e opcode) {
    assert(context != NULL);
    gc_var2(context, tmp1, tmp2);

    switch (opcode) {
        default:
            snprintf(context->str_buffer, INTERNAL_STR_BUFFER_SIZE, "%d: illegal operator", opcode);
            Error_Throw_0(context, context->str_buffer);
    }

    gc_release_var(context);
    return IMM_TRUE;
}

static object op_exec_compute(context_t context, enum opcode_e opcode) {
    assert(context != NULL);
    gc_var2(context, tmp1, tmp2);

    switch (opcode) {
        default:
            snprintf(context->str_buffer, INTERNAL_STR_BUFFER_SIZE, "%d: illegal operator", opcode);
            Error_Throw_0(context, context->str_buffer);
    }

    gc_release_var(context);
    return IMM_TRUE;
}


static object op_exec_predicate(context_t context, enum opcode_e opcode) {
    assert(context != NULL);
    gc_var2(context, tmp1, tmp2);

    switch (opcode) {
        default:
            snprintf(context->str_buffer, INTERNAL_STR_BUFFER_SIZE, "%d: illegal operator", opcode);
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
        case OP_GC:
        case OP_ERROR:
            // args: (error "message" . object)
            assert(is_pair(context->args));

            context->ret = -1;
            // 输出 message
            port_put_cstr(context, context->out_port, "Error: ");
            if (!is_string(pair_car(context->args))) {
                port_put_cstr(context, context->out_port, "--");

            }
            port_put_string(context, context->out_port, pair_car(context->args));

            // 检查剩余的参数
            context->args = pair_cdr(context->args);
            s_goto(context, OP_ERROR_PRINT_OBJECT);
        case OP_ERROR_PRINT_OBJECT:
            // args: (. object)

            port_put_cstr(context, context->out_port, " ");

            if (context->args != IMM_UNIT) {
                // 遍历输出所有参数
                s_save(context, OP_ERROR_PRINT_OBJECT, pair_cdr(context->args), IMM_UNIT);
                context->args = pair_car(context->args);
                s_goto(context, OP_PRINT_OBJECT);
            } else {
                // 参数为空则处理错误后的操作
                port_put_cstr(context, context->out_port, "\n");
                if (context->repl_mode) {
                    // repl 模式则回到 OP_TOP_LEVEL_SETUP
                    s_goto(context, OP_TOP_LEVEL_SETUP);
                } else {
                    // 其它模式直接结束解释器运行
                    // context->ret 在抛出异常前应当先设置
                    gc_release_var(context);
                    return IMM_UNIT;
                }
            }
        default:
            snprintf(context->str_buffer, INTERNAL_STR_BUFFER_SIZE, "%d: illegal operator", opcode);
            Error_Throw_0(context, context->str_buffer);
    }

    gc_release_var(context);
    return IMM_TRUE;
}

/******************************************************************************
                                  文件读入
******************************************************************************/
EXPORT_API GC int interpreter_load_cstr(context_t context, const char *cstr) {
    assert(context != NULL);
    assert(cstr != NULL);

    gc_var2(context, port, str);
    scheme_stack_reset(context);
    context->current_env = context->global_environment;
    stack_clean(context->load_stack);
    stack_clean(context->bracket_level_stack);

    // 填入文件
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
    return context->ret;

    gc_release_var(context);
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
    assert(file != NULL);
    assert(file_name != NULL);

    gc_var1(context, port);
    scheme_stack_reset(context);
    context->current_env = context->global_environment;
    stack_clean(context->load_stack);
    stack_clean(context->bracket_level_stack);

    // 填入文件
    port = stdio_port_from_file_op(context, file, PORT_INPUT);
    context->load_stack = stack_push_auto_increase_op(context, context->load_stack, port, 50);
    context->bracket_level_stack = stack_push_auto_increase_op(context, context->bracket_level_stack, i64_imm_make(0),
                                                               50);
    context->bracket_level = 0;
    context->load_port = port;
    context->ret = 0;
    if (file == stdin) {
        context->repl_mode = 1;
    } else {
        context->repl_mode = 0;
    }

    context->in_port = port;
    return context->ret;

    gc_release_var(context);
}