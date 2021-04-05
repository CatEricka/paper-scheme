#ifndef BASE_SCHEME_OBJECT_H
#define BASE_SCHEME_OBJECT_H
#pragma once


/**
 * object.h object.c
 * 对象结构定义
 * 测试内容见 test/test_cases/object_test.h test/test_cases/vm_test.h test/test_cases/value_test.h
 */


#include <paper-scheme/feature.h>


/**
                               对象类型标记
******************************************************************************/
// todo  增加新类型重写 object_type_enum 枚举
enum object_type_enum {
    OBJ_I64 = 0,
    OBJ_D64,
    OBJ_CHAR,
    OBJ_BOOLEAN,
    OBJ_UNIT,
    OBJ_EOF,
    OBJ_PAIR,
    OBJ_BYTES,
    OBJ_STRING,
    OBJ_STRING_BUFFER,
    OBJ_SYMBOL,
    OBJ_VECTOR,
    OBJ_STACK,
    OBJ_STRING_PORT,
    OBJ_STDIO_PORT,
    OBJ_HASH_SET,
    OBJ_HASH_MAP,
    OBJ_WEAK_REF,

    OBJECT_TYPE_ENUM_MAX, // 标记枚举最大值
};
typedef enum object_type_enum object_type_tag;
//compile_time_assert(((size_t) OBJECT_TYPE_ENUM_MAX) <= SIZE_MAX);

// 对象头魔数, uint8_t, B1010 1010
#define OBJECT_HEADER_MAGIC (0xAAu)
/**
                               对象内存布局
******************************************************************************/

/**
 * 内存对齐掩码
 * B0111 = 7
 * 对齐到 8 字节整倍数
 */
#ifdef IS_32_BIT_ARCH
compile_time_assert(sizeof(uintptr_t) == 4u);
compile_time_assert(sizeof(void *) == 4u);
compile_time_assert(sizeof(void *) == sizeof(uint32_t));
compile_time_assert(sizeof(double) == 8u);
compile_time_assert(sizeof(double) == sizeof(uint64_t));
// 32位时, 指针和内存分配大小对齐到 8字节, B0111 = 7
# define ALIGN_MASK (7u)
// 指针和内存分配大小的最低 3 bit 必须为 0
# define ALIGN_BITS (3u)
// 对齐到 8u bytes
# define ALIGN_SIZE (8u)
#elif IS_64_BIT_ARCH
compile_time_assert(sizeof(uintptr_t) == 8u);
compile_time_assert(sizeof(void *) == 8u);
compile_time_assert(sizeof(void *) == sizeof(uint64_t));
compile_time_assert(sizeof(double) == 8u);
compile_time_assert(sizeof(double) == sizeof(uint64_t));
// 64位时, 指针和内存分配大小对齐到 8字节, B0111 = 7
# define ALIGN_MASK (7u)
// 指针和内存分配大小的最低 3 bit 必须为 0
# define ALIGN_BITS (3u)
// 对齐到 8u bytes 的整倍数
# define ALIGN_SIZE (8u)
#else
# error("Unknown arch")
#endif

/**
 * 内存对齐检查
 */
#define assert_aligned_ptr_check(x) assert(((((uintptr_t) (x)) & ((uintptr_t) ALIGN_MASK)) == 0u))
#define assert_aligned_size_check(x) assert(((((size_t) (x)) & ((uintptr_t) ALIGN_MASK)) == 0u))


/**
                                基础对象结构声明
******************************************************************************/
// port 类型
enum port_kind {
    // 具有内部缓冲 https://srfi.schemers.org/srfi-6/srfi-6.html
            PORT_SRFI6 = 1,
    // 输入 port
            PORT_INPUT = 2,
    // 输出 port
            PORT_OUTPUT = 4,
    // port 到达结尾
            PORT_EOF = 8,
};

struct object_struct_t;
typedef struct object_struct_t *object;


struct object_struct_t {
    /*  对象头  */
    // 对象头魔数
    uint8_t magic;
    // gc状态, 1为存活
    uint8_t marked: 1;
    // 是否为不可变对象
    uint8_t immutable: 1;
    // 对象尾部对齐填充大小
    uint8_t padding_size;
    // 对象类型
    object_type_tag type;
    // 移动对象用转发地址
    object forwarding;

