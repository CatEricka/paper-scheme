#ifndef BASE_SCHEME_RUNTIME_H
#define BASE_SCHEME_RUNTIME_H
#pragma once


/**
 * runtime.h runtime.c
 * 包含解释器上下文结构定义和运行时支持, 以及各种值类型构造函数
 *
 * 测试内容见 test/test_cases/repl_test.h test/test_cases/value_test.h
 *
 */


#include <paper-scheme/opcodes.h>
#include <paper-scheme/gc.h>


/******************************************************************************
                                对象构造 API
******************************************************************************/

/**
 * 构造 i64 类型对象, 不建议直接使用, 因为要考虑到复杂的边界条件, 参见 i64_make_op()
 * @param heap
 * @param v i64 值
 * @return
 */
EXPORT_API OUT NOTNULL GC object i64_make_real_object_op(REF NOTNULL context_t context, IN int64_t v);

/**
 * 构造 i64 类型对象, 如果值范围属于 [- 2^(63-1), 2^(63-1)-1] 则构造立即数
 * @param heap
 * @param v i64 值
 * @return object 或立即数
 */
EXPORT_API OUT NOTNULL GC object i64_make_op(REF NOTNULL context_t context, IN int64_t v);

/**
 * 构造 doublenum 类型对象
 * @param context
 * @param v
 * @return
 */
EXPORT_API OUT NOTNULL GC object doublenum_make_op(REF NOTNULL context_t context, IN double v);

/**
 * 构造 pair 类型对象
 * @param context
 * @param v
 * @return
 */
EXPORT_API OUT NOTNULL GC object
pair_make_op(REF NOTNULL context_t context, REF NULLABLE object car, REF NULLABLE object cdr);

/**
 * 构造 symbol 类型对象
 * <p>symbol_len() 运算结果不包括 '\0', 但是 object->symbol.len 包括 '\0', 这是为了方便运行时计算对象大小</p>
 * @param context
 * @param cstr C字符串, '\0'结尾
 * @return
 */
EXPORT_API OUT NOTNULL GC object
symbol_make_from_cstr_op(REF NOTNULL context_t context, COPY char *cstr);

/**
 * 构造 bytes 对象
 * @param context
 * @param capacity bytes 容量, bytes_size * sizeof(char)
 * @return
 */
EXPORT_API OUT NOTNULL GC object
bytes_make_op(REF NOTNULL context_t context, IN size_t capacity);

/**
 * 构造 string 类型对象
 * <p>string_len() 运算结果不包括 '\0',
 * 但是 object->string.len 包括 '\0', 这是为了方便运行时计算对象大小</p>
 * @param context
 * @param cstr C字符串, '\0'结尾
 * @return
 */
EXPORT_API OUT NOTNULL GC object
string_make_from_cstr_op(REF NOTNULL context_t context, COPY char *cstr);

/**
 * 构造 string_buffer 对象
 * @param context
 * @param char_size char 容量, 实际 string_buffer 要多用一个 char 存储 '\0\
 * @return
 */
EXPORT_API OUT NOTNULL GC object
string_buffer_make_op(REF NOTNULL context_t context, IN size_t char_size);

/**
 * 从 string 构造 string_buffer 对象, 末尾包含 '\0'
 * @param context
 * @param str string, 深拷贝
 * @return
 */
EXPORT_API OUT NOTNULL GC object
string_buffer_make_from_string_op(REF NOTNULL context_t context, COPY object str);

/**
 * 构造 vector 类型对象, 填充 Unit, 即 '()
 * @param context
 * @param vector_len vector 大小
 * @return
 */
EXPORT_API OUT NOTNULL GC object
vector_make_op(REF NOTNULL context_t context, IN size_t vector_len);

/**
 * 构造 stack 类型对象, 填充 Unit.
 * @param context
 * @param stack_size
 * @return
 */
EXPORT_API OUT NOTNULL GC object
stack_make_op(REF NOTNULL context_t context, IN size_t stack_size);

/**
 * 从输入字符串打开 input port, 传入的 string 应当是不可变对象
 * <p>obj->value.string_port.length 长度包含 '\0'</p>
 * <p>(open-input-string "string here")</p>
 * @param context
 * @param str
 * @return 打开失败返回 IMM_UNIT
 */
EXPORT_API OUT NOTNULL GC object
string_port_input_from_string(REF NOTNULL context_t context, REF NULLABLE object str);

/**
 * 打开 output port, 具有内部缓冲
 * <p>(open-output-string)</p>
 * @param context
 * @return 打开失败返回 IMM_UNIT, 否则返回值满足 is_srfi6_port(obj),
 */
EXPORT_API OUT NOTNULL GC object
string_port_output_use_buffer(REF NOTNULL context_t context);

