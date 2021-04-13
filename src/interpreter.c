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
                          opcode procedure ���ͼ��
******************************************************************************/
//#define TYPE_NONE         NULL
// �κ�����
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
        {.test = NULL, .type_kind = NULL},                              // NULL δʹ��: �������ͼ��
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
                         �ڲ� opcode dispatch table
******************************************************************************/
// repl ����
static object op_exec_repl(context_t context, enum opcode_e opcode);

// �﷨ʵ��
static object op_exec_syntax(context_t context, enum opcode_e opcode);

// �ʷ��������ַ������
static object op_exec_lexical(context_t context, enum opcode_e opcode);

// ν�ʺ���
static object op_exec_predicate(context_t context, enum opcode_e opcode);

// �ڽ�����
static object op_exec_builtin_function(context_t context, enum opcode_e opcode);

/**
 * op_exec_ �ַ����ڲ�����
 */
op_code_info internal_dispatch_table[] = {
        {OP_READ_SEXP, op_exec_lexical,          NULL, 0, 0, NULL},
        {OP_ERROR,     op_exec_builtin_function, NULL, 0, 0, NULL},
};

/**
 * op_exec_ �������ͼ���
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

    // syntax_table
    // 30 ���������ʺ�, 30 > 17/0.75
    context->syntax_table = hashmap_make_op(context, 30, DEFAULT_HASH_SET_MAP_LOAD_FACTOR);

    context->load_stack = stack_make_op(context, MAX_LOAD_FILE_DEEP);
    // ÿ����һ���ļ�, Ҫ����һ���ʷ���������������ȼ�¼
    context->bracket_level_stack = stack_make_op(context, MAX_LOAD_FILE_DEEP);

    // opcode ��ʼ��Ϊ 0, ��Ҫ������ȷ��ʼ��
    context->opcode = 0;
    context->value = IMM_UNIT;
    context->args = IMM_UNIT;
    context->code = IMM_UNIT;

    context->current_env = context->global_environment;
    context->scheme_stack = IMM_UNIT;

    // Ĭ���������
    context->in_port = stdio_port_from_file_op(context, stdin, PORT_INPUT);
    context->out_port = stdio_port_from_file_op(context, stdout, PORT_OUTPUT);
    context->err_out_port = stdio_port_from_file_op(context, stderr, PORT_OUTPUT);
    context->load_port = IMM_UNIT;

    // ��ʼ�� opcode dispatch_table ���ڲ� proc
    context->dispatch_table = raw_alloc(sizeof(op_code_info) * MAX_OP_CODE);
    notnull_or_return(context->dispatch_table, "[ERROR] context->dispatch_table alloc failed.", 0);
    init_opcode(context);

    // TODO ��д opcode ��ʼ���ؼ���
    // todo ��ӹؼ��� (syntax) �ǵ��޸�����
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

    context->in_port = IMM_UNIT;
    context->out_port = IMM_UNIT;
    context->err_out_port = IMM_UNIT;

    context->saves = NULL;

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
    set_immutable(symbol);
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
    context->opcode = stack_frame_op(stack_frame);
    context->args = stack_frame_args(stack_frame);
    context->code = stack_frame_code(stack_frame);
    context->current_env = stack_frame_env(stack_frame);

    context->scheme_stack = pair_cdr(context->scheme_stack);
    return value;
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
        stack_pop_op(context->load_stack);
        assert(port != NULL);

        stdio_finalizer(context, port);
        context->load_port = stack_peek_op(context->load_stack);
    }
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
    gc_var2(context, strbuff, str);

#define __Format_buff_size 30
    char format_buff[__Format_buff_size];

    strbuff = string_buffer_make_op(context, 512);
    if (is_stdio_port(context->in_port) && stdio_port_get_file(context->in_port) != stdin) {
        // ��ʾ��������
        string_buffer_append_cstr_op(context, strbuff, "(");
        string_buffer_append_string_op(context, strbuff, stdio_port_get_filename(context->in_port));
        string_buffer_append_cstr_op(context, strbuff, " : ");
        snprintf(format_buff, __Format_buff_size, "%zu", stdio_port_get_line(context->in_port));
        string_buffer_append_cstr_op(context, strbuff, format_buff);
        string_buffer_append_cstr_op(context, strbuff, ") ");
        string_buffer_append_cstr_op(context, strbuff, message);
    }
#undef __Format_buff_size

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
    return IMM_UNIT;
}

#define Error_Throw_0(ctx, msg)       error_throw(ctx, msg, NULL)
#define Error_Throw_1(ctx, msg, obj)    error_throw(ctx, msg, obj)

/**
 * ����ڽ����̲���
 * @param context
 * @param vptr
 */
static void builtin_function_args_type_check(context_t context, op_code_info *vptr) {
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
    if (ok_flag && n > vptr->max_args_length) {
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
        for (size_t i = 0, type_check_index = 0; i < n; i++) {
            int index = (int) (*type_check);
            type_test_func test = type_test_table[index].test;
            if (!test(pair_car(args))) {
                ok_flag = 0;
                snprintf(context->str_buffer, INTERNAL_STR_BUFFER_SIZE, "%s: argument %d must be: %s",
                         vptr->name,
                         i + 1,
                         type_test_table[index].type_kind);
                break;
            }

            if (type_check[1] != '\0') {
                // �ظ����һ������
                type_check++;
            }

            args = pair_cdr(args);
        }
    }

    if (!ok_flag) {
        Error_Throw_0(context, context->str_buffer);
    }
}

EXPORT_API GC void eval_apply_loop(context_t context, enum opcode_e opcode) {
    context->opcode = opcode;
    while (1) {
        // ���� opcode ȡ�ö�Ӧ����
        assert(context->opcode >= 0 && context->opcode <= MAX_OP_CODE);
        op_code_info *vptr = &context->dispatch_table[context->opcode];

        // �ڽ�������������Ƿ���ȷ
        if (vptr->name != NULL) {
            builtin_function_args_type_check(context, vptr);
        }

        // ִ��һ��ѭ��
        if (vptr->func(context, context->opcode) == IMM_UNIT) {
            // ִ�н�������ִ���ʱ����
            // ���н���� context->ret
            return;
        }
    }
}

static object op_exec_repl(context_t context, enum opcode_e opcode) {
    return IMM_TRUE;
}

static object op_exec_syntax(context_t context, enum opcode_e opcode) {
    return IMM_TRUE;
}

static object op_exec_lexical(context_t context, enum opcode_e opcode) {
    return IMM_TRUE;
}

static object op_exec_predicate(context_t context, enum opcode_e opcode) {
    return IMM_TRUE;
}

static object op_exec_builtin_function(context_t context, enum opcode_e opcode) {
    return IMM_TRUE;
}

/******************************************************************************
                                  �ļ�����
******************************************************************************/
EXPORT_API GC void interpreter_load_cstr(context_t context, const char *cstr) {

}

EXPORT_API GC void interpreter_load_file(context_t context, FILE *file) {
    if (file == stdin) {
        interpreter_load_file_with_name(context, file, "<stdin>");

    } else {
        interpreter_load_file_with_name(context, file, "<unknown>");
    }
}

EXPORT_API GC void interpreter_load_file_with_name(context_t context, FILE *file, const char *file_name) {
}