    union object_value_u {
        /*  基本对象  */
        //定点64位有符号整数, 不可变
        int64_t i64;

        //浮点数, 不可变
        double doublenum;

        //pair, 可变
        struct value_pair_t {
            object car;
            object cdr;
        } pair;

        // bytes, 可变
        struct value_bytes_t {
            // 用于 hash 计算, 固定不变
            uint32_t hash;
            size_t capacity;
            char data[0];
        } bytes;

        //字符串, 不可变
        struct value_string_t {
            // 用于 hash 计算, 固定不变
            uint32_t hash;
            // char data[] 大小, 注意是指 char 个数, 包括 '\0'
            size_t len;
            char data[0];
        } string;

        // 字符串缓冲, 可变
        struct value_string_buffer_t {
            // 用于 hash 计算, 固定不变
            uint32_t hash;
            // 缓冲总长度
            size_t buffer_size;
            // 当前长度, 包括 '\0'
            size_t buffer_length;
            object bytes_buffer;
        } string_buffer;

        //symbol, 不可变
        struct value_symbol_t {
            // 用于 hash 计算, 固定不变
            uint32_t hash;
            // char data[] 大小, 注意是指 char 个数
            size_t len;
            char data[0];
        } symbol;

        //向量, 可变
        struct value_vector_t {
            // 用于 hash 计算, 固定不变
            uint32_t hash;
            // object data[0] 大小, 注意是指 object 个数, 不是字节长度!
            size_t len;
            object data[0];
        } vector;

        //栈, 可变
        struct value_stack_t {
            // 用于 hash 计算, 固定不变
            uint32_t hash;
            // 栈大小
            size_t size;
            // 栈内 object 数量, 空栈为 0
            size_t length;
            object data[0];
        } stack;

        // 字符串类型输入输出, 可变
        struct value_string_port_t {
            // 用于 hash 计算, 构造后固定不变
            uint32_t hash;
            enum port_kind kind;
            object string_buffer_data;
            size_t length;
            size_t current;
        } string_port;

        // stdio 类型输入输出, 可变
        struct value_stdio_port_t {
            // 用于 hash 计算, 固定不变
            uint32_t hash;
            enum port_kind kind;
            FILE *file;
            object filename;
            // 是否已经释放
            int is_released;
            // 是否需要关闭, stdin, stdout, stderr 不需要关闭
            int need_close;
        } stdio_port;

        // 实现 hash set, 可变
        // 最好只放入原子类型, 无法保证复合类型正常工作
        struct value_hashset_t {
            // 用于 hash 计算, 固定不变
            uint32_t hash;
            // hashmap 实现
            object map;
        } hashset;

        // hash map, 可变
        // key 最好为原子类型
        struct value_hashmap_t {
            // 用于 hash 计算, 固定不变
            uint32_t hash;
            // 当前 hashmap 键值对数量
            size_t size;
            double load_factor;
            // 扩容上限
            size_t threshold;
            // #( ((k v) '())
            //    ((k v) '())
            //    ((k v) '()))
            // vector
            object table;
        } hashmap;

        /*  运行时结构  */
        // 弱引用, 可变
        // 注意, 立即数无法被检查是否引用, 因此引用立即数的弱引用是不可靠的
        struct value_weak_ref_t {
            // 用于 hash 计算, 固定不变
            uint32_t hash;
            // GC 时使用的内部结构
            object _internal_next_ref;
            // 弱引用
            object ref;
        } weak_ref;

    } value;
    /*  对齐填充, 对齐到 ALIGN_SIZE, 即 sizeof(void *)  */
};



