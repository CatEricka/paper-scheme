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

static GC void assign_proc(context_t context, char *name, enum opcode_e opcode) {
    assert(context != NULL);
    assert(name != NULL);
    assert(strlen(name) != 0);
    assert(opcode >= 0 && opcode < MAX_OP_CODE);
    gc_var2(context, proc, sym);

    sym = symbol_make_from_cstr_op(context, name);
    proc = proc_make_internal(context, sym, opcode);
    new_slot_in_spec_env(context, context->global_environment, sym, proc);

    gc_release_var(context);
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


/******************************************************************************
                          opcode procedure 类型检查
******************************************************************************/
//#define TYPE_NONE         NULL
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

static int is_any_test(object obj) { return 1; }

static int is_real_test(object obj) { return is_doublenum(obj); }

static int is_natural_test(object obj) { return is_i64(obj) && i64_getvalue(obj) >= 0; }

static int is_char_test(object obj) { return is_imm_char(obj); }

static int is_string_test(object obj) { return is_string(obj); }

static int is_symbol_test(object obj) { return is_symbol(obj); }

static int is_pair_test(object obj) { return is_pair(obj); }

static int is_list_test(object obj) { return obj == IMM_UNIT && is_pair(obj); }

static int is_vector_test(object obj) { return is_vector(obj); }

static int is_port_test(object obj) { return is_port(obj); }

static int is_input_port_test(object obj) { return is_port_input(obj); }

static int is_output_port_test(object obj) { return is_port_output(obj); }

static int is_environment_test(object obj) { return is_ext_type_environment(obj); }

typedef int (*type_test_func)(object);

struct type_test_t {
    type_test_func test;
    const char *type_kind;
} type_test_table[] = {
        {.test = NULL, .type_kind = NULL},      // NULL 未使用: 不做类型检查
        {.test = is_any_test, .type_kind = "any"},   // 001
        {.test = is_i64, .type_kind = "integer"},   // 002
        {.test = is_real_test, .type_kind = "real number"},   // 003
        {.test = is_natural_test, .type_kind = "non-negative integer"},   // 004
        {.test = is_char_test, .type_kind = "char"},   // 005
        {.test = is_string_test, .type_kind = "string"},   // 006
        {.test = is_symbol_test, .type_kind = "symbol"},   // 007
        {.test = is_pair_test, .type_kind = "pair"},   // 010
        {.test = is_list_test, .type_kind = "list"},   // 011
        {.test = is_vector_test, .type_kind = "vector"},   // 012
        {.test = is_port_test, .type_kind = "port"},   // 013
        {.test = is_input_port_test, .type_kind = "input port"},   // 014
        {.test = is_output_port_test, .type_kind = "output port"},   // 015
        {.test = is_environment_test, .type_kind = "environment"},   // 016
};


/******************************************************************************
                         内部 opcode dispatch table
******************************************************************************/
static object op_exec_0(context_t context, enum opcode_e opcode);

static object op_exec_1(context_t context, enum opcode_e opcode);

static object op_exec_2(context_t context, enum opcode_e opcode);

/**
 * op_exec_ 分发表内部定义
 */
op_code_info internal_dispatch_table[] = {
        {READ_SEXP_OP, op_exec_0, NULL, 0, 0, NULL},
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

    // op_code 初始化为 0, 需要后续正确初始化
    context->op_code = 0;
    context->value = IMM_UNIT;
    context->args = IMM_UNIT;
    context->code = IMM_UNIT;

    context->current_env = context->global_environment;
    context->scheme_stack = IMM_UNIT;

    // 默认输入输出
    context->in_port = stdio_port_from_file_op(context, stdin, PORT_INPUT);
    context->out_port = stdio_port_from_file_op(context, stdout, PORT_OUTPUT);
    context->err_out_port = stdio_port_from_file_op(context, stderr, PORT_OUTPUT);

    // 初始化 op_code dispatch_table 和内部 proc
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

    context->op_code = 0;
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

    context->in_port = IMM_UNIT;
    context->out_port = IMM_UNIT;
    context->err_out_port = IMM_UNIT;

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
    context->op_code = stack_frame_op(stack_frame);
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
    gc_release_param(context);
}


/******************************************************************************
                              load_stack 操作
******************************************************************************/
static object file_push(context_t context, object file_name);

static object file_pop(context_t context, object file_name);


/******************************************************************************
                                    元循环
******************************************************************************/
uint32_t eval_apply_loop(context_t context, enum opcode_e opcode) {
    return 1;
}

static object op_exec_0(context_t context, enum opcode_e opcode) {
    return 0;
}


/******************************************************************************
                                  文件读入
******************************************************************************/
EXPORT_API GC void interpreter_load_cstr(context_t context, const char *cstr) {

}

EXPORT_API GC void interpreter_load_file(context_t context, FILE *file) {
    interpreter_load_file_with_name(context, file, NULL);
}

EXPORT_API GC void interpreter_load_file_with_name(context_t context, FILE *file, const char *file_name) {

}