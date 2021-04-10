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

    context->args = IMM_UNIT;
    context->code = IMM_UNIT;
    // current_env (hashmap next_env)
    context->current_env = IMM_UNIT;
    tmp = stack_make_op(context, MAX_STACK_BLOCK_DEEP);
    tmp = pair_make_op(context, tmp, IMM_UNIT);
    context->scheme_stack = tmp;

    // TODO op_code ��ʼ��
    context->op_code = OP_TOP_LEVEL;
    context->value = IMM_UNIT;

    context->load_stack = stack_make_op(context, MAX_LOAD_FILE_DEEP);

    // ȫ�ַ��ű� global_symbol_table ������ hashset
    context->global_symbol_table =
            weak_hashset_make_op(context, GLOBAL_SYMBOL_TABLE_INIT_SIZE, DEFAULT_HASH_SET_MAP_LOAD_FACTOR);

    // ȫ�� environment
    context->global_environment =
            hashmap_make_op(context, GLOBAL_ENVIRONMENT_INIT_SIZE, DEFAULT_HASH_SET_MAP_LOAD_FACTOR);

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
                          global_environment ����
******************************************************************************/



/******************************************************************************
                               current_env ����
******************************************************************************/


/******************************************************************************
                                    Ԫѭ��
******************************************************************************/
uint32_t eval_apply_loop(context_t context, enum opcode_e opcode) {


    return 1;
}

static object operation_execute_0(context_t context, enum opcode_e opcode) {

}