/**
                               立即数标记定义
    `*` 标记为已经实现


     bits end in
             *         1:  i64 number
                      00:  pointer
             * 0000 0010:  保留
             * 0000 0110:  保留
             * 0000 1110:  保留
             * 0001 1110:  char, 30u
             * 0011 1110:  unique immediate (IMM_UNIT, IMM_TRUE, IMM_FALSE 或其它), 62u

    对于 object, 有三种可能类型:
        - 真实的 object:
        - 立即数:
        - NULL:

    对于 NULL:
        判别:
            - NULL:             is_null(obj)
    对于立即数:
        判别:
            - IMM_UNIT:         is_imm_unit(obj)
            - IMM_TRUE:         is_imm_true(obj)
            - IMM_FALSE:        is_imm_false(obj)
            - IMM_EOF:          is_imm_eof(obj)
            - all unit:         is_imm(obj)
            - i64:              is_imm_i64(obj), 不推荐直接使用
            - char:             is_imm_char(obj)
        立即数构造方法:
            - unique:           make_unique_immediate(), 该方法不应当被直接使用
            - i64:              无, 参见 i64_make_real_object_op() 和 i64_make_op()
            - char:             char_imm_make()
        立即数取值方法:
            - unique:           无, 直接比较即可
            - i64:              无, 参见 i64_getvalue()
            - char:             char_getvalue()

    真实的 object:
        判别:
            - i64:              is_i64(obj)
            - double number:    is_doublenum(obj
            - pair:             is_pair(obj)
            - bytes:            is_bytes(obj)
            - string:           is_string(obj)
            - string_buffer:    is_string_buffer(obj)
            - symbol:           is_symbol(obj)
            - vector:           is_vector(obj)
            - stack:            is_stack(obj)
            - string_port:      TODO is_string_port(obj), is_string_port_input(), is_string_port_output(),
                                TODO is_string_port_in_out_put(obj), is_string_port_eof(obj)
            - stdio_port:       TODO is_stdio_port(obj), is_stdio_port_input(obj), is_stdio_port_output(obj),
                                TODO is_stdio_port_in_out_put(obj), is_stdio_port_eof(obj)
            - string_port & stdio_port:
                                TODO is_port_input(obj), is_port_output(obj), is_port_in_out_put(obj), is_port_eof(obj)
            - srfi6 string_port:
                                TODO is_srfi6_port(obj)
            - hashset:          is_hashset(obj)
            - hashmap:          is_hashmap(obj)
            - weak_ref:         is_weak_ref(obj)
        构造:
            - i64:              i64_make_op()
            - double number:    doublenum_make_op()
            - pair:             pair_make_op()
            - bytes:            bytes_make_op()
            - string:           string_make_from_cstr_op()
            - string_buffer:    string_buffer_make_op(), string_buffer_make_from_string_op()
            - symbol:           symbol_make_from_cstr_op()
            - vector:           vector_make_op()
            - stack:            stack_make_op()
            - string_port:      TODO string_port_input_from_string(), string_port_output_use_buffer(),
                                TODO string_port_in_out_put_from_string_use_buffer()
            - stdio_port:       TODO stdio_port_from_filename(), stdio_port_from_file()
            - hashset:          TODO hashset_make_op()
            - hashmap:          TODO hashmap_make_op()
            - weak_ref:         TODO weak_ref_make_op()
        取值:
            - i64:              i64_getvalue()
            - double number:    doublenum_getvalue()
            - pair:             pair_getcar(), pair_getcdr()
            - bytes:            bytes_capacity(), bytes_index(), bytes_data()
            - string:           string_get_cstr(), string_len(), string_index()
            - string_buffer:    string_buffer_empty(), string_buffer_full(),
                                string_buffer_length(), string_buffer_index(), string_buffer_bytes_data(),
                                string_buffer_capacity(), string_buffer_bytes_obj
            - symbol:           symbol_make_get_cstr(), symbol_len(), symbol_index()
            - vector:           vector_len(), vector_ref()
            - stack:            stack_clean(), stack_capacity(), stack_len()
                                stack_full(), stack_empty()
                                stack_push, stack_pop(),  stack_peek(),
            - string_port:      TODO string_port_kind()
            - stdio_port:       TODO stdio_port_kind()
            - hashset:          hashset_size()
            - hashmap:          hashmap_size()
            - weak_ref:         weak_ref_is_valid()
        操作:
            - string:           string_append_op()
            - string_buffer:    string_buffer_append_string_op(), string_buffer_append_imm_char_op(),
                                string_buffer_append_char_op()
            - hashset:          TODO hashset_contains_op(), hashset_put_op(), hashset_put_all_op()
                                TODO hashset_clear_op(), hashset_remove_op()
            - hashmap:          TODO hashmap_contains_key_op(), hashmap_put_op(), hashmap_get_op()
                                TODO hashmap_put_all_op(), hashmap_clear_op(), hashmap_remove_op()
            - weak_ref:         weak_ref_get()
        扩容:
            - bytes:            bytes_capacity_increase()
            - string_buffer:    string_buffer_capacity_increase()
            - vector:           vector_capacity_increase()
            - stack:            stack_capacity_increase(),  stack_push_auto_increase()
        类型转换:
            - char (立即数):     imm_char_to_string()
            - char (C 原始类型): char_to_string()
            - symbol:           TODO symbol_to_string()
            - string:           TODO string_to_symbol()
            - string_buffer:    string_buffer_to_string(), TODO string_buffer_to_symbol()
        哈希算法 & equals 算法:
            - i64:
            - double number:
            - pair:
            - bytes:
            - string:           string_hash_code(), string_equals()
            - string_buffer:
            - symbol:           symbol_hash_code(), symbol_equals()
            - vector:
            - stack:
            - string_port:
            - stdio_port:
            - string_port & stdio_port:
            - srfi6 string_port:
            - hashset:
            - hashmap:
            - weak_ref:
******************************************************************************/


