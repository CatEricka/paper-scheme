#include <paper-scheme/context.h>


/**
                                运行时类型信息
******************************************************************************/
/**
 * 基础类型定义
 * 这部分赋值在 context 中此时无法自举, 应当后期再处理, .name 定义应当仅用于备忘
 * todo  增加新类型重写 scheme_type_specs
 */
static struct object_runtime_type_info_t scheme_type_specs[OBJECT_TYPE_ENUM_MAX] = {
        {
                // 这部分赋值此时无法自举, 应当后期再处理, .name 定义应当仅用于备忘
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
                .hash_code = i64_hash_code,
                .equals = i64_equals,
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
                .hash_code = d64_hash_code,
                .equals = d64_equals,
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
                .hash_code = char_hash_code,
                .equals = char_equals,
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
                .hash_code = boolean_hash_code,
                .equals = boolean_equals,
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
                .hash_code = unit_hash_code,
                .equals = unit_equals,
        },
        {
                .name = (object) "Eof", .tag = OBJ_EOF,
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
                .hash_code = eof_hash_code,
                .equals = eof_equals,
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
                .hash_code = pair_hash_code,
                .equals = pair_equals,
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
                .hash_code = bytes_hash_code,
                .equals = bytes_equals,
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
                .hash_code = vector_hash_code,
                .equals = vector_equals,
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
                .hash_code = stack_hash_code,
                .equals = stack_equals,
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
                .hash_code = string_port_hash_code,
                .equals = string_port_equals,
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
                .hash_code = stdio_port_hash_code,
                .equals = stdio_port_equals,
        },
        {
                .name = (object) "Hash-Set", .tag = OBJ_HASH_SET,
                .getter = IMM_FALSE, .setter = IMM_FALSE, .to_string = IMM_FALSE,

                .member_base = object_offsetof(hashset, map),
                .member_eq_len_base = 1,
                .member_len_base = 1,
                .member_meta_len_offset = 0,
                .member_meta_len_scale = 0,

                .size_base = object_sizeof_base(hashset),
                .size_meta_size_offset = 0,
                .size_meta_size_scale = 0,
                .finalizer = NULL,
                .hash_code = hash_set_hash_code,
                .equals = hash_set_equals,
        },
        {
                .name = (object) "Hash-Map", .tag = OBJ_HASH_MAP,
                .getter = IMM_FALSE, .setter = IMM_FALSE, .to_string = IMM_FALSE,

                .member_base = object_offsetof(hashmap, table),
                .member_eq_len_base = 1,
                .member_len_base = 1,
                .member_meta_len_offset = 0,
                .member_meta_len_scale = 0,

                .size_base = object_sizeof_base(hashmap),
                .size_meta_size_offset = 0,
                .size_meta_size_scale = 0,
                .finalizer = NULL,
                .hash_code = hash_map_hash_code,
                .equals = hash_map_equals,
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
                .hash_code = weak_ref_hash_code,
                .equals = weak_ref_equals,
        },
};



/**
                               解释器上下文结构
******************************************************************************/


/**
 * 初始化标准类型信息
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
 * 在上下文注册类型信息, 用于后续拓展
 * @param context 上下文
 * @param name 类型名
 * @param type_tag enum object_type_enum, 与 object 结构体相匹配, 最大 255
 * @param type_info 类型信息, 需要手动分配内存
 * @return 0: 注册失败
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

    // 修正全局表长度
    if (context->global_type_table_len < (((size_t) type_tag) + 1)) {
        context->global_type_table_len = (((size_t) type_tag) + 1);
    }
    return 1;
}


/**
 * 构造上下文, 虚拟机操作是针对上下文结构进行的
 * @param heap_init_size 堆初始容量, 必须对齐到 ALIGN_SIZE, 参见
 * @param heap_growth_scale 增长倍率
 * @param heap_max_size 最大堆大小
 * @return NULL: 内存分配失败
 */
