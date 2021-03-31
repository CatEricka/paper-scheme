#include <paper-scheme/context.h>


/**
                                ����ʱ������Ϣ
******************************************************************************/
/**
 * �������Ͷ���
 * �ⲿ�ָ�ֵ�� context �д�ʱ�޷��Ծ�, Ӧ�������ٴ���, .name ����Ӧ�������ڱ���
 */
static struct object_runtime_type_info_t scheme_type_specs[OBJECT_TYPE_ENUM_MAX] = {
        {
                // �ⲿ�ָ�ֵ��ʱ�޷��Ծ�, Ӧ�������ٴ���, .name ����Ӧ�������ڱ���
                .name = (object) "I64", .tag = OBJ_I64,
                .getter = IMM_FALSE, .setter = IMM_FALSE, .to_string = IMM_FALSE,

                .member_base = 0,
                .member_eq_len_base = 0,
                .member_len_base = 0,
                .member_meta_len_offset = 0,
                .member_meta_len_scale = 0,

                .size_base = object_sizeof_base(i64),
                .size_meta_size_offset = 0,
                .size_meta_size_scale = 0,
                .finalizer = NULL,
                .hash_code = NULL,
                .equals = NULL,
        },
        {
                .name = (object) "Doublenum", .tag = OBJ_D64,
                .getter = IMM_FALSE, .setter = IMM_FALSE, .to_string = IMM_FALSE,

                .member_base = 0,
                .member_eq_len_base = 0,
                .member_len_base = 0,
                .member_meta_len_offset = 0,
                .member_meta_len_scale = 0,

                .size_base = object_sizeof_base(doublenum),
                .size_meta_size_offset = 0,
                .size_meta_size_scale = 0,
                .finalizer = NULL,
                .hash_code = NULL,
                .equals = NULL,
        },
        {
                .name = (object) "Char", .tag = OBJ_CHAR,
                .getter = IMM_FALSE, .setter = IMM_FALSE, .to_string = IMM_FALSE,

                .member_base = 0,
                .member_eq_len_base = 0,
                .member_len_base = 0,
                .member_meta_len_offset = 0,
                .member_meta_len_scale = 0,

                .size_base = 0,
                .size_meta_size_offset = 0,
                .size_meta_size_scale = 0,
                .finalizer = NULL,
                .hash_code = NULL,
                .equals = NULL,
        },
        {
                .name = (object) "Boolean", .tag = OBJ_BOOLEAN,
                .getter = IMM_FALSE, .setter = IMM_FALSE, .to_string = IMM_FALSE,

                .member_base = 0,
                .member_eq_len_base = 0,
                .member_len_base = 0,
                .member_meta_len_offset = 0,
                .member_meta_len_scale = 0,

                .size_base = 0,
                .size_meta_size_offset = 0,
                .size_meta_size_scale = 0,
                .finalizer = NULL,
                .hash_code = NULL,
                .equals = NULL,
        },
        {
                .name = (object) "Unit", .tag = OBJ_UNIT,
                .getter = IMM_FALSE, .setter = IMM_FALSE, .to_string = IMM_FALSE,

                .member_base = 0,
                .member_eq_len_base = 0,
                .member_len_base = 0,
                .member_meta_len_offset = 0,
                .member_meta_len_scale = 0,

                .size_base = 0,
                .size_meta_size_offset = 0,
                .size_meta_size_scale = 0,
                .finalizer = NULL,
                .hash_code = NULL,
                .equals = NULL,
        },
        {
                .name = (object) "Pair", .tag = OBJ_PAIR,
                .getter = IMM_FALSE, .setter = IMM_FALSE, .to_string = IMM_FALSE,

                .member_base = object_offsetof(pair, car),
                .member_eq_len_base = 2,
                .member_len_base = 2,
                .member_meta_len_offset = 0,
                .member_meta_len_scale = 0,

                .size_base = object_sizeof_base(pair),
                .size_meta_size_offset = 0,
                .size_meta_size_scale = 0,
                .finalizer = NULL,
                .hash_code = NULL,
                .equals = NULL,
        },
        {
                .name = (object) "Bytes", .tag = OBJ_BYTES,
                .getter = IMM_FALSE, .setter = IMM_FALSE, .to_string = IMM_FALSE,

                .member_base = 0,
                .member_eq_len_base = 0,
                .member_len_base = 0,
                .member_meta_len_offset = 0,
                .member_meta_len_scale = 0,

                .size_base = object_sizeof_base(bytes),
                .size_meta_size_offset = object_offsetof(bytes, capacity),
                .size_meta_size_scale = sizeof(char),
                .finalizer = NULL,
                .hash_code = NULL,
                .equals = NULL,
        },
        {
                .name = (object) "String", .tag = OBJ_STRING,
                .getter = IMM_FALSE, .setter = IMM_FALSE, .to_string = IMM_FALSE,

                .member_base = 0,
                .member_eq_len_base = 0,
                .member_len_base = 0,
                .member_meta_len_offset = 0,
                .member_meta_len_scale = 0,

                .size_base = object_sizeof_base(string),
                .size_meta_size_offset = object_offsetof(string, len),
                .size_meta_size_scale = sizeof(char),
                .finalizer = NULL,
                .hash_code = string_hash_code,
                .equals = string_equals,
        },
        {
                .name = (object) "String-Buffer", .tag = OBJ_STRING_BUFFER,
                .getter = IMM_FALSE, .setter = IMM_FALSE, .to_string = IMM_FALSE,

                .member_base = object_offsetof(string_buffer, bytes_buffer),
                .member_eq_len_base = 1,
                .member_len_base = 1,
                .member_meta_len_offset = 0,
                .member_meta_len_scale = 0,

                .size_base = object_sizeof_base(string_buffer),
                .size_meta_size_offset = 0,
                .size_meta_size_scale = 0,
                .finalizer = NULL,
                .hash_code = NULL,
                .equals = NULL,
        },
        {
                .name = (object) "Symbol", .tag = OBJ_SYMBOL,
                .getter = IMM_FALSE, .setter = IMM_FALSE, .to_string = IMM_FALSE,

                .member_base = 0,
                .member_eq_len_base = 0,
                .member_len_base = 0,
                .member_meta_len_offset = 0,
                .member_meta_len_scale = 0,

                .size_base = object_sizeof_base(symbol),
                .size_meta_size_offset = object_offsetof(symbol, len),
                .size_meta_size_scale = sizeof(char),
                .finalizer = NULL,
                .hash_code = symbol_hash_code,
                .equals = symbol_equals,
        },
        {
                .name = (object) "Vector", .tag = OBJ_VECTOR,
                .getter = IMM_FALSE, .setter = IMM_FALSE, .to_string = IMM_FALSE,

                .member_base = object_offsetof(vector, data),
                .member_eq_len_base = 0,
                .member_len_base = 0,
                .member_meta_len_offset = object_offsetof(vector, len),
                .member_meta_len_scale = 1,

                .size_base = object_sizeof_base(vector),
                .size_meta_size_offset = object_offsetof(vector, len),
                .size_meta_size_scale = sizeof(object),
                .finalizer = NULL,
                .hash_code = NULL,
                .equals = NULL,
        },
        {
                .name = (object) "Stack", .tag = OBJ_STACK,
                .getter = IMM_FALSE, .setter = IMM_FALSE, .to_string = IMM_FALSE,

                .member_base = object_offsetof(stack, data),
                .member_eq_len_base = 0,
                .member_len_base = 0,
                .member_meta_len_offset = object_offsetof(stack, length),
                .member_meta_len_scale = 1,

                .size_base = object_sizeof_base(stack),
                .size_meta_size_offset = object_offsetof(stack, size),
                .size_meta_size_scale = sizeof(object),
                .finalizer = NULL,
                .hash_code = NULL,
                .equals = NULL,
        },
        {
                .name = (object) "String-Port", .tag = OBJ_STRING_PORT,
                .getter = IMM_FALSE, .setter = IMM_FALSE, .to_string = IMM_FALSE,

                .member_base = object_offsetof(string_port, string_buffer_data),
                .member_eq_len_base = 1,
                .member_len_base = 1,
                .member_meta_len_offset = 0,
                .member_meta_len_scale = 0,

                .size_base = object_sizeof_base(string_port),
                .size_meta_size_offset = 0,
                .size_meta_size_scale = 0,
                .finalizer = NULL,
                .hash_code = NULL,
                .equals = NULL,
        },
        {
                .name = (object) "Stdio-Port", .tag = OBJ_STDIO_PORT,
                .getter = IMM_FALSE, .setter = IMM_FALSE, .to_string = IMM_FALSE,

                .member_base = object_offsetof(stdio_port, filename),
                .member_eq_len_base = 0,
                .member_len_base = 0,
                .member_meta_len_offset = 0,
                .member_meta_len_scale = 0,

                .size_base = object_sizeof_base(stdio_port),
                .size_meta_size_offset = 0,
                .size_meta_size_scale = 0,
                .finalizer = stdio_finalizer,
                .hash_code = NULL,
                .equals = NULL,
        },
        {
                .name = (object) "Hash-Set", .tag = OBJ_HASH_SET,
                .getter = IMM_FALSE, .setter = IMM_FALSE, .to_string = IMM_FALSE,

                .member_base = object_offsetof(hashset, table),
                .member_eq_len_base = 1,
                .member_len_base = 1,
                .member_meta_len_offset = 0,
                .member_meta_len_scale = 0,

                .size_base = object_sizeof_base(hashset),
                .size_meta_size_offset = 0,
                .size_meta_size_scale = 0,
                .finalizer = NULL,
                .hash_code = NULL,
                .equals = NULL,
        },
        {
                .name = (object) "Hash-Map", .tag = OBJ_HASH_MAP,
                .getter = IMM_FALSE, .setter = IMM_FALSE, .to_string = IMM_FALSE,

                .member_base = object_offsetof(hashmap, table),
                .member_eq_len_base = 1,
                .member_len_base = 3,
                .member_meta_len_offset = 0,
                .member_meta_len_scale = 0,

                .size_base = object_sizeof_base(hashmap),
                .size_meta_size_offset = 0,
                .size_meta_size_scale = 0,
                .finalizer = NULL,
                .hash_code = NULL,
                .equals = NULL,
        },