/**
                               普通立即数标记
******************************************************************************/
#define USE_IMMEDIATE

// i64 立即数最低1位舍弃
#define I64_EXTENDED_BITS (1u)
// i64 立即数掩码 B01
#define I64_IMM_MASK (1u)
// i64 立即数标记 B01
#define I64_IMM_TAG (1u)

#ifdef IS_64_BIT_ARCH
// i64 立即数最小值: - 2^(63-1)
# define I64_IMM_MIN (-4611686018427387904)
// i64 立即数最大值:   2^(63-1)-1
# define I64_IMM_MAX (4611686018427387903)
#elif IS_32_BIT_ARCH
// i64 立即数最小值: - 2^(31-1)
# define I64_IMM_MIN (-1073741824i32)
// i64 立即数最大值:   2^(31-1)-1
# define I64_IMM_MAX (1073741823i32)
#else
# error("Unknown arch")
#endif

// 指针掩码 B0011
#define POINTER_MASK (3u)
#define POINTER_TAG (0u)

// 无符号 char (8bits), 立即数最低 8 位舍弃
#define CHAR_EXTENDED_BITS (8u)
// char 立即数掩码 B1111 1111
#define CHAR_IMM_MASK (255u)
// char 立即数标记 B0001 1110
#define CHAR_IMM_TAG (30u)
// char 值域 MASK
#define CHAR_VALUE_MASK (255u)



/**
                               常量立即数标记
******************************************************************************/
/**
 * 最低 8 位用于常量立即数标记
 */
#define UNIQUE_IMMEDIATE_EXTENDED_BITS (8u)
/**
 * 常量立即数标记掩码 B1111 1111
 */
#define UNIQUE_IMMEDIATE_MASK (255u)
/**
 * 常量立即数标记 B0011 1110
 */
#define UNIQUE_IMMEDIATE_TAG (62u)
/**
 * 常量立即数生成
 */
#define make_unique_immediate(n) \
    ((object) (((n)<<UNIQUE_IMMEDIATE_EXTENDED_BITS) | UNIQUE_IMMEDIATE_TAG))


/**
                               常量立即数定义
******************************************************************************/
/**
 * 基本类型 false
 */
#define IMM_FALSE make_unique_immediate(0u)
/**
 * 基本类型 true
 */
#define IMM_TRUE make_unique_immediate(1u)
/**
 * 基本类型 Unit, 等价于 '()
 * <p>对于内部结构, 应当统一使用 NULL; 但 List 应当以 Unit 结束而不是 NULL; 空 vector 应当以 Unit 填充</p>
 */
#define IMM_UNIT make_unique_immediate(2u)
/**
 * EOF 符号
 */
#define IMM_EOF make_unique_immediate(3u)

/**
                               立即数构造
******************************************************************************/

/**
 * 生成 i64 立即数
 * @param object
 */
#define i64_imm_make(x) ((object) ((ptr_to_uintptr(x) << I64_EXTENDED_BITS) | I64_IMM_TAG))
/**
 * 构造 char 对象/立即数, 参数为 char, 注意类型
 * @param object
 */
