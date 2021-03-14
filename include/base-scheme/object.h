#ifndef BASE_SCHEME_OBJECT_H
#define BASE_SCHEME_OBJECT_H
#pragma once


/**
 * object.h object.c
 * 对象结构定义
 */


#include "base-scheme/util.h"


/**
                               对象类型标记
******************************************************************************/
enum object_type_enum {
    FOREIGN_OBJECT = 0,
    OBJ_I64,
    OBJ_D64,
    OBJ_PAIR,
    OBJ_STRING,
    OBJ_SYMBOL,
    OBJ_VECTOR,
    OBJ_PORT,
// 不能超过 UINT8_MAX
            OBJECT_TYPE_ENUM_MAX,
};
COMPILE_TIME_ASSERT(OBJECT_TYPE_ENUM_MAX <= UINT8_MAX);

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
COMPILE_TIME_ASSERT(sizeof(uintptr_t) == 4u);
COMPILE_TIME_ASSERT(sizeof(void *) == 4u);
// 32位时, 指针和内存分配大小对齐到 8字节, B0111 = 7
# define ALIGN_MASK (7u)
// 指针和内存分配大小的最低 3 bit 必须为 0
# define ALIGN_BITS (3u)
// 对齐到 8u bytes
# define ALIGN_SIZE (8u)
#elif IS_64_BIT_ARCH
COMPILE_TIME_ASSERT(sizeof(uintptr_t) == 8u);
COMPILE_TIME_ASSERT(sizeof(void *) == 8u);
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

struct object_struct_t;
typedef struct object_struct_t *object;
struct object_struct_t {
    /*  对象头  */
    // 对象类型
    // 对象头魔数
    uint8_t magic;
    uint8_t type;
    // gc状态, 1为存活
    uint8_t marked: 1;
    // 对象尾部对齐填充大小
    uint8_t padding_size;
    // 移动对象用转发地址
    object forwarding;

    union object_value_u {
        /*  基本对象  */
        //定点64位有符号整数
        int64_t i64;
        //浮点数
        double doublenum;
        //pair
        struct value_pair_t {
            object car;
            object cdr;
        } pair;
        //字符串
        struct value_string_t {
            // char data[] 大小, 注意是指 char 个数
            size_t len;
            char data[0];
        } string;
        //symbol
        struct value_symbol_t {
            // char data[] 大小, 注意是指 char 个数
            size_t len;
            char data[0];
        } symbol;
        //向量
        struct value_vector_t {
            // object data[0] 大小, 注意是指 object 个数, 不是字节长度!
            size_t len;
            object data[0];
        } vector;

        /*  运行时结构  */
    } value;
    /*  对齐填充, 对齐到 ALIGN_SIZE, 即 sizeof(void *)  */
};



/**
                                复杂结构声明
******************************************************************************/


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
        - 真实的 object:     is_object(obj)
        - 立即数:            is_imm(obj)
        - NULL:             is_null(obj)    // ((void*)0)

    对于立即数:
        立即数构造方法:
            - unique:           MAKE_UNIQUE_IMMEDIATE(), 该方法不应当被直接使用
            - i64:              无, 参见 i64_make_real_object() 和 i64_make()
            - char:             char_imm_make()
        立即数取值方法:
            - unique:           无, 直接比较即可
            - i64:              无, 参见 i64_getvalue()
            - char:             char_getvalue()

    真实的 object:
        判别:
            - i64:              assert(is_i64(obj))
            - double number:    assert(is_doublenum(obj)
            - pair:             assert(is_pair(obj))
            - string:           assert(is_string(obj))
            - symbol:           assert(is_symbol(obj))
            - vector:           assert(is_vector(obj))
            - port: TODO port 判断 还未实现
        构造:
            - i64:              i64_make()
            - double number:    doublenum_make()
            - pair:             pair_make()
            - string:           string_make_from_cstr()
            - symbol:           symbol_make_from_cstr()
            - vector:           vector_make()
            - port: TODO port 构造 还未实现
        取值:
            - i64:              i64_getvalue()
            - double number:    doublenum_getvalue()
            - pair:             TODO pair_getcar(), pair_getcdr()
            - string:           TODO string_get_cstr(), string_len(), string_index()
            - symbol:           TODO symbol_make_get_cstr(), symbol_len(), symbol_index()
            - vector:           TODO vector_make(), vector_len(), vector_ref()
            - port: TODO port 取值 还未实现
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
# define I64_IMM_MIN (-4611686018427387904i64)
// i64 立即数最大值:   2^(63-1)-1
# define I64_IMM_MAX (4611686018427387903i64)
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
#define MAKE_UNIQUE_IMMEDIATE(n)  ((object) (((n)<<UNIQUE_IMMEDIATE_EXTENDED_BITS) \
                                          | UNIQUE_IMMEDIATE_TAG))