        {
                .name = (object) "Weak-References", .tag = OBJ_WEAK_REF,
                .getter = IMM_FALSE, .setter = IMM_FALSE, .to_string = IMM_FALSE,

                .member_base = object_offsetof(weak_ref, ref),
                .member_eq_len_base = 1,
                .member_len_base = 1,
                .member_meta_len_offset = 0,
                .member_meta_len_scale = 0,

                .size_base = object_sizeof_base(weak_ref),
                .size_meta_size_offset = 0,
                .size_meta_size_scale = 0,
                .finalizer = NULL,
                .hash_code = NULL,
                .equals = NULL,
        },
};



/**
                               �����������Ľṹ
******************************************************************************/


/**
 * ��ʼ����׼������Ϣ
 * @param context
 */
static int context_init_globals_type_specs(REF context_t context) {
    context->global_type_table = (object_type_info_ptr) raw_alloc(
            sizeof(struct object_runtime_type_info_t) * OBJECT_TYPE_ENUM_MAX);
    context->global_type_table_len = 0;
    context->type_info_table_size = OBJECT_TYPE_ENUM_MAX;
    notnull_or_return(context->global_type_table, "context->global_type_table make failed.", 0);

    assert(OBJECT_TYPE_ENUM_MAX >= sizeof(scheme_type_specs) / sizeof(struct object_runtime_type_info_t));
    for (size_t i = 0; i < sizeof(scheme_type_specs) / sizeof(struct object_runtime_type_info_t); i++) {
        object_type_info_ptr type = &scheme_type_specs[i];

        assert(context->type_info_table_size >= (size_t) type->tag);

        context->global_type_table[type->tag].name = IMM_FALSE;
        context->global_type_table[type->tag].getter = type->getter;
        context->global_type_table[type->tag].setter = type->setter;
        context->global_type_table[type->tag].to_string = type->to_string;
        context->global_type_table[type->tag].tag = type->tag;

        context->global_type_table[type->tag].member_base = type->member_base;
        context->global_type_table[type->tag].member_eq_len_base = type->member_eq_len_base;
        context->global_type_table[type->tag].member_len_base = type->member_len_base;

        context->global_type_table[type->tag].member_meta_len_offset = type->member_meta_len_offset;
        context->global_type_table[type->tag].member_meta_len_scale = type->member_meta_len_scale;

        context->global_type_table[type->tag].size_base = type->size_base;
        context->global_type_table[type->tag].size_meta_size_offset = type->size_meta_size_offset;
        context->global_type_table[type->tag].size_meta_size_scale = type->size_meta_size_scale;

        context->global_type_table[type->tag].finalizer = type->finalizer;
        context->global_type_table[type->tag].hash_code = type->hash_code;
        context->global_type_table[type->tag].equals = type->equals;

        context->global_type_table_len++;
    }
    return 1;
}