#define char_imm_make(x) ((object) (((ptr_to_uintptr(x) & CHAR_VALUE_MASK) << CHAR_EXTENDED_BITS) | CHAR_IMM_TAG))



/**
                               对象操作 API
******************************************************************************/

/**
 * 计算对象大小
 * @param value_field object->value->value_field
 * @return 对象大小
 */
#define object_sizeof_base(value_field)\
    (offsetof(struct object_struct_t, value) + sizeof(((object)0)->value.value_field))
/**
* 计算对象结构体成员所在偏移量
* @param value_field object->value->value_field
* @return 对象大小
*/
#define object_offsetof(_value, _value_field) \
    ((size_t)&(((object)0)->value._value._value_field))
/**
 * 计算对齐的大小
 * 用法: aligned_size(object_sizeof_base(value_field))
 * 给定一个大小, 返回对齐到 ALIGN_SIZE 的大小
 * @param unaligned_size
 * @return
 */
EXPORT_API OUT size_t aligned_size(IN size_t unaligned_size);
/**
 * 计算对象头大小
 * @return 对象头大小
 */
#define object_sizeof_header() (offsetof(struct object_struct_t, value))
/**
 * 运行时计算对象大小
 * <p>todo 仅供 context_t->global_type_table 建立前使用</p>
 * <p>详见 context.h: struct object_runtime_type_info_t, macro object_type_info_sizeof()</p>
 * @param object NOTNULL 不能是立即数或空指针
 * @param object
 */
EXPORT_API OUT OUT size_t object_bootstrap_sizeof(REF NOTNULL object obj);


/**
                           对象值操作: is_a
******************************************************************************/
/**
 * 检查是否为特殊立即数, 注意 NULL 不是特殊立即数
 * @param object
 */
#define is_unique_imm(x) \
    ((ptr_to_uintptr(x) & UNIQUE_IMMEDIATE_MASK) == UNIQUE_IMMEDIATE_TAG)
/**
 * 检查是否为 i64 立即数
 * @param object
 */
#define is_imm_i64(x)                   ((ptr_to_uintptr(x) & I64_IMM_MASK) == I64_IMM_TAG)
/**
 * 检查是否为对象指针
 * <p>这意味着如果表达式成立, 则它是个有效的 object 指针</p>
 * @param object
 */
#define is_object(obj) ((!is_null(obj)) \
    && ((ptr_to_uintptr(obj) & POINTER_MASK) == POINTER_TAG) \
    && (((object) (obj))->magic == OBJECT_HEADER_MAGIC))
/**
 * 检查是否被垃圾回收标记存活
 */
#define is_marked(obj)                  ((obj)->marked)
/**
 * 检查是否为 char 立即数, 参数必须为 object
 * @param object
 */
#define is_imm_char(x)                  ((ptr_to_uintptr(x) & CHAR_IMM_MASK) == CHAR_IMM_TAG)
/**
 * 检查是否为立即数
 * @param object
 */
#define is_imm(x)                       (!is_null(x) && ((ptr_to_uintptr(x) & POINTER_MASK) != POINTER_TAG))

// 是否为 IMM_UNIT
#define is_imm_unit(obj)                ((obj) == IMM_UNIT)
// 是否为 IMM_TRUE
#define is_imm_true(obj)                ((obj) == IMM_TRUE)
// 是否为 IMM_FALSE
#define is_imm_false(obj)               ((obj) == IMM_FALSE)
// 是否为 IMM_EOF
#define is_imm_eof(obj)                 ((obj) == IMM_EOF)
/**
 * 判断 object 是否为 i64, 可能是立即数, 参见 is_i64_real()
 * @param i64 NULLABLE
 * @return !0 -> true, 0 -> false
 */
EXPORT_API OUT int is_i64(REF NULLABLE object i64);
/**
 * 判断 object 是否为真正的 i64 对象, 而不是一个立即数
 * @param i64
 * @return !0 -> true, 0 -> false
 */
#define is_i64_real(object)             (is_object(object) && ((object)->type == OBJ_I64))
/**
 * 判断 object 是否为 doublenum
 * @param object NULLABLE
 * @return !0 -> true, 0 -> false
 */