/**
 * 从输入字符串打开 input-output port, 深拷贝, 具有内部缓冲
 * <p>(open-input-output-string "string here")</p>
 * @param context
 * @param str
 * @return 打开失败返回 IMM_UNIT, 否则返回值满足 is_srfi6_port(obj), is_string_port_in_out_put(obj)
 */
EXPORT_API OUT NOTNULL GC object
string_port_in_out_put_from_string_use_buffer(REF NOTNULL context_t context, COPY NULLABLE object str);

/**
 * 从文件名打开 file port
 * @param context
 * @param filename
 * @param kind PORT_INPUT / PORT_OUTPUT / PORT_INPUT & PORT_OUTPUT
 * @return 打开失败返回 IMM_UNIT
 */
EXPORT_API OUT NOTNULL GC object
stdio_port_from_filename(REF NOTNULL context_t context, REF NULLABLE object filename, IN enum port_kind kind);

/**
 * 从 FILE * 打开 file port
 * @param context
 * @param file
 * @param kind PORT_INPUT / PORT_OUTPUT / PORT_INPUT & PORT_OUTPUT
 * @return 打开失败返回 IMM_UNIT
 */
EXPORT_API OUT NOTNULL GC object
stdio_port_from_file(REF NOTNULL context_t context, REF NOTNULL FILE *file, enum port_kind kind);

/**
 * 构造 hashset
 * @param context
 * @param init_capacity hashset 初始大小 (默认 DEFAULT_HASH_SET_MAP_INIT_init_CAPACITY)
 * @param load_factor 负载因子 (默认大小 DEFAULT_HASH_SET_MAP_LOAD_FACTOR)
 * @return
 */
EXPORT_API OUT NOTNULL GC object
hashset_make_op(REF NOTNULL context_t context, IN size_t init_capacity, IN double load_factor);

/**
 * 构造 hashmap
 * @param context
 * @param init_capacity hashmap 初始大小 (默认 DEFAULT_HASH_SET_MAP_INIT_init_CAPACITY)
 * @param load_factor 负载因子 (默认大小 DEFAULT_HASH_SET_MAP_LOAD_FACTOR)
 * @return
 */
EXPORT_API OUT NOTNULL GC object
hashmap_make_op(REF NOTNULL context_t context, IN size_t init_capacity, IN double load_factor);

/**
 * 构造 weak ref
 * @param context
 * @param obj
 * @return
 */
EXPORT_API OUT NOTNULL GC object
weak_ref_make_op(REF NOTNULL context_t context, REF NULLABLE object obj);

/******************************************************************************
                                对象操作 API
******************************************************************************/

/**
 * string 对象拼接, 深拷贝
 * @param context
 * @param string_a
 * @param string_b
 * @return 拼接后的 string
 */
EXPORT_API OUT NOTNULL GC object
string_append_op(REF NOTNULL context_t context, COPY NULLABLE object string_a, COPY NULLABLE object string_b);

/**
 * string_buffer 拼接 string, 深拷贝
 * @param context
 * @param str_buffer string_buffer
 * @param str string
 * @return 修改后的 string_buffer
 */
EXPORT_API OUT NOTNULL GC object
string_buffer_append_string_op(
        REF NOTNULL context_t context,
        IN NULLABLE object str_buffer, COPY NULLABLE object str);

/**
 * string_buffer 拼接 imm_char, 深拷贝
 * @param context
 * @param str_buffer
 * @param imm_char
 * @return
 */
EXPORT_API OUT NOTNULL GC object
string_buffer_append_imm_char_op(
        REF NOTNULL context_t context,
        IN NULLABLE object str_buffer, COPY NOTNULL object imm_char);

/**
 * string_buffer 拼接 char, 深拷贝
 * @param context
 * @param str_buffer
 * @param ch
 * @return
 */
EXPORT_API OUT NOTNULL GC object
string_buffer_append_char_op(REF NOTNULL context_t context, IN NULLABLE object str_buffer, COPY char ch);


/**
 * hashset 是否包含指定的对象
 * @param context
 * @param hashset
 * @param obj object 不能为 NULL
 * @return IMM_TRUE / IMM_FALSE
 */
EXPORT_API OUT NOTNULL GC object
hashset_contains_op(REF NOTNULL context_t context, REF NOTNULL object hashset, REF NOTNULL object obj);

/**
 * obj 放入 hashset
 * @param context
 * @param obj
 */
EXPORT_API void
hashset_put_op(REF NOTNULL context_t context, REF NOTNULL object hashset, REF NOTNULL object obj);

/**
 * hashset_b 全部放入 hashset_a, 浅拷贝
 * @param context
 * @param hashset_a 不能为空
 * @param hashset_b 不能为空
 */
EXPORT_API GC void
hashset_put_all_op(REF NOTNULL context_t context, REF NOTNULL object hashset_a, REF NOTNULL object hashset_b);

/**
 * 清空 hashset
 * @param context
 * @param hashset 不能为空
 * @return
 */
EXPORT_API GC void hashset_clear_op(REF NOTNULL context_t context, REF NOTNULL object hashset);

