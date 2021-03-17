#include <paper-scheme/vm.h>


/******************************************************************************
                                对象构造 API
******************************************************************************/

/**
 * 内部方法, 构造 object
 * @param context
 * @param type 类型 enum object_type_enum
 * @param unaligned_object_size
 * @return
 */
static OUT NOTNULL object
raw_object_make(REF NOTNULL context_t context, IN object_type_tag type, IN size_t unaligned_object_size) {
    assert(context != NULL);

    // 计算对象大小
    size_t real_size = aligned_size(unaligned_object_size);
    assert(unaligned_object_size <= real_size);

    size_t padding_size = real_size - unaligned_object_size;
    assert(padding_size <= UINT8_MAX);  // 填充大小不能超过 object->padding_size 类型最大值上限

    // 分配对象
    object ret = gc_alloc(context, real_size);
    memset(ret, 0, real_size);

    // 预处理对象头
    ret->magic = OBJECT_HEADER_MAGIC;
    ret->type = type;
    ret->marked = 0;
    ret->padding_size = (uint8_t) padding_size;   // 填充大小可能为0
    ret->forwarding = NULL;

    return ret;
}

/**
 * 构造 i64 类型对象, 不建议直接使用, 因为要考虑到复杂的边界条件, 参见 i64_make_op()
 * @param heap
 * @param v i64 值
 * @return
 */
EXPORT_API OUT NOTNULL object i64_make_real_object_op(REF NOTNULL context_t context, IN int64_t v) {
    assert(context != NULL);
    object ret = raw_object_make(context, OBJ_I64, object_sizeof_base(i64));
    // 对象赋值
    ret->value.i64 = v;
    return ret;
}

/**
 * 构造 i64 类型对象, 如果值范围属于 [- 2^(63-1), 2^(63-1)-1] 则构造立即数
 * @param heap
 * @param v i64 值
 * @return object 或立即数
 */
EXPORT_API OUT NOTNULL object i64_make_op(REF NOTNULL context_t context, IN int64_t v) {
    if (v >= I64_IMM_MIN && v <= I64_IMM_MAX) {
        return i64_imm_make(v);
    } else {
        return i64_make_real_object_op(context, v);
    }
}

/**
 * 构造 doublenum 类型对象
 * @param context
 * @param v
 * @return
 */
EXPORT_API OUT NOTNULL object doublenum_make_op(REF NOTNULL context_t context, double v) {
    assert(context != NULL);
    object ret = raw_object_make(context, OBJ_D64, object_sizeof_base(doublenum));
    ret->value.doublenum = v;
    return ret;
}

/**
 * 构造 pair 类型对象
 * @param context
 * @param v
 * @return
 */
EXPORT_API OUT NOTNULL object
pair_make_op(REF NOTNULL context_t context, REF NULLABLE object car, REF NULLABLE object cdr) {
    assert(context != NULL);
    // TODO 实现保护链
    object ret = raw_object_make(context, OBJ_PAIR, object_sizeof_base(pair));
    ret->value.pair.car = car;
    ret->value.pair.cdr = cdr;
    return ret;
}

/**
 * 构造 symbol 类型对象
 * <p>symbol_len() 运算结果不包括 '\0', 但是 object->symbol.len 包括 '\0', 这是为了方便运行时计算对象大小</p>
 * @param context
 * @param cstr C字符串, '\0'结尾
 * @return
 */
EXPORT_API OUT NOTNULL object
symbol_make_from_cstr_op(REF NOTNULL context_t context, char *cstr) {
    assert(context != NULL);
    size_t cstr_len;

    // 空指针则初始化为空字符串
    if (is_null(cstr)) { cstr_len = 1; }
    else { cstr_len = strlen(cstr) + 1; }

    object ret = raw_object_make(context, OBJ_SYMBOL,
                                 object_sizeof_base(symbol) + sizeof(char) * cstr_len);
    ret->value.symbol.len = cstr_len;
    if (is_null(cstr)) {
        assert(ret->value.symbol.len == 1);
        ret->value.symbol.data[0] = '\0';
    } else {
        assert(ret->value.symbol.len == strlen(cstr) + 1);
        memcpy(ret->value.symbol.data, cstr, cstr_len);
    }
    return ret;
}

/**
 * 构造 string 类型对象
 * <p>string_len() 运算结果不包括 '\0',
 * 但是 object->string.len 包括 '\0', 这是为了方便运行时计算对象大小</p>
 * @param context
 * @param cstr C字符串, '\0'结尾
 * @return
 */
EXPORT_API OUT NOTNULL object
string_make_from_cstr_op(REF NOTNULL context_t context, char *cstr) {
    assert(context != NULL);
    size_t cstr_len;

    // 空指针则初始化为空字符串
    if (is_null(cstr)) { cstr_len = 1; }
    else { cstr_len = strlen(cstr) + 1; }

    object ret = raw_object_make(context, OBJ_STRING,
                                 object_sizeof_base(string) + sizeof(char) * cstr_len);
    ret->value.string.len = cstr_len;
    if (is_null(cstr)) {
        assert(ret->value.string.len == 1);
        ret->value.string.data[0] = '\0';
    } else {
        assert(ret->value.string.len == strlen(cstr) + 1);
        memcpy(ret->value.string.data, cstr, cstr_len);
    }
    return ret;
}

/**
 * 构造 vector 类型对象, 初始化填充 Unit, 即 '()
 * @param context
 * @param vector_len vector 容量
 * @return
 */
EXPORT_API OUT NOTNULL object
vector_make_op(REF NOTNULL context_t context, IN size_t vector_len) {
    assert(context != NULL);

    object ret = raw_object_make(context, OBJ_VECTOR,
                                 object_sizeof_base(vector) + sizeof(object) * vector_len);
    ret->value.vector.len = vector_len;
    for (size_t i = 0; i < vector_len; i++) {
        ret->value.vector.data[i] = IMM_UNIT;
    }
    return ret;
}