#define is_doublenum(object)            (is_object(object) && ((object)->type == OBJ_D64))
/**
 * 判断 object 是否为 pair
 * @param object NULLABLE
 * @return !0 -> true, 0 -> false
 */
#define is_pair(object)                 (is_object(object) && ((object)->type == OBJ_PAIR))
// bytes
#define is_bytes(obj)                   (is_object(obj) && ((obj)->type == OBJ_BYTES))
/**
 * 判断 object 是否为 string
 * @param object NULLABLE
 * @return !0 -> true, 0 -> false
 */
#define is_string(object)               (is_object(object) && ((object)->type == OBJ_STRING))
// string buffer
#define is_string_buffer(obj)           (is_object(obj) && ((obj)->type == OBJ_STRING_BUFFER))
/**
 * 判断 object 是否为 symbol
 * @param object NULLABLE
 * @return !0 -> true, 0 -> false
 */
#define is_symbol(object)               (is_object(object) && ((object)->type == OBJ_SYMBOL))
/**
 * 判断 object 是否为 vector
 * @param object NULLABLE
 * @return !0 -> true, 0 -> false
 */
#define is_vector(object)               (is_object(object) && ((object)->type == OBJ_VECTOR))
// stack
#define is_stack(object)                (is_object(object) && ((object)->type == OBJ_STACK))

//port
#define is_port(obj)                    (is_object(obj) && (((obj)->type == OBJ_STRING_PORT) || ((obj)->type == OBJ_STDIO_PORT)))

// string_port
#define string_port_kind(obj)           ((obj)->value.string_port.kind)
#define is_string_port(obj)             (is_port(obj) && ((obj)->type == OBJ_STRING_PORT))
#define is_string_port_input(obj)       (is_port(obj) && ((unsigned)string_port_kind(obj) & (unsigned)PORT_INPUT))
#define is_string_port_output(obj)      (is_port(obj) && ((unsigned)string_port_kind(obj) & (unsigned)PORT_OUTPUT))
#define is_string_port_in_out_put(obj)  (is_port(obj) && ((unsigned)string_port_kind(obj) & ((unsigned)PORT_INPUT | (unsigned)PORT_OUTPUT)))
#define is_string_port_eof(obj)         (is_port(obj) && ((unsigned)string_port_kind(obj) & (unsigned)PORT_EOF))

// stdio_port
#define stdio_port_kind(obj)            ((obj)->value.stdio_port.kind)
#define is_stdio_port(obj)              (is_port(obj) && ((obj)->type == OBJ_STDIO_PORT))
#define is_stdio_port_input(obj)        (is_port(obj) && ((unsigned)stdio_port_kind(obj) & (unsigned)PORT_INPUT))
#define is_stdio_port_output(obj)       (is_port(obj) && ((unsigned)stdio_port_kind(obj) & (unsigned)PORT_OUTPUT))
#define is_stdio_port_in_out_put(obj)   (is_port(obj) && ((unsigned)stdio_port_kind(obj) & ((unsigned)PORT_INPUT | (unsigned)PORT_OUTPUT)))
#define is_stdio_port_eof(obj)          (is_port(obj) && ((unsigned)stdio_port_kind(obj) & (unsigned)PORT_EOF))

// srfi6
#define is_srfi6_port(obj)              ((is_string_port(obj)) && ((unsigned)string_port_kind(obj) & (unsigned)PORT_SRFI6))

// port
#define is_port_input(obj)              (is_string_port(obj) ? is_string_port_input(obj) : is_stdio_port_input(obj))
#define is_port_output(obj)             (is_string_port(obj) ? is_string_port_output(obj) : is_stdio_port_output(obj))
#define is_port_in_out_put(obj)         (is_string_port(obj) ? is_string_port_in_out_put(obj) : is_stdio_port_in_out_put(obj))

#define is_port_eof(obj)                (is_string_port(obj) ? is_string_port_eof(obj) : is_stdio_port_eof(obj))

// hash set
#define is_hashset(obj)                 (is_object(obj) && (obj)->type == OBJ_HASH_SET)
// hash map
#define is_hashmap(obj)                 (is_object(obj) && (obj)->type == OBJ_HASH_MAP)