/**
 * ��������ע��������Ϣ, ���ں�����չ
 * @param context ������
 * @param name ������
 * @param type_tag enum object_type_enum, �� object �ṹ����ƥ��, ��� 255
 * @param type_info ������Ϣ, ��Ҫ�ֶ������ڴ�
 * @return 0: ע��ʧ��
 */
EXPORT_API int
context_register_type(REF NOTNULL context_t context,
                      IN object_type_tag type_tag,
                      IN NOTNULL object_type_info_ptr type_info) {
    assert(!is_null(context));
    assert(!is_null(type_info));
    assert(type_tag >= 0);

    if (((size_t) type_tag) + 1 > context->type_info_table_size) {
        size_t new_size = (((size_t) type_tag) + 1);
        context->global_type_table = raw_realloc(context->global_type_table,
                                                 new_size * sizeof(struct object_runtime_type_info_t));
        if (context->global_type_table == NULL) return 0;
        size_t old_size = context->type_info_table_size;
        context->type_info_table_size = new_size;

        size_t empty_type_info_size = new_size - old_size;
        memset(&context->global_type_table[old_size], 0,
               empty_type_info_size * sizeof(struct object_runtime_type_info_t));
    }

    context->global_type_table[type_tag].name = NULL;
    context->global_type_table[type_tag].getter = type_info->getter;
    context->global_type_table[type_tag].setter = type_info->setter;
    context->global_type_table[type_tag].to_string = type_info->to_string;
    context->global_type_table[type_tag].tag = type_info->tag;

    context->global_type_table[type_tag].member_base = type_info->member_base;
    context->global_type_table[type_tag].member_eq_len_base = type_info->member_eq_len_base;
    context->global_type_table[type_tag].member_len_base = type_info->member_len_base;

    context->global_type_table[type_tag].member_meta_len_offset = type_info->member_meta_len_offset;
    context->global_type_table[type_tag].member_meta_len_scale = type_info->member_meta_len_scale;

    context->global_type_table[type_tag].size_base = type_info->size_base;
    context->global_type_table[type_tag].size_meta_size_offset = type_info->size_meta_size_offset;
    context->global_type_table[type_tag].size_meta_size_scale = type_info->size_meta_size_scale;

    context->global_type_table[type_tag].finalizer = type_info->finalizer;

    // ����ȫ�ֱ���
    if (context->global_type_table_len < (((size_t) type_tag) + 1)) {
        context->global_type_table_len = (((size_t) type_tag) + 1);
    }
    return 1;
}


