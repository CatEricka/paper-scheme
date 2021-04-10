#include "paper-scheme/interpreter.h"


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

    context->load_stack = stack_make_op(context, MAX_LOAD_FILE_DEEP);

    // TODO op_code ��ʼ��
    context->op_code = OP_TOP_LEVEL;
    context->value = IMM_UNIT;
    context->args = IMM_UNIT;
    context->code = IMM_UNIT;

    context->current_env = context->global_environment;
    context->scheme_stack = IMM_UNIT;

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

    interpreter_default_env_init(context);
    return context;
}

/**
 * ����������
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

    // ȫ�ַ��ű� global_symbol_table ������ hashset
    context->global_symbol_table = IMM_UNIT;

    // ȫ�� environment
    context->global_environment = IMM_UNIT;

    context->init_done = 0;

    context_destroy(context);
}

/******************************************************************************
                         global_symbol_table ����
******************************************************************************/
/**
 * ��ȫ�ַ��ű���� symbol ��������� symbol
 * @param context
 * @param symbol symbol ����
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
 * ��ȫ�ַ��ű��в����Ƿ���ڴ˷���
 * <p>���ᴥ�� GC</p>
 * @param context
 * @param symbol
 * @return ���ڷ��� IMM_TRUE, ���򷵻� IMM_UNIT
 */
EXPORT_API OUT NOTNULL object
global_symbol_found(REF NOTNULL context_t context, REF NOTNULL object symbol) {
    assert(context != NULL);
    assert_init_done(context);
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
    assert_init_done(context);
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
    assert_init_done(context);

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
    assert_init_done(context);

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
    assert_init_done(context);

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

    // ��� stack_frame
    tmp = stack_frame_make_op(context, op, args, code, context->current_env);
    // push ջ
    context->scheme_stack = pair_make_op(context, tmp, context->scheme_stack);

    gc_release_param(context);
}

EXPORT_API object scheme_stack_return(context_t context, object value) {
    assert(context != NULL);
    assert(is_pair(context->scheme_stack) || context->scheme_stack == IMM_UNIT);

    context->value = value;

    if (context->scheme_stack == IMM_UNIT) {
        // ջ��
        // TODO ���Ǹ�����, Ӧ����Ԫѭ������
        return IMM_UNIT;
    }

    object stack_frame = pair_car(context->scheme_stack);
    // �ָ�����
    context->op_code = stack_frame_op(stack_frame);
    context->args = stack_frame_args(stack_frame);
    context->code = stack_frame_code(stack_frame);
    context->current_env = stack_frame_env(stack_frame);

    context->scheme_stack = pair_cdr(context->scheme_stack);
    return IMM_TRUE;
}

/******************************************************************************
                           TODO   environment ����
******************************************************************************/
/**
 * �� context->current_env ���ϲ��� env_slot
 * <p>���ᴥ�� GC</p>
 * @param context
 * @param symbol
 * @param all 1: �������ҵ�ȫ�� env; 0: ֻ���ҵ�ǰ environment
 * @return env_slot / IMM_UNIT (δ�ҵ�)
 */
EXPORT_API object find_slot_in_env(REF NOTNULL context_t context, object env, object symbol, int all) {
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
EXPORT_API GC void new_slot_in_env(context_t context, object symbol, object value) {
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
 * �� old_env ��Ϊ�ϲ�, ������ frame, ��ֵ�� context->current_env
 * @param context
 * @param old_env һ���� context->current_env
 */
EXPORT_API GC void new_frame_in_env(context_t context, object old_env) {
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

static object operation_execute_0(context_t context, enum opcode_e opcode) {
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