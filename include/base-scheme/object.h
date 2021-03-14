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
            size_t size;
            char data[0];
        } string;
        //symbol
        struct value_symbol_t {
            size_t size;
            char data[0];
        } symbol;
        //向量
        struct value_vector_t {
            size_t size;
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
    普通立即数标记
******************************************************************************/
#define USE_IMMEDIATE

// i64 立即数最低1位舍弃
#define I64_EXTENDED_BITS (1u)
// i64 立即数掩码 B01
#define I64_TAG_MASK (1u)
// i64 立即数标记 B01
#define I64_TAG (1u)
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
#define POINTER_TAG_MASK (3u)

// 无符号 char (8bits), 立即数最低 8 位舍弃
#define CHAR_EXTENDED_BITS (8u)
// char 立即数掩码 B0001 1110
#define CHAR_TAG_MASK (30u)
// char 立即数标记 B0001 1110
#define CHAR_TAG (30u)
// char 立即数取值MASK
#define CHAR_VALUE_MASK (255u)


/**
    立即数标记定义
    `*` 标记为已经实现
******************************************************************************/
/**
 *   bits end in
 *           *         1:  i64 number
 *                    00:  pointer
 *   imm:
 *                  0 10:  string cursor (optional)
 *                 01 10:  immediate symbol (optional)
 *            0000 11 10:  immediate flonum (optional)
 *           *0001 11 10:  char, 30u
 *            0010 11 10:  reader label (optional)
 *           *0011 11 10:  unique immediate (NULL, TRUE, FALSE or else), 62u
 */



/**
    常量立即数标记
******************************************************************************/
/**
 * 最低 8 位用于常量立即数标记
 */
#define UNIQUE_IMMEDIATE_EXTENDED_BITS (8u)
/**
 * 常量立即数标记掩码
 */
#define UNIQUE_IMMEDIATE_MASK (62u)
/**
 * 常量立即数生成
 */
#define MAKE_UNIQUE_IMMEDIATE(n)  ((object) (((n)<<UNIQUE_IMMEDIATE_EXTENDED_BITS) \
                                          + UNIQUE_IMMEDIATE_MASK))


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
 * nils
 */
#define IMM_NIL MAKE_UNIQUE_IMMEDIATE(2u)
// 虚拟机内部类型


/**
    立即数操作
******************************************************************************/

// 检查是否为 i64 立即数
#define is_i64_imm(x) ((ptr_to_uintptr(x) & I64_TAG_MASK) != 0)
// 生成 i64 立即数
#define i64_imm_make(x) ((object) ((ptr_to_uintptr(x) << I64_EXTENDED_BITS) | I64_TAG))

// 检查是否为对象指针
#define is_object(x) ((ptr_to_uintptr(x) & POINTER_TAG_MASK) == 0)

// 检查是否为 char 立即数, 参数必须为 object
#define is_char_imm(x) ((ptr_to_uintptr(x) & CHAR_TAG_MASK) != 0)
// 构造 char 对象/立即数, 参数为 char, 注意类型
#define char_imm_make(x) ((object) (((ptr_to_uintptr(x) & CHAR_VALUE_MASK) << CHAR_EXTENDED_BITS) | CHAR_TAG))
// char 立即数取值, 参数必须为 object, 返回值为 char
#define char_imm_getvalue(x) ((char) ((ptr_to_uintptr(x) >> CHAR_EXTENDED_BITS) & CHAR_VALUE_MASK))

// 检查是否为立即数
#define is_imm(x) (!is_object((x)))


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

#define is_null(x) ((x) == NULL)
/**
 * 判断 object 是否为 i64
 * @param i64 NULLABLE
 * @return !0 -> true, 0 -> false
 */
EXPORT_API OUT int is_i64(REF NULLABLE object i64);
/**
 * 判断 object 是否为 doublenum
 * @param object NULLABLE
 * @return !0 -> true, 0 -> false
 */
#define is_doublenum(object) (!is_null(x) && (is_object(x)) && ((x).type == OBJ_D64))
/**
 * 判断 object 是否为 pair
 * @param object NULLABLE
 * @return !0 -> true, 0 -> false
 */
#define is_pair(x) (!is_null(x) && (is_object(x))&& ((x).type == OBJ_PAIR))
/**
 * 判断 object 是否为 string
 * @param object NULLABLE
 * @return !0 -> true, 0 -> false
 */
#define is_string(x) (!is_null(object) && (is_object(object)) && ((object).type == OBJ_STRING))
/**
 * 判断 object 是否为 symbol
 * @param object NULLABLE
 * @return !0 -> true, 0 -> false
 */
#define is_symbol(object) (!is_null(object) && (is_object(object)) && ((object).type == OBJ_SYMBOL))
/**
 * 判断 object 是否为 vector
 * @param object NULLABLE
 * @return !0 -> true, 0 -> false
 */
#define is_vector(object) (!is_null(object) && (is_object(object)) && ((object).type == OBJ_VECTOR))

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