EXPORT_API OUT NULLABLE context_t
context_make(IN size_t heap_init_size, IN size_t heap_growth_scale, IN size_t heap_max_size) {
    context_t context = raw_alloc(sizeof(struct scheme_context_t));
    //return;
    notnull_or_return(context, "context make failed.", NULL);

    context->heap = heap_make(heap_init_size, heap_growth_scale, heap_max_size);
    //return;
    notnull_or_return(context->heap, "context->heap make failed.", NULL);

    // 初始化 gc 相关
    context->gc_collect_on = 1;
    context->mark_stack_top = NULL;
    context->saves = NULL;

    context->in_port = stdin;
    context->out_port = stdout;
    context->err_out_port = stderr;


    context->_internal_scheme_type_specs = scheme_type_specs;
    // 初始化类型信息结束, 此时堆结构已经正常工作, 但类型信息还不完整
    // 初始化解释器结构时应当完整初始化
    if (context_init_globals_type_specs(context)) return context;
    else return NULL;
}

/**
 * 释放上下文结构
 * @param context
 */
EXPORT_API void context_destroy(IN NOTNULL context_t context) {
    // todo context 修改后, context_destroy 补全
    if (context == NULL) {
        return;
    }

    // context_destroy 中应当对所有的对象进行 finalize
    for (heap_node_t node = context->heap->first_node; node != NULL; node = node->next) {
        for (char *ptr = node->data; ptr < node->free_ptr;) {
            object obj = (object) ptr;
            assert(is_object(obj));
            size_t size = context_object_sizeof(context, obj);

            // 执行对象析构方法
            proc_1 finalizer = context_get_object_finalize(context, obj);
            if (finalizer != NULL) {
                finalizer(context, obj);
            }

            ptr += size;
        }
    }

    // 释放堆结构
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
                              对象析构函数
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
                               hash 值算法
******************************************************************************/
EXPORT_API uint32_t hash_of_hash(uint32_t x, uint32_t y) {
    // https://stackoverflow.com/questions/17765494/commutative-hash-function-for-uint32-t-value-pairs
    const uint64_t a = (uint64_t) (x);
    const uint64_t b = (uint64_t) (y);

    const uint64_t h0 = (b << 32u) | a;
    const uint64_t h1 = (a << 32u) | b;

    return (x < y) ? h0 : h1; // conditional move (CMOV) instruction
}

EXPORT_API uint32_t i64_hash_code(context_t context, object i64) {
    uint32_t a = (uint64_t) i64_getvalue(i64) & 0xffffffff;
    uint32_t b = ((uint64_t) i64_getvalue(i64) >> 32u) & 0xffffffff;
    return hash_of_hash(a, b);
}

EXPORT_API uint32_t d64_hash_code(context_t context, object d64) {
    assert(sizeof
                   double == 8);
    double num = doublenum_getvalue(d64);

    uint64_t x = *((uint64_t *) &num);
    uint32_t a = (uint64_t) x & 0xffffffff;
    uint32_t b = ((uint64_t) x >> 32u) & 0xffffffff;

    return hash_of_hash(a, b);
}

EXPORT_API uint32_t char_hash_code(context_t context, object imm_char) {
    return 0;
}

EXPORT_API uint32_t boolean_hash_code(context_t context, object imm_bool) {
    return 0;
}

EXPORT_API uint32_t unit_hash_code(context_t context, object unit_obj) {
    return 0;
}

EXPORT_API uint32_t eof_hash_code(context_t context, object eof_obj) {
    return 0;
}

EXPORT_API uint32_t pair_hash_code(context_t context, object pair) {
    return 0;
}

EXPORT_API uint32_t bytes_hash_code(context_t context, object bytes) {
    return 0;
}

EXPORT_API uint32_t string_buffer_hash_code(context_t context, object string_buffer) {
    return 0;
}

EXPORT_API uint32_t vector_hash_code(context_t context, object vector) {
    return 0;
}

EXPORT_API uint32_t stack_hash_code(context_t context, object stack) {
    return 0;
}

EXPORT_API uint32_t string_port_hash_code(context_t context, object string_port) {
    return 0;
}

EXPORT_API uint32_t stdio_port_hash_code(context_t context, object stdio_port) {
    return 0;
}

EXPORT_API uint32_t hash_set_hash_code(context_t context, object hashset) {
    return 0;
}

EXPORT_API uint32_t hash_map_hash_code(context_t context, object hashmap) {
    return 0;
}

EXPORT_API uint32_t weak_ref_hash_code(context_t context, object weak_ref) {
    return 0;
}
/**
 * symbol hash code 计算
 * @param context
 * @param symbol
 * @return imm_i64, 非负数
 */
EXPORT_API uint32_t symbol_hash_code(context_t context, object symbol) {
    assert(is_symbol(symbol));

    size_t length = symbol_len(symbol);
    const char *str = symbol_get_cstr(symbol);

    uint32_t hash = 0;
    uint32_t seed = 131;

    // 添加类型标记
    // 大端序, 减小 0 的影响
    char tag[4] = {
            (((uint32_t) OBJ_SYMBOL) >> 24u) & 0xffu,
            (((uint32_t) OBJ_SYMBOL) >> 16u) & 0xffu,
            (((uint32_t) OBJ_SYMBOL) >> 8u) & 0xffu,
            (((uint32_t) OBJ_SYMBOL) >> 0u) & 0xffu,
    };
    for (size_t i = 0; i < 4; i++) {
        hash = (hash * seed) + tag[i];
    }

    for (size_t i = 0; i < length; ++str, ++i) {
        hash = (hash * seed) + (*str);
    }

    return hash;
}

/**
 * string hash code 计算
 * @param context
 * @param str
 * @return imm_i64, 非负数
 */
EXPORT_API uint32_t string_hash_code(context_t context, object str_obj) {
    assert(is_string(str_obj));

    size_t length = string_len(str_obj);
    const char *str = string_get_cstr(str_obj);

    uint32_t hash = 0;
    uint32_t seed = 131;

    // 添加类型标记
    // 大端序, 减小 0 的影响
    char tag[4] = {
            (((uint32_t) OBJ_STRING) >> 24u) & 0xffu,
            (((uint32_t) OBJ_STRING) >> 16u) & 0xffu,
            (((uint32_t) OBJ_STRING) >> 8u) & 0xffu,
            (((uint32_t) OBJ_STRING) >> 0u) & 0xffu,
    };
    for (size_t i = 0; i < 4; i++) {
        hash = (hash * seed) + tag[i];
    }

    for (size_t i = 0; i < length; ++str, ++i) {
        hash = (hash * seed) + (*str);
    }

    return hash;
}

/**
                               equals 函数
******************************************************************************/
EXPORT_API int i64_equals(context_t context, object i64_a, object i64_b) {
    if (!is_i64(i64_a) || !is_i64(i64_b)) {
        return 0;
    } else if (i64_a == i64_b) {
        return 1;
    } else {
        return i64_getvalue(i64_a) == i64_getvalue(i64_b);
    }
}

EXPORT_API int d64_equals(context_t context, object d64_a, object d64_b) {
    if (!is_doublenum(d64_a) || !is_doublenum(d64_b)) {
        return 0;
    } else if (d64_a == d64_b) {
        return 1;
    } else {
        return doublenum_getvalue(d64_a) == doublenum_getvalue(d64_b);
    }
}

EXPORT_API int char_equals(context_t context, object char_a, object char_b) {
    if (!is_imm_char(char_a) || !is_imm_char(char_b)) {
        return 0;
    } else {
        return char_a == char_b;
    }
}

EXPORT_API int boolean_equals(context_t context, object boolean_a, object boolean_b) {
    if (is_imm_true(boolean_a)) {
        return is_imm_true(boolean_b);
    } else if (is_imm_false(boolean_a)) {
        return is_imm_false(boolean_b);
    } else {
        return 0;
    }
}

EXPORT_API int unit_equals(context_t context, object unit_a, object unit_b) {
    return is_imm_unit(unit_a) && is_imm_unit(unit_b);
}

EXPORT_API int eof_equals(context_t context, object eof_a, object eof_b) {
    return is_imm_eof(eof_a) && is_imm_eof(eof_b);
}

EXPORT_API int pair_equals(context_t context, object pair_a, object pair_b) {
    if (!is_pair(pair_a) || !is_pair(pair_b)) {
        return 0;
    } else if (pair_a == pair_b) {
        return 0;
    } else {
        object pair_a_car = pair_car(pair_a);
        object pair_a_cdr = pair_cdr(pair_a);
        equals_fn equals_car = object_equals_helper(context, pair_a_car);
        equals_fn equals_cdr = object_equals_helper(context, pair_a_cdr);
        assert(equals_car != NULL);
        assert(equals_cdr != NULL);

        object pair_b_car = pair_car(pair_b);
        object pair_b_cdr = pair_cdr(pair_b);

        return equals_car(context, pair_a_car, pair_b_car) &&
               equals_cdr(context, pair_a_cdr, pair_b_cdr);
    }
}

EXPORT_API int bytes_equals(context_t context, object bytes_a, object bytes_b) {
    if (!is_bytes(bytes_a) || !is_bytes(bytes_b)) {
        return 0;
    } else if (bytes_a == bytes_b) {
        return 1;
    } else if (bytes_capacity(bytes_a) != bytes_capacity(bytes_b)) {
        // 容量不等
        return 0;
    } else {
        return 0 == memcmp(bytes_data(bytes_a), bytes_data(bytes_b), bytes_capacity(bytes_a));
    }
}

EXPORT_API int string_buffer_equals(context_t context, object string_buffer_a, object string_buffer_b) {
    if (!is_string_buffer(string_buffer_a) || !is_string_buffer(string_buffer_b)) {
        return 0;
    } else if (string_buffer_a == string_buffer_b) {
        return 1;
    } else if (string_buffer_length(string_buffer_a) != string_buffer_length(string_buffer_b)) {
        // 容量不等
        return 0;
    } else {
        return 0 == memcmp(string_buffer_bytes_data(string_buffer_a), string_buffer_bytes_data(string_buffer_b),
                           string_buffer_length(string_buffer_a));
    }
}

EXPORT_API int vector_equals(context_t context, object vector_a, object vector_b) {
    if (!is_vector(vector_a) || !is_vector(vector_b)) {
        return 0;
    } else if (vector_a == vector_b) {
        return 1;
    } else if (vector_len(vector_a) != vector_len(vector_b)) {
        return 0;
    } else {
        for (size_t i = 0; i < vector_len(vector_a); i++) {
            object a = vector_ref(vector_a, i);
            object b = vector_ref(vector_b, i);
            equals_fn equals = object_equals_helper(context, a);
            assert(equals != NULL);
            if (!equals(context, a, b)) {
                return 0;
            }
        }
        return 1;
    }
}

EXPORT_API int stack_equals(context_t context, object stack_a, object stack_b) {
    if (!is_stack(stack_a) || !is_stack(stack_b)) {
        return 0;
    } else if (stack_a == stack_b) {
        return 1;
    } else if (stack_len(stack_a) != stack_len(stack_b)) {
        return 0;
    } else {
        //TODO stack_equals 需要认真测试
        for (size_t i = 0; i < stack_len(stack_a); i++) {
            object a = stack_a->value.stack.data[i];
            object b = stack_b->value.stack.data[i];
            equals_fn equals = object_equals_helper(context, a);
            assert(equals != NULL);
            if (!equals(context, a, b)) {
                return 0;
            }
        }
        return 1;
    }
}

EXPORT_API int string_port_equals(context_t context, object string_port_a, object string_port_b) {
    if (!is_string_port(string_port_a) || !is_string_port(string_port_b)) {
        return 0;
    } else {
        // 考虑到流的特殊性, 只检查是否是同一个对象
        return string_port_a == string_port_b;
    }
}

EXPORT_API int stdio_port_equals(context_t context, object stdio_a, object stdio_b) {
    if (!is_stdio_port(stdio_a) || !is_stdio_port(stdio_b)) {
        return 0;
    } else {
        return stdio_a == stdio_b;
    }
}

EXPORT_API int hash_set_equals(context_t context, object hashset_a, object hashset_b) {
    if (!is_hashset(hashset_a) || !is_hashset(hashset_b)) {
        return 0;
    } else {
        return hashset_a == hashset_b;
    }
}

EXPORT_API int hash_map_equals(context_t context, object hashmap_a, object hashmap_b) {
    if (!is_hashmap(hashmap_a) || !is_hashmap(hashmap_b)) {
        return 0;
    } else {
        return hashmap_a == hashmap_b;
    }
}

EXPORT_API int weak_ref_equals(context_t context, object weak_ref_a, object weak_ref_b) {
    if (!is_weak_ref(weak_ref_a) || !is_weak_ref(weak_ref_b)) {
        return 0;
    } else if (!weak_ref_is_valid(weak_ref_a) && !weak_ref_is_valid(weak_ref_b)) {
        // 都是空的弱引用
        return 1;
    } else if (!weak_ref_is_valid(weak_ref_a) && weak_ref_is_valid(weak_ref_b)) {
        // weak_ref_a 无效, weak_ref_b 有效
        return 0;
    } else if (weak_ref_is_valid(weak_ref_a) && !weak_ref_is_valid(weak_ref_b)) {
        // weak_ref_a 有效, weak_ref_b 无效
        return 10;
    } else {
        // 二者都有效
        object ref_a = weak_ref_get(weak_ref_a);
        object ref_b = weak_ref_get(weak_ref_b);
        equals_fn equals = object_equals_helper(context, ref_a);
        assert(equals != NULL);
        return equals(context, ref_a, ref_b);
    }
}

/**
 * symbol 比较
 * @param context
 * @param symbol_a
 * @param symbol_b
 * @return 1: 相等; 0: 不相等
 */
EXPORT_API int symbol_equals(context_t context, object symbol_a, object symbol_b) {
    if (!is_symbol(symbol_a) || !is_symbol(symbol_b)) {
        return 0;
    } else if (symbol_a == symbol_b) {
        return 1;
    } else if (symbol_len(symbol_a) != symbol_len(symbol_b)) {
        return 0;
    } else {
        int cmp = memcmp(symbol_get_cstr(symbol_a), symbol_get_cstr(symbol_b), symbol_len(symbol_a) + 1);
        return (cmp == 0);
    }
}

/**
 * string 比较
 * @param context
 * @param str_a
 * @param str_b
 * @return IMM_TRUE / IMM_FALSE
 */
EXPORT_API int string_equals(context_t context, object str_a, object str_b) {
    if (!is_string(str_a) || !is_string(str_b)) {
        return 0;
    } else if (str_a == str_b) {
        return 1;
    } else if (string_len(str_a) != string_len(str_b)) {
        return 0;
    } else {
        int cmp = memcmp(string_get_cstr(str_a), string_get_cstr(str_b), string_len(str_a) + 1);
        return (cmp == 0);
    }
}


/**
                                    帮助函数
******************************************************************************/
/**
 * 从上下文中获取对象的类型信息
 * @param _context context_t
 * @param _obj object 不能为 NULL
 * @return object_type_info_ptr
 */
object_type_info_ptr context_get_object_type(NOTNULL context_t context, NOTNULL object obj) {
    assert(context != NULL);

    // todo 增加新类型重写运行时类型 context_get_object_type
    if (is_object(obj)) {
        return &context->global_type_table[(obj)->type];
    } else {
        if (is_null(obj)) {
            assert(0 && ("context_get_object_type: NULL object"));
            return NULL;
        } else if (is_imm_i64(obj)) {
            return &context->global_type_table[OBJ_I64];
        } else if (is_imm_char(obj)) {
            return &context->global_type_table[OBJ_CHAR];
        } else if (is_imm_true(obj)) {
            return &context->global_type_table[OBJ_BOOLEAN];
        } else if (is_imm_false(obj)) {
            return &context->global_type_table[OBJ_BOOLEAN];
        } else if (is_imm_unit(obj)) {
            return &context->global_type_table[OBJ_UNIT];
        } else if (is_imm_eof(obj)) {
            return &context->global_type_table[OBJ_EOF];
        } else {
            assert(0 && ("Unknown Type!"));
            return NULL;
        }
    }
}

/**
 * 从对象返回 hash_code 函数
 * @param context
 * @param obj
 * @return 如果不存在, 返回 NULL
 */
EXPORT_API hash_code_fn object_hash_helper(context_t context, object obj) {
    object_type_info_ptr t = context_get_object_type(context, obj);
    return type_info_hash_code(t);
}

/**
 * 从对象返回 equals 函数
 * @param context
 * @param obj
 * @return 如果不存在, 返回 NULL
 */
EXPORT_API equals_fn object_equals_helper(context_t context, object obj) {
    object_type_info_ptr t = context_get_object_type(context, obj);
    return type_info_equals(t);
}