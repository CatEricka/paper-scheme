#include "paper-scheme/interpreter.h"



/******************************************************************************
                          opcode procedure ���ͼ��
******************************************************************************/


/******************************************************************************
                         �ڲ� opcode dispatch table
******************************************************************************/
static object op_exec_0(context_t context, enum opcode_e opcode);

static object op_exec_1(context_t context, enum opcode_e opcode);

static object op_exec_2(context_t context, enum opcode_e opcode);

/**
 * op_exec_ �ַ����ڲ�����
 */
op_code_info internal_dispatch_table[] = {
        {READ_SEXP_OP, op_exec_0, NULL, 0, 0, NULL,},
};

/**
 * op_exec_ �������ͼ���
 */

static void register_op_code(context_t context) {
    for (size_t i = 0; i < sizeof(internal_dispatch_table) / sizeof(op_code_info); i++) {
        op_code_info *info = &internal_dispatch_table[i];
        int op = info->op;
        context->dispatch_table[op].op = info->op;
        context->dispatch_table[op].func = info->func;
        context->dispatch_table[op].min_args_length = info->min_args_length;
        context->dispatch_table[op].max_args_length = info->max_args_length;
        context->dispatch_table[op].op = info->op;

        if (info->name == NULL) {
            context->dispatch_table[op].name = IMM_UNIT;
        } else {
            // �����ڲ� proc
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
    context->repl_mode = 0;

    // ȫ�ַ��ű� global_symbol_table ������ hashset
    context->global_symbol_table =
            weak_hashset_make_op(context, GLOBAL_SYMBOL_TABLE_INIT_SIZE, DEFAULT_HASH_SET_MAP_LOAD_FACTOR);
    // ȫ�� environment
    tmp = pair_make_op(context, IMM_UNIT, IMM_UNIT);
    pair_car(tmp) = hashmap_make_op(context, GLOBAL_ENVIRONMENT_INIT_SIZE, DEFAULT_HASH_SET_MAP_LOAD_FACTOR);
    context->global_environment = tmp;
    context->gensym_count = 0;

    context->load_stack = stack_make_op(context, MAX_LOAD_FILE_DEEP);

    // op_code ��ʼ��Ϊ 0, ��Ҫ������ȷ��ʼ��
    context->op_code = 0;
    context->value = IMM_UNIT;
    context->args = IMM_UNIT;
    context->code = IMM_UNIT;

    context->current_env = context->global_environment;
    context->scheme_stack = IMM_UNIT;

    // Ĭ���������
    context->in_port = stdio_port_from_file_op(context, stdin, PORT_INPUT);
    context->out_port = stdio_port_from_file_op(context, stdout, PORT_OUTPUT);
    context->err_out_port = stdio_port_from_file_op(context, stderr, PORT_OUTPUT);

    // ��ʼ�� op code dispatch table
    context->dispatch_table = raw_alloc(sizeof(op_code_info) * MAX_OP_CODE_SIZE);
    notnull_or_return(context->dispatch_table, "[ERROR] context->dispatch_table alloc failed.", 0);
    register_op_code(context);

    // ע���ڲ� proc

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

    // ȫ�ַ��ű� global_symbol_table ������ hashset
    context->global_symbol_table = IMM_UNIT;
    // ȫ�� environment
    context->global_environment = IMM_UNIT;
    context->gensym_count = 0;

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
            global_symbol_add_from_symbol_obj(context, new_sym);
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
 */
EXPORT_API OUT NOTNULL GC void
global_symbol_add_from_symbol_obj(REF NOTNULL context_t context, REF NOTNULL object symbol) {
    assert(context != NULL);
    assert(is_symbol(symbol));
    assert(is_weak_hashset(context->global_symbol_table));
    gc_param1(context, symbol);

    weak_hashset_put_op(context, context->global_symbol_table, symbol);
    gc_release_param(context);
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
    global_symbol_add_from_symbol_obj(context, symbol);
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
    global_symbol_add_from_symbol_obj(context, symbol);
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
    global_symbol_add_from_symbol_obj(context, symbol);
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
    assert(is_pair(args) || args == IMM_UNIT);
    assert(is_pair(code) || code == IMM_UNIT);

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
        // TODO ���Ǹ�����, Ӧ����Ԫѭ������
        return IMM_UNIT;
    }

    // ȡ������ջ֡
    object stack_frame = pair_car(context->scheme_stack);
    // �ָ�����
    context->op_code = stack_frame_op(stack_frame);
    context->args = stack_frame_args(stack_frame);
    context->code = stack_frame_code(stack_frame);
    context->current_env = stack_frame_env(stack_frame);

    context->scheme_stack = pair_cdr(context->scheme_stack);
    return value;
}

/******************************************************************************
                           TODO   environment ����
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
    gc_release_param(context);
}


/******************************************************************************
                              load_stack ����
******************************************************************************/


/******************************************************************************
                                    Ԫѭ��
******************************************************************************/
uint32_t eval_apply_loop(context_t context, enum opcode_e opcode) {
    return 1;
}

static object op_exec_0(context_t context, enum opcode_e opcode) {
    return 0;
}


/******************************************************************************
                                  �ļ�����
******************************************************************************/
EXPORT_API GC void interpreter_load_cstr(context_t context, const char *cstr) {

}

EXPORT_API GC void interpreter_load_file(context_t context, FILE *file) {
    interpreter_load_file_with_name(context, file, NULL);
}

EXPORT_API GC void interpreter_load_file_with_name(context_t context, FILE *file, const char *file_name) {

}