// 弱引用
#define is_weak_ref(obj)                (is_object(obj) && (obj)->type == OBJ_WEAK_REF)
/**
                                对象值操作
******************************************************************************/
/**
 * char 立即数取值, 参数必须为 object, 返回值为 char
 * @param object
 */
#define char_imm_getvalue(x) \
    ((char) ((ptr_to_uintptr(x) >> CHAR_EXTENDED_BITS) & CHAR_VALUE_MASK))

/**
 * 获取 i64 对象的值
 * @param i64: object
 * @return
 */
EXPORT_API OUT int64_t i64_getvalue(REF NOTNULL object i64);

/**
 * 获取 doublenum 对象的值
 * @param object
 * @return
 */
#define doublenum_getvalue(obj) ((obj)->value.doublenum)

/**
 * 获取 pair 对象的 car
 * @param object
 * @return
 */
#define pair_car(obj) ((obj)->value.pair.car)

/**
 * 获取 pair 对象的 cdr
 * @param object
 * @return
 */
#define pair_cdr(obj) ((obj)->value.pair.cdr)

#define pair_caar(x)        (pair_car(pair_car(x)))
#define pair_cadr(x)        (pair_car(pair_cdr(x)))
#define pair_cdar(x)        (pair_cdr(pair_car(x)))
#define pair_cddr(x)        (pair_cdr(pair_cdr(x)))
#define pair_caaar(x)       (pair_car(pair_caar(x)))
#define pair_caadr(x)       (pair_car(pair_cadr(x)))
#define pair_cadar(x)       (pair_car(pair_cdar(x)))
#define pair_caddr(x)       (pair_car(pair_cddr(x)))
#define pair_cdaar(x)       (pair_cdr(pair_caar(x)))
#define pair_cdadr(x)       (pair_cdr(pair_cadr(x)))
#define pair_cddar(x)       (pair_cdr(pair_cdar(x)))
#define pair_cdddr(x)       (pair_cdr(pair_cddr(x)))
#define pair_cadddr(x)      (pair_cadr(pair_cddr(x)))

/**
 * bytes 计算总大小
 */
#define bytes_capacity(obj)     ((obj)->value.bytes.capacity)

/**
 * bytes 索引
 */
#define bytes_index(obj, i)     ((obj)->value.bytes.data[(i)])

/**
 * get bytes' data
 * @return
 */
#define bytes_data(obj)         ((obj)->value.bytes.data)

/**
 * 获取 string 对象的 cstr
 * @param object
 * @return
 */
#define string_get_cstr(obj) ((obj)->value.string.data)

/**
 * 获取 string 对象的 char 个数, 注意不包括 '\0'
 * @param object
 * @return
 */
#define string_len(obj)     ((obj)->value.string.len - 1u)

/**
 * 使用索引访问 string 的 cstr 的对应字符, 范围 [ 0, symbol_len(obj) )
 * @param object
 * @param i: 索引值
 * @return char 引用
 */
#define string_index(obj, i) ((obj)->value.string.data[(i)])

/**
 * 检查 string buffer 是否为空
 */
#define string_buffer_empty(obj)    ((obj)->value.string_buffer.buffer_length == 0)

/**
 * string buffer 字符容量
 */
#define string_buffer_capacity(obj)     ((obj)->value.string_buffer.buffer_size)

/**
 * string buffer 当前长度
 */
#define string_buffer_length(obj)   ((obj)->value.string_buffer.buffer_length)

/**
 * string buffer 是否已满
 */
#define string_buffer_full(obj)     (string_buffer_capacity(obj) == string_buffer_length(obj))

/**
 * string buffer 访问内容
 */
#define string_buffer_index(obj, i)    (bytes_index((obj)->value.string_buffer.bytes_buffer, (i)))

/**
 * 获取 string_buffer 内部字节数组
 * @return
 */
#define string_buffer_bytes_data(obj)    (bytes_data((obj)->value.string_buffer.bytes_buffer))

/**
 * 获取 string_buffer 内部 bytes 对象
 * @return
 */
#define string_buffer_bytes_obj(obj)    ((obj)->value.string_buffer.bytes_buffer)


/**
 * 获取 symbol 对象的 cstr
 * @param object
 * @return
 */
