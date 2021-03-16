#include <paper-scheme/context.h>


/**
                                运行时类型信息
******************************************************************************/
/**
 * 基础类型定义
 */
struct object_runtime_type_info_t scheme_type_specs[] = {
        {
                .name = (object) "I64", .tag = OBJ_I64,
                .getter = IMM_FALSE, .setter = IMM_FALSE, .to_string = IMM_FALSE,
                .member_base = 0, .member_eq_len_base = 0, .member_len_base = 0,
                .member_meta_len_offset = 0, .member_meta_len_scale = 0,
                .size_base = 0, .size_meta_size_offset = 0, .size_meta_size_scale = 0,
                .finalizer = NULL
        },
        {
                .name = (object) "Doublenum", .tag = OBJ_D64,
                .getter = IMM_FALSE, .setter = IMM_FALSE, .to_string = IMM_FALSE,
                .member_base = 0, .member_eq_len_base = 0, .member_len_base = 0,
                .member_meta_len_offset = 0, .member_meta_len_scale = 0,
                .size_base = 0, .size_meta_size_offset = 0, .size_meta_size_scale = 0,
                .finalizer = NULL
        },
        {
                .name = (object) "Pair", .tag = OBJ_PAIR,
                .getter = IMM_FALSE, .setter = IMM_FALSE, .to_string = IMM_FALSE,
                .member_base = object_offsetof(pair, car), .member_eq_len_base = 2, .member_len_base = 2,
                .member_meta_len_offset = 0, .member_meta_len_scale = 0,
                .size_base = object_size(pair), .size_meta_size_offset = 0, .size_meta_size_scale = 0,
                .finalizer = NULL
        },
};



/**
                               解释器上下文结构
******************************************************************************/

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

    context->port_stdin = stdin;
    context->port_stdout = stdout;
    context->port_stderr = stderr;


    // 初始化类型信息

    return context;
}

/**
 * 初始化标准类型信息
 * @param context
 */
static int vm_init_globals_type_specs(REF context_t context) {
    // TODO 需要测试 注册类型信息, 初始大小设置为 OBJECT_TYPE_ENUM_MAX
    context->global_type_table = (object_type_info *) raw_alloc(
            sizeof(struct object_runtime_type_info_t) * OBJECT_TYPE_ENUM_MAX);
    context->type_info_len = 0;
    context->type_info_table_size = OBJECT_TYPE_ENUM_MAX;
    notnull_or_return(context->global_type_table, "context->global_type_table make failed.", 0);
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
                      IN NOTNULL object_type_info type_info) {
    assert(!is_null(context));
    assert(!is_null(type_info));
    assert(type_tag >= 0);

    if (((size_t) type_tag) + 1 > context->type_info_table_size) {
        context->global_type_table = raw_realloc(context->global_type_table, ((size_t) type_tag) + 1);
        if (context->global_type_table == NULL) return 0;
    }

    context->global_type_table[type_tag] = type_info;
    return 1;
}

/**
 * 释放上下文结构
 * @param context
 */
EXPORT_API void context_destroy(IN NOTNULL context_t context) {
    if (context == NULL) {
        return;
    }
    heap_destroy(context->heap);
    raw_free(context);
}
