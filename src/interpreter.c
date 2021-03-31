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

    context->args = IMM_UNIT;
    context->code = IMM_UNIT;
    // TODO current_env hash map
    context->current_env = hashmap_make_op(context, DEFAULT_HASH_SET_MAP_INIT_init_CAPACITY,
                                           DEFAULT_HASH_SET_MAP_LOAD_FACTOR);
    tmp = stack_make_op(context, MAX_STACK_BLOCK_DEEP);
    tmp = pair_make_op(context, tmp, IMM_UNIT);
    context->scheme_stack = tmp;

    // TODO op_code 初始化
    context->op_code = OP_TOP_LEVEL;
    context->value = IMM_UNIT;

    context->load_stack = stack_make_op(context, MAX_LOAD_FILE_DEEP);

    // TODO 弱引用 global_symbol_table
    context->global_symbol_table = hashset_make_op(context, GLOBAL_SYMBOL_TABLE_INIT_SIZE,
                                                   DEFAULT_HASH_SET_MAP_LOAD_FACTOR);
    context->global_environment = hashmap_make_op(context, GLOBAL_ENVIRONMENT_INIT_SIZE,
                                                  DEFAULT_HASH_SET_MAP_LOAD_FACTOR);

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