#define symbol_get_cstr(obj) ((obj)->value.symbol.data)

/**
 * 获取 symbol 对象的 char 个数, 注意不包括 '\0'
 * @param object
 * @return
 */
#define symbol_len(obj) ((obj)->value.symbol.len - 1u)

/**
 * 使用索引访问 symbol 的 cstr 的对应字符, 范围 [0, len)
 * @param object
 * @param i: 索引值
 * @return 引用
 */
#define symbol_index(obj, i) ((obj)->value.symbol.data[(i)])

/**
 * 获取 vector 对象的容量
 * @param object
 * @return
 */
#define vector_len(obj) ((obj)->value.vector.len)

/**
 * 使用索引访问 vector 的内容, 范围 [0, len)
 * @param object
 * @param i: 索引值
 * @return object 引用
 */
#define vector_ref(obj, i) ((obj)->value.vector.data[(i)])

/**
 * 使用索引修改 vector 的内容, 范围 [0, len)
 * @param object
 * @param i: 索引值
 * @return 不要使用返回值
 */
#define vector_set(obj, i, v) ((obj)->value.vector.data[(i)] = (v))

/**
 * 检查栈是否为空
 * @param obj object
 * @return int 0 => 空; !0 => 非空
 */
#define stack_empty(obj) (((obj)->value.stack.length) == 0)

/**
 * 检查栈是否为满
 * @param obj object
 * @return int 0 => 满; !0 => 非满
 */
#define stack_full(obj) (((obj)->value.stack.length) == ((obj)->value.stack.size))

/**
 * 清空栈
 */
#define stack_clean(obj) ((obj)->value.stack.length = 0)

/**
 * 栈容量
 */
#define stack_capacity(obj) ((obj)->value.stack.size)

/**
 * 返回栈长度, 等于栈中元素数量
 */
#define stack_len(obj) ((obj)->value.stack.length)

/**
 * 返回栈顶元素
 * @param stack
 * @return object 如果栈为空, 返回 NULL
 */
NULLABLE CHECKED REF object stack_peek(object stack);

/**
 * 入栈
 * @param stack
 * @param obj
 * @return 如果栈满, 返回 0, 否则返回 1
 */
NULLABLE OUT int stack_push(REF object stack, REF object obj);

/**
 * 出栈
 * @param stack
 * @return 如果栈空, 返回 0; 否则返回 1
 */
CHECKED OUT int stack_pop(REF object stack);

/**
 * port 是否需要关闭 (stdin stdout stderr 不需要关闭)
 */
#define stdio_port_need_close(obj) ((obj)->value.stdio_port.need_close)

/**
 * port 是否已经释放
 */
#define stdio_port_is_released(obj) ((obj)->value.stdio_port.is_released)

/**
 * hashset 元素数量
 */
#define hashset_size(obj)   ((obj)->value.hashset.size)

/**
 *  hashmap 元素数量
 */
#define hashmap_size(obj)   ((obj)->value.hashmap.size)

/**
 * 弱引用是否可用
 * @return 0: 不可用; 1: 可用
 */
#define weak_ref_is_valid(obj)   ((obj)->value.weak_ref.ref != NULL)

/**
 * 获取弱引用对应的引用
 * 如果不可用, 返回 NULL
 * 注意, 立即数无法被检查是否引用, 因此引用立即数的弱引用是不可靠的
 */
#define weak_ref_get(obj)   ((obj)->value.weak_ref.ref)


/**
                           对象值操作: compare
******************************************************************************/
// TODO 实现基础对象比较


/**
                             malloc & free 封装
******************************************************************************/

/**
 * malloc() 的封装
 * @param size 字节数
 * @return 分配的内存块, 为空则分配失败
 */
EXPORT_API OUT NULLABLE void *raw_alloc(IN size_t size);

/**
 * free() 的封装
 * @param obj raw_alloc() 分配的内存
 */
EXPORT_API void raw_free(IN NOTNULL void *obj);

/**
 * realloc() 的封装
 * @param obj raw_alloc() 分配的内存
 */
EXPORT_API void *raw_realloc(IN NOTNULL void *obj, size_t new_size);


#endif // _BASE_SCHEME_OBJECT_HEADER_