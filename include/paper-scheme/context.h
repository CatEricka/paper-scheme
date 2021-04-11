#ifndef BASE_SCHEME_CONTEXT_H
#define BASE_SCHEME_CONTEXT_H
#pragma once


/**
 * context.h context.c
 * 上下文结构, 所有 scheme 解释器操作需要这个结构为基础
 * 测试内容见 test/test_cases/gc_test.h
 */


#include <paper-scheme/heap.h>


/**
                                上下文结构定义
******************************************************************************/

/**
 * 用来在 gc 前保存临时变量, 移动对象后会正确更新索引
 */
typedef struct gc_illusory_dream {
    object *illusory_object;
    struct gc_illusory_dream *next;
} *gc_saves_list_t;

/**
 * gc 时遍历图结构用的标记栈
 */
typedef struct gc_mark_stack_node_t {
    object *start, *end;
    struct gc_mark_stack_node_t *prev;
} *gc_mark_stack_ptr;

/**
 * 类型信息
 */
struct object_runtime_type_info_t;
typedef struct object_runtime_type_info_t *object_type_info_ptr;

/**
 * 上下文
 */
struct scheme_context_t;
typedef struct scheme_context_t *context_t;

/**
 * op_exec_ 函数指针
 */
typedef object (*op_exec_dispatch_func)(context_t r, enum opcode_e e);

/**
 * opcodes info
 */
typedef struct op_code_info_t {
    enum opcode_e op;
    op_exec_dispatch_func func;
    object name;
    size_t min_args_length;
    size_t max_args_length;
    char *args_type_table;
} op_code_info;

/**
 * 上下文结构定义
 */
struct scheme_context_t {
    /**
     *                      开关选项
     ****************************************************/
    // 调试信息 开关
    int debug;
    // 是否处于 repl 模式
    int repl_mode;
    // 解释器环境是否初始化结束
    int init_done;


    /**
     *                  op_exec 分发表
     ****************************************************/
    op_code_info *dispatch_table;


    /**
     *                    解释器寄存器
     ****************************************************/
    // TODO 解释器寄存器
    /**
     * 参数寄存器, pair / IMM_UNIT
     */
    GC object args;
    /**
     * s-exp 寄存器, pair / IMM_UNIT
     */
    GC object code;
    /**
     * 当前 environment 指针
     * <p>只有最顶端的全局 env 是 hashmap, </p>
     * <p>env: (list ... env_frame2 env_frame1 global_environment '())</p>
     * <p>env_frame: (list ... slot3 slot2 slot1)</p>
     * <p>因为后面的常常很短, 构造 hashmap 的代价过高</p>
     * <p>参见 env_stack 类型</p>
     */
    GC object current_env;
    /**
     * 栈记录, 链栈, 为了实现`延续`
     */
    GC object scheme_stack;

    /**
     * 当前操作码
     */
    int op_code;
    /**
     * 返回值, any type
     */
    GC object value;

    /**
     * (load "") 栈, (make-stack MAX_LOAD_FILE_DEEP)
     */
    GC object load_stack;


    /**
     *                   全局信息表
     ****************************************************/
    /**
     * 全局符号表 弱引用 hashset
     * <p>(make-weak-hashset GLOBAL_SYMBOL_TABLE_INIT_SIZE)</p>
     * <p>symbol</p>
     */
    GC object global_symbol_table;
    /**
     * 全局 environment, (make-hashmap)
     * symbol->any
     */
    GC object global_environment;
    /**
     * 用于 gensym 生成唯一 symbol
     */
    uint64_t gensym_count;

    /**
     * GC 全局类型信息表
     */
    GC struct object_runtime_type_info_t *global_type_table;
    // 全局类型信息表最大长度
    size_t type_info_table_size;
    // 全局类型信息表当前长度
    size_t global_type_table_len;


    /**
     *                      堆结构
     ****************************************************/
    // 堆
    heap_t heap;
    // 抑制 gc 时 gc_collect() 函数功能, 仅用于测试
    int gc_collect_on;
    // GC! gc 时的临时变量保护链
    GC gc_saves_list_t saves;
    // 当发生过 GC 时, 该标志被设置为 1, 用于调试于断言
    int gc_happened;
    // 弱引用链, 用于 GC 时扫描对象后标记弱引用
    GC WEAK_REF object weak_ref_chain;
    // gc 标记栈
    struct gc_mark_stack_node_t mark_stack[GC_MAX_MARK_STACK_DEEP];
    // 标记栈顶
    gc_mark_stack_ptr mark_stack_top;

    // TODO 修改为内部 port 实现, 决定构造时机; 解释器输入输出
    GC object in_port;
    GC object out_port;
    GC object err_out_port;