/**
 * ����������, �������������������Ľṹ���е�
 * @param heap_init_size �ѳ�ʼ����, ������뵽 ALIGN_SIZE, �μ�
 * @param heap_growth_scale ��������
 * @param heap_max_size ���Ѵ�С
 * @return NULL: �ڴ����ʧ��
 */
EXPORT_API OUT NULLABLE context_t
context_make(IN size_t heap_init_size, IN size_t heap_growth_scale, IN size_t heap_max_size) {
    context_t context = raw_alloc(sizeof(struct scheme_context_t));
    //return;
    notnull_or_return(context, "context make failed.", NULL);

    context->heap = heap_make(heap_init_size, heap_growth_scale, heap_max_size);
    //return;
    notnull_or_return(context->heap, "context->heap make failed.", NULL);

    // ��ʼ�� gc ���
    context->gc_collect_on = 1;
    context->mark_stack_top = NULL;
    context->saves = NULL;

    context->in_port = stdin;
    context->out_port = stdout;
    context->err_out_port = stderr;


    context->_internal_scheme_type_specs = scheme_type_specs;
    // ��ʼ��������Ϣ����, ��ʱ�ѽṹ�Ѿ���������, ��������Ϣ��������
    // ��ʼ���������ṹʱӦ��������ʼ��
    if (context_init_globals_type_specs(context)) return context;
    else return NULL;
}