/**
 * 从 hashset 中移除 object
 * @param context
 * @param hashset
 * @param obj 不能为空, 可以为 IMM_UNIT
 */
EXPORT_API void
hashset_remove_op(REF NOTNULL context_t context, REF NOTNULL object hashset, REF NOTNULL object obj);


/**
 * hashmap 是否包含指定的对象
 * @param context
 * @param hashmap
 * @param key object 不能为 NULL
 * @return IMM_TRUE / IMM_FALSE
 */
EXPORT_API OUT NOTNULL GC object
hashmap_contains_key_op(REF NOTNULL context_t context, REF NOTNULL object hashmap, REF NOTNULL object key);

/**
 * obj 放入 hashmap
 * @param context
 * @param hashmap
 * @param k 键
 * @param v 值
 * @return 如果 k 已经存在, 则返回旧的 v, 否则返回 IMM_UNIT
 */
EXPORT_API OUT NOTNULL GC object
hashmap_put_op(REF NOTNULL context_t context, object hashmap, REF NOTNULL object k, REF NOTNULL object v);

/**
 * hashmap 取出 key 对应的 value
 * @param context
 * @param hashmap
 * @param key
 * @return 如果 key 存在, 则返回对应的 value, 否则返回 IMM_UNIT
 */
EXPORT_API OUT NOTNULL GC object
hashmap_get_op(REF NOTNULL context_t context, object hashmap, REF NOTNULL object key);

/**
 * hashmap_b 全部放入 hashmap_a, 浅拷贝
 * @param context
 * @param hashmap_a
 * @param hashmap_b
 */
EXPORT_API void
hashmap_put_all_op(REF NOTNULL context_t context, REF NOTNULL object hashmap_a, REF NOTNULL object hashmap_b);

/**
 * 清空 hashmap
 * @param context
 * @param hashmap
 */
EXPORT_API void hashmap_clear_op(REF NOTNULL context_t context, REF NOTNULL object hashmap);

/**
 * 从 hashmap 移除指定的 key
 * @param context
 * @param hashmap
 * @param key
 * @return 如果 key 已经存在, 返回被移除的 value, 否则返回 IMM_UNIT
 */
EXPORT_API OUT NOTNULL GC object
hashmap_remove_op(REF NOTNULL context_t context, REF NOTNULL object hashmap, REF NOTNULL object key);


/******************************************************************************
                                对象扩容 API
******************************************************************************/

/**
 * bytes 扩容, 深拷贝
 * @param context
 * @param bytes
 * @param add_size 增加的大小
 * @return
 */
EXPORT_API OUT NOTNULL GC object
bytes_capacity_increase(REF NOTNULL context_t context, IN object bytes, size_t add_size);

/**
 * string_buffer 扩容, 深拷贝
 * @param context
 * @param str_buffer
 * @param add_size 新增大小
 * @return
 */
EXPORT_API OUT NOTNULL GC object
string_buffer_capacity_increase(REF NOTNULL context_t context, IN object str_buffer, size_t add_size);

/**
 * vector 扩容, 深拷贝
 * @param context
 * @param vec
 * @param add_size
 * @return
 */
EXPORT_API OUT NOTNULL GC object
vector_capacity_increase(REF NOTNULL context_t context, IN object vec, size_t add_size);

/**
 * stack 扩容, 深拷贝
 * @param context
 * @param stack
 * @param add_size
 * @return
 */
EXPORT_API OUT NOTNULL GC object
stack_capacity_increase(REF NOTNULL context_t context, IN object stack, size_t add_size);

/**
 * stack 自动增长的入栈
 * <p>注意, push 后应当重新给原来的栈赋值</p>
 * @param context
 * @param stack
 * @param element
 * @param extern_growth_size 如果栈满, 会自动增长 extern_growth_size + 1 (填 0 则自动增长 1)
 * @return
 */
EXPORT_API OUT NOTNULL GC object
stack_push_auto_increase(REF NOTNULL context_t context, REF object stack, REF object element,
                         size_t extern_growth_size);


/******************************************************************************
                                类型转换 API
******************************************************************************/

/**
 * char 立即数 转 string
 * @param context
 * @param imm_char 立即数
 * @return string
 */
EXPORT_API OUT NOTNULL GC object
imm_char_to_string(REF NOTNULL context_t context, COPY object imm_char);

/**
 * char 转 string
 * @param context
 * @param ch
 * @return string
 */
EXPORT_API OUT NOTNULL GC object
char_to_string(REF NOTNULL context_t context, COPY char ch);

/**
 * string_buffer 转换为 string, 深拷贝
 * @param context
 * @param str_buffer string_buffer
 * @return string
 */
EXPORT_API OUT NOTNULL GC object
string_buffer_to_string(REF NOTNULL context_t context, COPY object str_buffer);


#endif //BASE_SCHEME_RUNTIME_H
