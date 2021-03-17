#ifndef BASE_SCHEME_CONTEXT_H
#define BASE_SCHEME_CONTEXT_H
#pragma once


/**
 * context.h context.c
 * 上下文结构, 所有 scheme 解释器操作需要这个结构为基础
 * 测试内容见 test/test_cases/gc_test.h
 */


#include <paper-scheme/util.h>
#include <paper-scheme/object.h>
#include <paper-scheme/heap.h>


/**
                                上下文结构定义
******************************************************************************/
/**
 * 类型信息
 */
struct object_runtime_type_info_t;
typedef struct object_runtime_type_info_t *object_type_info;
/**
 * vm 上下文
 */
struct scheme_context_t;
typedef struct scheme_context_t {
    // 堆
    heap_t heap;

    FILE *port_stdin;
    FILE *port_stdout;
    FILE *port_stderr;

    // 全局类型信息表
    // TODO 完成全局表构造
    size_t type_info_len;
    size_t type_info_table_size;
    struct object_runtime_type_info_t *global_type_table;

    // 基础类型信息索引表, 仅用于构造期, 其中 object 类型均不可用; .name 字段为字符串指针
    struct object_runtime_type_info_t const *scheme_type_specs;
} *context_t;


/**
                                运行时类型信息
******************************************************************************/

typedef void (*proc_1)(context_t context, object arg1);

typedef void (*proc_2)(context_t context, object arg1, object arg2);

typedef void (*proc_3)(context_t context, object arg1, object arg2, object arg3);

typedef void (*proc_4)(context_t context, object arg1, object arg2, object arg3, object arg4);

typedef void (*proc_n)(context_t context, size_t argument_length, object args[]);

/**
 * 运行时类型信息
 */
struct object_runtime_type_info_t {
    // TODO 编写测试
    object name, getter, setter, to_string;
    object_type_tag tag;

    /** 用于标记对象引用的子对象, 类型必须是object, 且排列在一起 */
    size_t member_base;                  // 对象成员起始偏移量
    size_t member_eq_len_base;           // 比较对象时需要比较的 成员基本大小
    size_t member_len_base;              // 成员基本大小
    // 可变长度数据结构会用到这个属性, 如 vector, stack
    size_t member_meta_len_offset;       // 成员如果是可变长度, 则 描述成员数量的数量 在结构体的偏移量, 否则填 0
    size_t member_meta_len_scale;        // 成员数量的缩放系数, 一般为 1 或 0, 取决于成员数量是否可变
    /** 用于计算对象大小, 包含可变长度对象的信息 */
    size_t size_base;                   // 对象基本大小
    size_t size_meta_size_offset;       // 柔性数组长度属性偏移量
    size_t size_meta_size_scale;        // 柔性数组元素大小

    proc_1 finalizer;                   // finalizer
};


/**
                           运行时类型信息计算辅助宏
******************************************************************************/
// TODO 运行时类型信息计算辅助宏 编写测试
// object_type_info 结构体字段访问宏
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
 * @param _obj object
 * @return object_type_info
 */
#define context_get_object_type(_context, _obj) \
    (&((_context)->global_type_table[(_obj)->type]))

/**
 * 根据对象的类型信息计算对象的运行时大小, 需要完整的 context_t 结构, 需要正确初始化object->padding_size
 * @param _type object_type_info
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
 * @param _type object_type_info
 * @param _obj object
 * @param object *
 */
#define type_info_get_object_of_first_member(_type, _obj) \
    ((object *)((char*)(_obj) + type_info_member_base((_type))))

/**
 * 根据对象的类型信息取得 全部 对象成员字段个数
 * @param _type object_type_info
 * @param _obj object
 * @return size_t 成员字段个数
*/
#define object_type_info_member_slots_of(_type, _obj)                             \
  (type_info_get_size_by_size_field_offset((_obj), type_info_member_meta_len_offset((_type))) \
   * type_info_member_meta_len_scale((_type)) \
   + type_info_member_len_base((_type)))

/**
 * 根据对象的类型信息取得 compare 时需要比较的对象成员字段个数, 一般 <= object_type_info_member_slots_of
 * @param _type object_type_info
 * @param _obj object
 * @return size_t 成员字段个数
*/
#define object_type_info_member_eq_slots_of(_type, _obj)                          \
  (type_info_get_size_by_size_field_offset((_obj), type_info_member_meta_len_offset((_type))) \
   * type_info_member_meta_len_scale((_type)) \
   + type_info_member_eq_len_base((_type)))

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
 * 在上下文注册类型信息 TODO context_register_type 暂时无法使用和定义
 * @param context 上下文
 * @param type_tag enum object_type_enum, 与 object 结构体相匹配, 最大 255
 * @param type_info 类型信息, 需要手动分配内存
 * @return 0: 注册失败
 */
EXPORT_API int
context_register_type(REF NOTNULL context_t context,
                      IN object_type_tag type_tag,
                      IN NOTNULL object_type_info type_info);

/**
 * 释放上下文结构
 * @param context
 */
EXPORT_API void context_destroy(IN NOTNULL context_t context);


#endif //BASE_SCHEME_CONTEXT_H