    /**
     *                   公共信息引用
     ****************************************************/
    // 基础类型信息索引表, 仅用于构造期, 其中 object 类型均不可用; .name 字段为字符串指针
    struct object_runtime_type_info_t const *_internal_scheme_type_specs;


    /**
     *                      C 交互
     ****************************************************/
    int ret;
};


/**
                                运行时类型信息
******************************************************************************/

typedef object (*proc_1)(context_t context, object arg1);

typedef object (*proc_2)(context_t context, object arg1, object arg2);

typedef object (*proc_3)(context_t context, object arg1, object arg2, object arg3);

typedef object (*proc_4)(context_t context, object arg1, object arg2, object arg3, object arg4);

typedef object (*proc_n)(context_t context, size_t argument_length, object args[]);

typedef uint32_t (*hash_code_fn)(context_t context, object arg);

typedef int (*equals_fn)(context_t context, object arg1, object arg2);

/**
 * 运行时类型信息
 */
struct object_runtime_type_info_t {
    object name, getter, setter, to_string;
    object_type_tag tag;

    /** 用于标记对象引用的子对象, 类型必须是object, 且排列在一起 */
    size_t member_base;                 // 对象成员起始偏移量
    size_t member_eq_len_base;          // 比较对象时需要比较的 成员基本数量
    size_t member_len_base;             // 成员基本数量
    // 可变长度数据结构会用到这个属性, 如 vector, stack
    size_t member_meta_len_offset;      // 成员如果是可变长度, 则 描述成员数量的数量 在结构体的偏移量, 否则填 0
    size_t member_meta_len_scale;       // 成员数量的缩放系数, 一般为 1 或 0, 取决于成员数量是否可变
    /** 用于计算对象大小, 包含可变长度对象的信息 */
    size_t size_base;                   // 对象基本大小
    size_t size_meta_size_offset;       // 柔性数组长度属性偏移量
    size_t size_meta_size_scale;        // 柔性数组元素大小

    proc_1 finalizer;                   // finalizer
    hash_code_fn hash_code;             // hash值计算      (context, object) -> uint32_t
    equals_fn equals;                   // 比较是否相等     (context, object) -> int 0/1
};


/**
                              对象析构函数
******************************************************************************/
/**
 * stdio_port finalizer
 * @param context
 * @param port
 * @return
 */
object stdio_finalizer(context_t _context, object port);


/**
                               hash 值算法
******************************************************************************/
// 辅助函数
EXPORT_API uint32_t uint32_pair_hash(uint32_t x, uint32_t y);

EXPORT_API uint32_t uint64_hash(uint64_t value);

EXPORT_API uint32_t double_number_hash(double num);
// https://www.partow.net/programming/hashfunctions/#BKDRHashFunction
EXPORT_API uint32_t symbol_hash_helper(object symbol);

EXPORT_API uint32_t string_hash_helper(object str_obj);
/**
 * 根据对象地址和对象的类型枚举生成 hash 值, 保证 hash_code_fn 与 equals 匹配
 * @param ptr
 * @param enum
 * @return
 */
EXPORT_API uint32_t pointer_with_type_to_hash(object ptr, enum object_type_enum type_enum);

/**
                              获取 hash 值
                       todo 增加新类型重写 hash 算法
******************************************************************************/
EXPORT_API uint32_t i64_hash_code(context_t context, object i64);
EXPORT_API uint32_t d64_hash_code(context_t context, object d64);
EXPORT_API uint32_t char_hash_code(context_t context, object imm_char);
EXPORT_API uint32_t boolean_hash_code(context_t context, object imm_bool);
EXPORT_API uint32_t unit_hash_code(context_t context, object unit_obj);
EXPORT_API uint32_t eof_hash_code(context_t context, object eof_obj);
EXPORT_API uint32_t pair_hash_code(context_t context, object pair);

EXPORT_API uint32_t bytes_hash_code(context_t context, object bytes);

EXPORT_API uint32_t string_buffer_hash_code(context_t context, object string_buffer);

EXPORT_API uint32_t vector_hash_code(context_t context, object vector);

EXPORT_API uint32_t stack_hash_code(context_t context, object stack);

EXPORT_API uint32_t string_port_hash_code(context_t context, object string_port);

EXPORT_API uint32_t stdio_port_hash_code(context_t context, object stdio_port);

EXPORT_API uint32_t hash_set_hash_code(context_t context, object hashset);

EXPORT_API uint32_t hash_map_hash_code(context_t context, object hashmap);

EXPORT_API uint32_t weak_ref_hash_code(context_t context, object weak_ref);

