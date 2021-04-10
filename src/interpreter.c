#include "paper-scheme/interpreter.h"


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
    // 全局 environment
    tmp = pair_make_op(context, IMM_UNIT, IMM_UNIT);
    pair_car(tmp) = hashmap_make_op(context, GLOBAL_ENVIRONMENT_INIT_SIZE, DEFAULT_HASH_SET_MAP_LOAD_FACTOR);
    context->global_environment = tmp;

    context->load_stack = stack_make_op(context, MAX_LOAD_FILE_DEEP);

    // TODO op_code 初始化
    context->op_code = OP_TOP_LEVEL;
    context->value = IMM_UNIT;
    context->args = IMM_UNIT;
    context->code = IMM_UNIT;

    context->current_env = context->global_environment;
    context->scheme_stack = IMM_UNIT;

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

    interpreter_default_env_init(context);
    return context;
}

/**
 * 析构解释器
 * @param context
 */
EXPORT_API void interpreter_destory(context_t context) {
    context->debug = 0;
    context->repl_mode = 0;

    context->args = IMM_UNIT;
    context->code = IMM_UNIT;

    context->current_env = IMM_UNIT;
    context->scheme_stack = IMM_UNIT;

    context->op_code = 0;
    context->value = IMM_UNIT;

    context->load_stack = IMM_UNIT;

    // 全局符号表 global_symbol_table 弱引用 hashset
    context->global_symbol_table = IMM_UNIT;

    // 全局 environment
    context->global_environment = IMM_UNIT;

    context->init_done = 0;

    context_destroy(context);
}

/******************************************************************************
                         global_symbol_table 操作
******************************************************************************/
/**
 * 向全局符号表添加 symbol 并返回这个 symbol
 * @param context
 * @param symbol symbol 对象
 */
EXPORT_API OUT NOTNULL GC void
global_symbol_add_from_symbol_obj(REF NOTNULL context_t context, REF NOTNULL object symbol) {
    assert(context != NULL);
    assert_init_done(context);
    assert(is_symbol(symbol));
    assert(is_weak_hashset(context->global_symbol_table));
    gc_param1(context, symbol);

    weak_hashset_put_op(context, context->global_symbol_table, symbol);
    gc_release_param(context);
}

/**
 * 从全局符号表中查找是否存在此符号
 * <p>不会触发 GC</p>
 * @param context
 * @param symbol
 * @return 存在返回 IMM_TRUE, 否则返回 IMM_UNIT
 */
EXPORT_API OUT NOTNULL object
global_symbol_found(REF NOTNULL context_t context, REF NOTNULL object symbol) {
    assert(context != NULL);
    assert_init_done(context);
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
    assert_init_done(context);
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
    assert_init_done(context);

    gc_var1(context, symbol);
    symbol = symbol_make_from_cstr_untracked_op(context, cstr);
    global_symbol_add_from_symbol_obj(context, symbol);
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
    assert_init_done(context);

    gc_param1(context, str);
    gc_var1(context, symbol);
    symbol = string_to_symbol_untracked_op(context, str);
    global_symbol_add_from_symbol_obj(context, symbol);
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
    assert_init_done(context);

    gc_param1(context, str_buffer);
    gc_var1(context, symbol);
    symbol = string_buffer_to_symbol_untracked_op(context, str_buffer);
    global_symbol_add_from_symbol_obj(context, symbol);
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

    // 填充 stack_frame
    tmp = stack_frame_make_op(context, op, args, code, context->current_env);
    // push 栈
    context->scheme_stack = pair_make_op(context, tmp, context->scheme_stack);

    gc_release_param(context);
}

EXPORT_API object scheme_stack_return(context_t context, object value) {
    assert(context != NULL);
    assert(is_pair(context->scheme_stack) || context->scheme_stack == IMM_UNIT);

    context->value = value;

    if (context->scheme_stack == IMM_UNIT) {
        // 栈空
        // TODO 这是个错误, 应该由元循环处理
        return IMM_UNIT;
    }

    object stack_frame = pair_car(context->scheme_stack);
    // 恢复环境
    context->op_code = stack_frame_op(stack_frame);
    context->args = stack_frame_args(stack_frame);
    context->code = stack_frame_code(stack_frame);
    context->current_env = stack_frame_env(stack_frame);

    context->scheme_stack = pair_cdr(context->scheme_stack);
    return IMM_TRUE;
}

/******************************************************************************
                           TODO   environment 操作
******************************************************************************/
/**
 * 从 context->current_env 向上查找 env_slot
 * <p>不会触发 GC</p>
 * @param context
 * @param symbol
 * @param all 1: 持续查找到全局 env; 0: 只查找当前 environment
 * @return env_slot / IMM_UNIT (未找到)
 */
EXPORT_API object find_slot_in_env(REF NOTNULL context_t context, object env, object symbol, int all) {
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
            for (object slot = pair_car(cur); slot != IMM_UNIT; slot = pair_cdr(slot)) {
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
EXPORT_API GC void new_slot_in_env(context_t context, object symbol, object value) {
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
 * 以 old_env 作为上层, 创建新 frame, 赋值给 context->current_env
 * @param context
 * @param old_env 一般是 context->current_env
 */
EXPORT_API GC void new_frame_in_env(context_t context, object old_env) {
    assert(context != NULL);
    assert(old_env != IMM_UNIT);

    gc_param1(context, old_env);
    context->current_env = pair_make_op(context, IMM_UNIT, old_env);
    gc_release_param(context);
}


/******************************************************************************
                              load_stack 操作
******************************************************************************/


/******************************************************************************
                                    元循环
******************************************************************************/
uint32_t eval_apply_loop(context_t context, enum opcode_e opcode) {
    return 1;
}

static object operation_execute_0(context_t context, enum opcode_e opcode) {
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