/**
 * �ͷ������Ľṹ
 * @param context
 */
EXPORT_API void context_destroy(IN NOTNULL context_t context) {
    // todo context �޸ĺ�, context_destroy ��ȫ
    if (context == NULL) {
        return;
    }

    // context_destroy ��Ӧ�������еĶ������ finalize
    for (heap_node_t node = context->heap->first_node; node != NULL; node = node->next) {
        for (char *ptr = node->data; ptr < node->free_ptr;) {
            object obj = (object) ptr;
            assert(is_object(obj));
            size_t size = context_object_sizeof(context, obj);

            // ִ�ж�����������
            proc_1 finalizer = context_get_object_finalize(context, obj);
            if (finalizer != NULL) {
                finalizer(context, obj);
            }

            ptr += size;
        }
    }

    // �ͷŶѽṹ
    if (context->global_type_table != NULL) {
        raw_free(context->global_type_table);
    }
    if (context->_internal_scheme_type_specs != NULL) {
        context->_internal_scheme_type_specs = NULL;
    }
    heap_destroy(context->heap);
    raw_free(context);
}

/**
                              ������������
******************************************************************************/
/**
 * stdio_port finalizer
 * @param context
 * @param port
 * @return
 */
object stdio_finalizer(context_t context, object port) {
    assert(is_object(port));
    assert(is_stdio_port(port));

    if (stdio_port_need_close(port) && !stdio_port_is_released(port)) {
        if (port->value.stdio_port.file != NULL) {
            fclose(port->value.stdio_port.file);
        }
        stdio_port_is_released(port) = 1;
        return IMM_TRUE;
    } else {
        return IMM_FALSE;
    }
}


/**
                               hash ֵ�㷨
******************************************************************************/
/**
 * symbol hash code ����
 * @param context
 * @param symbol
 * @return imm_i64, �Ǹ���
 */
EXPORT_API uint32_t symbol_hash_code(context_t context, object symbol) {
    assert(is_symbol(symbol));

    size_t length = symbol_len(symbol);
    const char *str = symbol_get_cstr(symbol);

    uint32_t hash = 0;
    uint32_t seed = 131;

    for (size_t i = 0; i < length; ++str, ++i) {
        hash = (hash * seed) + (*str);
    }

    return hash;
}

/**
 * string hash code ����
 * @param context
 * @param str
 * @return imm_i64, �Ǹ���
 */
EXPORT_API uint32_t string_hash_code(context_t context, object str_obj) {
    assert(is_string(str_obj));

    size_t length = string_len(str_obj);
    const char *str = string_get_cstr(str_obj);

    uint32_t hash = 0;
    uint32_t seed = 131;

    for (size_t i = 0; i < length; ++str, ++i) {
        hash = (hash * seed) + (*str);
    }

    return hash;
}

/**
                               equals ����
******************************************************************************/
/**
 * symbol �Ƚ�
 * @param context
 * @param symbol_a
 * @param symbol_b
 * @return 1: ���; 0: �����
 */
EXPORT_API int symbol_equals(context_t context, object symbol_a, object symbol_b) {
    if (!is_symbol(symbol_a) || !is_symbol(symbol_b)) {
        return 0;
    } else if (symbol_len(symbol_a) != symbol_len(symbol_b)) {
        return 0;
    } else {
        int cmp = memcmp(symbol_get_cstr(symbol_a), symbol_get_cstr(symbol_b), symbol_len(symbol_a) + 1);
        return (cmp == 0);
    }
}

/**
 * string �Ƚ�
 * @param context
 * @param str_a
 * @param str_b
 * @return IMM_TRUE / IMM_FALSE
 */
EXPORT_API int string_equals(context_t context, object str_a, object str_b) {
    if (!is_string(str_a) || !is_string(str_b)) {
        return 0;
    } else if (string_len(str_a) != string_len(str_b)) {
        return 0;
    } else {
        int cmp = memcmp(string_get_cstr(str_a), string_get_cstr(str_b), string_len(str_a) + 1);
        return (cmp == 0);
    }
}