EXPORT_API uint32_t weak_hashset_hash_code(context_t context, object weak_hashset);

EXPORT_API uint32_t stack_frame_hash_code(context_t context, object frame);

EXPORT_API uint32_t env_slot_hash_code(context_t context, object slot);


/**
 * symbol hash code 计算
 * <p>https://www.partow.net/programming/hashfunctions/#BKDRHashFunction</p>
 * @param context
 * @param symbol
 * @return imm_i64, 非负数
 */
EXPORT_API uint32_t symbol_hash_code(context_t _context, object symbol);

/**
 * string hash code 计算
 * <p>https://www.partow.net/programming/hashfunctions/#BKDRHashFunction</p>
 * @param context
 * @param str
 * @return imm_i64, 非负数
 */
EXPORT_API uint32_t string_hash_code(context_t _context, object str);

/**
                          equals 函数 (仅用于 hash 计算)
                          todo 增加新类型重写 equals 算法
******************************************************************************/
EXPORT_API int i64_equals(context_t context, object i64_a, object i64_b);
EXPORT_API int d64_equals(context_t context, object d64_a, object d64_b);
EXPORT_API int char_equals(context_t context, object char_a, object char_b);
EXPORT_API int boolean_equals(context_t context, object boolean_a, object boolean_b);
EXPORT_API int unit_equals(context_t context, object unit_a, object unit_b);
EXPORT_API int eof_equals(context_t context, object eof_a, object eof_b);
// 注意, 遇到自引用结构会出现问题, 递归比较可能无法终止
EXPORT_API int pair_equals(context_t context, object pair_a, object pair_b);
EXPORT_API int bytes_equals(context_t context, object bytes_a, object bytes_b);
EXPORT_API int string_buffer_equals(context_t context, object string_buffer_a, object string_buffer_b);
// 注意, 遇到自引用结构会出现问题, 递归比较可能无法终止
EXPORT_API int vector_equals(context_t context, object vector_a, object vector_b);
// 注意, 遇到自引用结构会出现问题, 递归比较可能无法终止
EXPORT_API int stack_equals(context_t context, object stack_a, object stack_b);

EXPORT_API int string_port_equals(context_t context, object string_port_a, object string_port_b);

EXPORT_API int stdio_port_equals(context_t context, object stdio_a, object stdio_b);

EXPORT_API int hash_set_equals(context_t context, object hashset_a, object hashset_b);

EXPORT_API int hash_map_equals(context_t context, object hashmap_a, object hashmap_b);

EXPORT_API int weak_ref_equals(context_t context, object weak_ref_a, object weak_ref_b);

EXPORT_API int weak_hashset_equals(context_t context, object weak_hashset_a, object weak_hashset_b);

EXPORT_API int stack_frame_equals(context_t context, object stack_a, object stack_b);

EXPORT_API int env_slot_equals(context_t context, object slot_a, object slot_b);

/**
 * symbol 比较
 * @param context
 * @param symbol_a
 * @param symbol_b
 * @return 1: 相等; 0: 不相等
 */
EXPORT_API int symbol_equals(context_t _context, object symbol_a, object symbol_b);
/**
 * string 比较
 * @param context
 * @param str_a
 * @param str_b
 * @return 1: 相等; 0: 不相等
 */
EXPORT_API int string_equals(context_t _context, object str_a, object str_b);

/**
                           运行时类型信息计算辅助宏
******************************************************************************/
// object_type_info_ptr 结构体字段访问宏
#define type_info_field(_t, _f) ((_t)->_f)

// 对象成员起始偏移量
#define type_info_member_base(_t)               type_info_field((_t), member_base)

// 比较对象时需要比较的 成员基本大小
#define type_info_member_eq_len_base(_t)        type_info_field((_t), member_eq_len_base)
// 成员基本大小
#define type_info_member_len_base(_t)           type_info_field((_t), member_len_base)
// 成员如果是可变长度, 则 描述成员数量的数量 在结构体的偏移量, 否则填 0
#define type_info_member_meta_len_offset(_t)    type_info_field((_t), member_meta_len_offset)
// 成员数量的缩放系数, 一般为 1 或 0, 取决于成员数量是否可变
#define type_info_member_meta_len_scale(_t)     type_info_field((_t), member_meta_len_scale)

// 对象基本大小
#define type_info_size_base(_t)                 type_info_field((_t), size_base)
// 柔性数组长度属性偏移量
#define type_info_size_meta_size_offset(_t)     type_info_field((_t), size_meta_size_offset)
// 柔性数组元素大小
#define type_info_size_meta_size_scale(_t)      type_info_field((_t), size_meta_size_scale)