// 检查是否为特殊立即数, 注意 NULL 不是特殊立即数
#define is_unique_imm(x) ((ptr_to_uintptr(x) & UNIQUE_IMMEDIATE_MASK) == UNIQUE_IMMEDIATE_TAG)

/**
                               常量立即数定义
******************************************************************************/
/**
 * 基本类型
 * false
 */
#define IMM_FALSE MAKE_UNIQUE_IMMEDIATE(0u)
/**
 * 基本类型
 * true
 */
#define IMM_TRUE MAKE_UNIQUE_IMMEDIATE(1u)
/**
 * 基本类型
 * <li>Unit, 等价于 '()</li>
 * <li>对于内部结构, 应当统一使用 NULL; 但 List 应当以 Unit 结束而不是 NULL</li>
 */
#define IMM_UNIT MAKE_UNIQUE_IMMEDIATE(2u)
// 虚拟机内部类型


/**
                               立即数操作
******************************************************************************/

// 检查是否为 i64 立即数
#define is_i64_imm(x) ((ptr_to_uintptr(x) & I64_IMM_MASK) == I64_IMM_TAG)
// 生成 i64 立即数
#define i64_imm_make(x) ((object) ((ptr_to_uintptr(x) << I64_EXTENDED_BITS) | I64_IMM_TAG))

// 检查是否为对象指针
// 这意味着如果表达式成立, 则它是个有效的 object 指针
#define is_object(obj) ((!is_null(obj)) \
    && ((ptr_to_uintptr(obj) & POINTER_MASK) == POINTER_TAG) \
    && (((object) (obj))->magic == OBJECT_HEADER_MAGIC))

// 检查是否为 char 立即数, 参数必须为 object
#define is_char_imm(x) ((ptr_to_uintptr(x) & CHAR_IMM_MASK) == CHAR_IMM_TAG)
// 构造 char 对象/立即数, 参数为 char, 注意类型
#define char_imm_make(x) ((object) (((ptr_to_uintptr(x) & CHAR_VALUE_MASK) << CHAR_EXTENDED_BITS) | CHAR_IMM_TAG))
// char 立即数取值, 参数必须为 object, 返回值为 char
#define char_imm_getvalue(x) ((char) ((ptr_to_uintptr(x) >> CHAR_EXTENDED_BITS) & CHAR_VALUE_MASK))

// 检查是否为立即数
#define is_imm(x) (!is_null(x) && ((ptr_to_uintptr(x) & POINTER_MASK) != POINTER_TAG))



/**
                               对象操作 API
******************************************************************************/

/**
 * 计算对象大小
 * @param value_field object->value->value_field
 * @return 对象大小
 */
#define object_size(value_field)\
    (offsetof(struct object_struct_t, value) + sizeof(((object)0)->value.value_field))

/**
 * 计算对齐的大小
 * 用法: aligned_size(object_size(value_field))
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
 * @param object
 */
EXPORT_API OUT OUT size_t object_size_runtime(REF NOTNULL object obj);


/**
                               对象值操作
******************************************************************************/

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
#define is_i64_real(object) (is_object(object) && ((object)->type == OBJ_I64))
/**
 * 判断 object 是否为 doublenum
 * @param object NULLABLE
 * @return !0 -> true, 0 -> false
 */
#define is_doublenum(object) (is_object(object) && ((object)->type == OBJ_D64))
/**
 * 判断 object 是否为 pair
 * @param object NULLABLE
 * @return !0 -> true, 0 -> false
 */
#define is_pair(object) (is_object(object) && ((object)->type == OBJ_PAIR))
/**
 * 判断 object 是否为 string
 * @param object NULLABLE
 * @return !0 -> true, 0 -> false
 */
#define is_string(object) (is_object(object) && ((object)->type == OBJ_STRING))
/**
 * 判断 object 是否为 symbol
 * @param object NULLABLE
 * @return !0 -> true, 0 -> false
 */
#define is_symbol(object) (is_object(object) && ((object)->type == OBJ_SYMBOL))
/**
 * 判断 object 是否为 vector
 * @param object NULLABLE
 * @return !0 -> true, 0 -> false
 */
#define is_vector(object) (is_object(object) && ((object)->type == OBJ_VECTOR))

/**
 * 获取 i64 对象的值
 * @param i64
 * @return
 */
EXPORT_API OUT int64_t i64_getvalue(REF NOTNULL object i64);


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


#endif // _BASE_SCHEME_OBJECT_HEADER_