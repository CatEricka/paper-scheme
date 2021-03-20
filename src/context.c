#include <paper-scheme/context.h>


/**
                                运行时类型信息
******************************************************************************/
/**
 * 基础类型定义
 * 这部分赋值在 context 中此时无法自举, 应当后期再处理, .name 定义应当仅用于备忘
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
                .finalizer = NULL
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
                .finalizer = NULL
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
                .finalizer = NULL
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
                .finalizer = NULL
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
                .finalizer = NULL
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
                .finalizer = NULL
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
                .finalizer = NULL
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
                .finalizer = NULL
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
                .finalizer = NULL
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

        context->global_type_table_len++;
    }
    return 1;
}

/**
 * 在上下文注册类型信息
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

    context->context_stdin = stdin;
    context->context_stdout = stdout;
    context->context_stderr = stderr;

    // 初始化类型信息, 此时需要堆结构正常工作, 但类型信息还不完整
    context->scheme_type_specs = scheme_type_specs;
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
    if (context->scheme_type_specs != NULL) {
        context->scheme_type_specs = NULL;
    }
    heap_destroy(context->heap);
    raw_free(context);
}