// finalize
#define type_info_finalizer(t)                  type_info_field((t), finalizer)
// hash_code
#define type_info_hash_code(t)                  type_info_field((t), hash_code)
// equals
#define type_info_equals(t)                     type_info_field((t), equals)


/**
 * 根据对象的 size_t 类型字段偏移量读取 size_t 值
 * @param _obj object 对象
 * @param _offset size_t 字段偏移量
 */
#define type_info_get_size_by_size_field_offset(_obj, _offset) \
    (((size_t*)((char*)(_obj) + (_offset)))[0])

/**
 * 从上下文中获取对象的类型信息
 * @param _context context_t
 * @param _obj object 不能为 NULL
 * @return object_type_info_ptr
 */
object_type_info_ptr context_get_object_type(NOTNULL context_t context, NOTNULL object obj);

#define context_get_object_finalize(context, obj) \
    type_info_finalizer(context_get_object_type((context), (obj)))

/**
 * 根据对象的类型信息计算对象的运行时大小, 需要完整的 context_t 结构, 需要正确初始化object->padding_size
 * @param _type object_type_info_ptr
 * @param _obj object
 */
#define object_type_info_sizeof(_type, _obj) \
  (type_info_get_size_by_size_field_offset((_obj), type_info_size_meta_size_offset((_type))) \
   * type_info_size_meta_size_scale((_type)) \
   + type_info_size_base((_type)) \
   + (_obj)->padding_size)

/**
 * 从上下文, 使用 context_t->global_type_table 信息获得对象大小
 * @param _context context_t
 * @param _obj object
*/
#define context_object_sizeof(_context, _obj) \
    object_type_info_sizeof(context_get_object_type((_context), (_obj)), (_obj))

/**
 * 根据对象的类型信息取得对象的第一个成员字段引用, 用于 gc 图遍历入栈
 * <p>注意, 返回值是 object *, 而不是 object</p>
 * @param _type object_type_info_ptr
 * @param _obj object
 * @param object *
 */
#define type_info_get_object_ptr_of_first_member(_type, _obj) \
    ((object *)((char*)(_obj) + type_info_member_base((_type))))

/**
 * 根据对象的类型信息取得 全部 对象成员字段个数
 * @param _type object_type_info_ptr
 * @param _obj object
 * @return size_t 成员字段个数
*/
#define object_type_info_member_slots_of(_type, _obj)                             \
  (type_info_get_size_by_size_field_offset((_obj), type_info_member_meta_len_offset((_type))) \
   * type_info_member_meta_len_scale((_type)) \
   + type_info_member_len_base((_type)))

/**
 * 根据对象的类型信息取得 compare 时需要比较的对象成员字段个数, 一般 <= object_type_info_member_slots_of
 * @param _type object_type_info_ptr
 * @param _obj object
 * @return size_t 成员字段个数
*/
#define object_type_info_member_eq_slots_of(_type, _obj)                          \
  (type_info_get_size_by_size_field_offset((_obj), type_info_member_meta_len_offset((_type))) \
   * type_info_member_meta_len_scale((_type)) \
   + type_info_member_eq_len_base((_type)))


/**
                                    帮助函数
******************************************************************************/

/**
 * 从对象返回 hash_code 函数
 * @param context
 * @param obj
 * @return 如果不存在, 返回 NULL
 */
EXPORT_API hash_code_fn object_hash_helper(context_t context, object obj);

/**
 * 从对象返回 equals 函数
 * @param context
 * @param obj
 * @return 如果不存在, 返回 NULL
 */
EXPORT_API equals_fn object_equals_helper(context_t context, object obj);


/**
                               解释器上下文结构
******************************************************************************/

/**
 * 构造上下文, 虚拟机操作是针对上下文结构进行的
 * @return NULL: 内存分配失败
 */
EXPORT_API OUT NULLABLE context_t
context_make(IN size_t heap_init_size, IN size_t heap_growth_scale, IN size_t heap_max_size);

/**
 * 在上下文注册类型信息, 用于后续拓展
 * @param context 上下文
 * @param type_tag enum object_type_enum, 与 object 结构体相匹配, 最大 255
 * @param type_info 类型信息, 需要手动分配内存
 * @return 0: 注册失败
 */
EXPORT_API int
context_register_type(REF NOTNULL context_t context,
                      IN object_type_tag type_tag,
                      IN NOTNULL object_type_info_ptr type_info);

/**
 * 释放上下文结构
 * @param context
 */
EXPORT_API void context_destroy(IN NOTNULL context_t context);


#endif //BASE_SCHEME_CONTEXT_H
