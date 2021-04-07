#include <paper-scheme/runtime.h>


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
static OUT NOTNULL GC object
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
EXPORT_API OUT NOTNULL GC object i64_make_real_object_op(REF NOTNULL context_t context, IN int64_t v) {
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
EXPORT_API OUT NOTNULL GC object i64_make_op(REF NOTNULL context_t context, IN int64_t v) {
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
EXPORT_API OUT NOTNULL GC object doublenum_make_op(REF NOTNULL context_t context, IN double v) {
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
EXPORT_API OUT NOTNULL GC object
pair_make_op(REF NOTNULL context_t context, REF NULLABLE object param_car, REF NULLABLE object param_cdr) {
    assert(context != NULL);

    gc_param2(context, param_car, param_cdr);

    object ret = raw_object_make(context, OBJ_PAIR, object_sizeof_base(pair));
    ret->value.pair.car = param_car;
    ret->value.pair.cdr = param_cdr;

    gc_release_param(context);
    return ret;
}


/**
 * 构造 bytes 对象
 * @param context
 * @param capacity bytes 容量, bytes_size * sizeof(char)
 * @return
 */
EXPORT_API OUT NOTNULL GC object
bytes_make_op(REF NOTNULL context_t context, IN size_t capacity) {
    assert(context != NULL);

    object ret = raw_object_make(context, OBJ_BYTES,
                                 object_sizeof_base(bytes) + capacity * sizeof(char));
    ret->value.bytes.capacity = capacity;
    ret->value.bytes.hash = pointer_with_type_to_hash(ret, OBJ_BYTES);
    return ret;
}

/**
 * 构造 symbol 类型对象
 * <p>symbol_len() 运算结果不包括 '\0', 但是 object->symbol.len 包括 '\0', 这是为了方便运行时计算对象大小</p>
 * @param context
 * @param cstr C字符串, '\0'结尾
 * @return
 */
EXPORT_API OUT NOTNULL GC object
symbol_make_from_cstr_op(REF NOTNULL context_t context, COPY char *cstr) {
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

    ret->value.symbol.hash = symbol_hash_helper(ret);
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
EXPORT_API OUT NOTNULL GC object
string_make_from_cstr_op(REF NOTNULL context_t context, COPY char *cstr) {
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

    ret->value.string.hash = string_hash_helper(ret);
    return ret;
}

/**
 * 构造 string_buffer 对象
 * @param context
 * @param char_size char 容量, 注意 string_buffer 不以 '\0' 结束
 * @return
 */
EXPORT_API OUT NOTNULL GC object
string_buffer_make_op(REF NOTNULL context_t context, IN size_t char_size) {
    assert(context != NULL);
    gc_var2(context, ret, tmp);

    size_t buffer_size = char_size;
    ret = raw_object_make(context, OBJ_STRING_BUFFER,
                          object_sizeof_base(string_buffer));
    tmp = bytes_make_op(context, buffer_size);
    bytes_data(tmp)[0] = '\0';

    ret->value.string_buffer.bytes_buffer = tmp;
    ret->value.string_buffer.buffer_size = buffer_size;
    // buffer 中没有 '\0' 作为结尾
    ret->value.string_buffer.buffer_length = 0;
    ret->value.string_buffer.hash = pointer_with_type_to_hash(ret, OBJ_STRING_BUFFER);

    gc_release_var(context);
    return ret;
}

/**
 * 从 string 构造 string_buffer 对象, 末尾包含 '\0'
 * @param context
 * @param str string, 深拷贝
 * @return
 */
EXPORT_API OUT NOTNULL GC object
string_buffer_make_from_string_op(REF NOTNULL context_t context, COPY object str) {
    assert(context != NULL);
    assert(is_object(str));
    assert(is_string(str));

    gc_param1(context, str);
    gc_var2(context, ret, tmp);

    ret = raw_object_make(context, OBJ_STRING_BUFFER,
                          object_sizeof_base(string_buffer));

    // 不包括结束的 '\0'
    size_t str_len = string_len(str);
    // 实际 buffer 大小要加上 STRING_BUFFER_DEFAULT_INIT_SIZE, 因为 buffer 常常继续增长
    // todo string_buffer 增长策略也许需要修改
    size_t new_buffer_size = str_len + STRING_BUFFER_DEFAULT_GROWTH_SIZE;
    tmp = bytes_make_op(context, new_buffer_size);
    memcpy(bytes_data(tmp), string_get_cstr(str), str_len);

    ret->value.string_buffer.bytes_buffer = tmp;
    ret->value.string_buffer.buffer_size = new_buffer_size;
    ret->value.string_buffer.buffer_length = str_len;
    ret->value.string_buffer.hash = pointer_with_type_to_hash(ret, OBJ_STRING_BUFFER);

    assert(bytes_capacity(string_buffer_bytes_obj(ret)) == string_buffer_capacity(ret));
    gc_release_param(context);
    return ret;
}

/**
 * 构造 vector 类型对象, 初始化填充 Unit, 即 '()
 * @param context
 * @param vector_len vector 容量
 * @return
 */
EXPORT_API OUT NOTNULL GC object
vector_make_op(REF NOTNULL context_t context, IN size_t vector_len) {
    assert(context != NULL);

    object ret = raw_object_make(context, OBJ_VECTOR,
                                 object_sizeof_base(vector) + sizeof(object) * vector_len);
    ret->value.vector.len = vector_len;
    for (size_t i = 0; i < vector_len; i++) {
        ret->value.vector.data[i] = IMM_UNIT;
    }
    ret->value.vector.hash = pointer_with_type_to_hash(ret, OBJ_VECTOR);

    return ret;
}

/**
 * 构造 stack 类型对象, 填充 Unit.
 * @param context
 * @param stack_size
 * @return
 */
EXPORT_API OUT NOTNULL GC object
stack_make_op(REF NOTNULL context_t context, IN size_t stack_size) {
    assert(context != NULL);

    object ret = raw_object_make(context, OBJ_STACK,
                                 object_sizeof_base(stack) + sizeof(object) * stack_size);

    ret->value.stack.size = stack_size;
    for (size_t i = 0; i < stack_size; i++) {
        ret->value.stack.data[i] = IMM_UNIT;
    }
    ret->value.stack.length = 0;
    ret->value.stack.hash = pointer_with_type_to_hash(ret, OBJ_STACK);

    return ret;
}

/**
 * 从输入字符串打开 input port, 传入的 string 应当是不可变对象
 * <p>obj->value.string_port.length 长度 不包含 '\0'</p>
 * <p>(open-input-string "string here")</p>
 * @param context
 * @param str
 * @return 打开失败返回 IMM_UNIT
 */
EXPORT_API OUT NOTNULL GC object
string_port_input_from_string(REF NOTNULL context_t context, REF NULLABLE object str) {
    assert(context != NULL);
    assert(is_string(str));

    gc_param1(context, str);

    object ret = raw_object_make(context, OBJ_STRING_PORT, object_sizeof_base(string_port));
    ret->value.string_port.kind = PORT_INPUT;
    ret->value.string_port.string_buffer_data = str;
    ret->value.string_port.length = string_len(str);
    ret->value.string_port.current = 0;
    ret->value.string_port.hash = pointer_with_type_to_hash(ret, OBJ_STRING_PORT);

    gc_release_param(context);
    return ret;
}

/**
 * 打开 output port, 具有内部缓冲
 * <p>(open-output-string)</p>
 * @param context
 * @return 打开失败返回 IMM_UNIT, 否则返回值满足 is_srfi6_port(obj),
 */
EXPORT_API OUT NOTNULL GC object
string_port_output_use_buffer(REF NOTNULL context_t context) {
    assert(context != NULL);

    gc_var1(context, str_buffer);

    object ret = raw_object_make(context, OBJ_STRING_PORT, object_sizeof_base(string_port));

    ret->value.string_port.kind = (unsigned) PORT_OUTPUT | (unsigned) PORT_SRFI6;
    ret->value.string_port.string_buffer_data = string_buffer_make_op(context, STRING_BUFFER_DEFAULT_INIT_SIZE);
    ret->value.string_port.current = 0;
    ret->value.string_port.hash = pointer_with_type_to_hash(ret, OBJ_STRING_PORT);

    gc_release_var(context);
    return ret;
}

/**
 * 从输入字符串打开 input-output port, 深拷贝, 具有内部缓冲
 * <p>(open-input-output-string "string here")</p>
 * @param context
 * @param str
 * @return 打开失败返回 IMM_UNIT, 否则返回值满足 is_srfi6_port(obj), is_string_port_in_out_put(obj)
 */
EXPORT_API OUT NOTNULL GC object
string_port_in_out_put_from_string_use_buffer(REF NOTNULL context_t context, REF NULLABLE object str) {
    assert(context != NULL);

    gc_param1(context, str);
    gc_var1(context, str_buffer);

    object ret = raw_object_make(context, OBJ_STRING_PORT, object_sizeof_base(string_port));

    ret->value.string_port.kind = (unsigned) PORT_OUTPUT | (unsigned) PORT_INPUT | (unsigned) PORT_SRFI6;
    ret->value.string_port.string_buffer_data = string_buffer_make_from_string_op(context, str);
    ret->value.string_port.current = 0;
    ret->value.string_port.hash = pointer_with_type_to_hash(ret, OBJ_STRING_PORT);

    gc_release_param(context);
    return ret;
}

/**
 * 从文件名打开 file port
 * @param context
 * @param filename
 * @param kind PORT_INPUT / PORT_OUTPUT / PORT_INPUT & PORT_OUTPUT
 * @return 打开失败返回 IMM_UNIT
 */
EXPORT_API OUT NOTNULL GC object
stdio_port_from_filename(REF NOTNULL context_t context, REF NULLABLE object filename, IN enum port_kind kind) {
    assert(context != NULL);
    assert(is_string(filename));

    gc_param1(context, filename);
    gc_var1(context, port);

    char *mode = NULL;
    if (kind == ((unsigned) PORT_INPUT | (unsigned) PORT_OUTPUT)) {
        mode = "a+";
    } else if (kind == PORT_OUTPUT) {
        mode = "w";
    } else {
        mode = "r";
    }

    FILE *file = fopen(string_get_cstr(filename), mode);
    if (file == NULL) {
        return IMM_UNIT;
    }
    port = raw_object_make(context, OBJ_STDIO_PORT, object_sizeof_base(stdio_port));

    port->value.stdio_port.kind = kind;
    port->value.stdio_port.file = file;
    port->value.stdio_port.filename = filename;
    port->value.stdio_port.is_released = 0;
    // 打开的文件需要关闭
    port->value.stdio_port.need_close = 1;
    port->value.stdio_port.hash = pointer_with_type_to_hash(port, OBJ_STDIO_PORT);

    gc_release_param(context);
    return port;
}

/**
 * 从 FILE * 打开 file port
 * @param context
 * @param file
 * @param kind PORT_INPUT / PORT_OUTPUT / PORT_INPUT & PORT_OUTPUT
 * @return
 */
EXPORT_API OUT NOTNULL GC object
stdio_port_from_file(REF NOTNULL context_t context, REF NOTNULL FILE *file, enum port_kind kind) {
    assert(context != NULL);

    gc_var1(context, port);

    port = raw_object_make(context, OBJ_STDIO_PORT, object_sizeof_base(stdio_port));

    port->value.stdio_port.kind = kind;
    port->value.stdio_port.file = file;
    port->value.stdio_port.filename = IMM_UNIT;
    port->value.stdio_port.is_released = 0;
    // 外部打开的文件外部负责关闭
    port->value.stdio_port.need_close = 0;
    port->value.stdio_port.hash = pointer_with_type_to_hash(port, OBJ_STDIO_PORT);

    gc_release_var(context);
    return port;
}

/**
 * 构造 hashset
 * @param context
 * @param init_capacity hashset 初始大小 (默认 DEFAULT_HASH_SET_MAP_INIT_CAPACITY)
 * @param load_factor 负载因子 (默认大小 DEFAULT_HASH_SET_MAP_LOAD_FACTOR)
 * @return
 */
EXPORT_API OUT NOTNULL GC object
hashset_make_op(REF NOTNULL context_t context, IN size_t init_capacity, IN double load_factor) {
    assert(context != NULL);
    assert(load_factor >= 0);

    gc_var2(context, hashset, map);

    map = hashmap_make_op(context, init_capacity, load_factor);
    hashset = raw_object_make(context, OBJ_HASH_SET, object_sizeof_base(hashset));
    hashset->value.hashset.map = map;
    hashset->value.hashset.hash = pointer_with_type_to_hash(hashset, OBJ_HASH_SET);

    gc_release_var(context);
    return hashset;
}

/**
 * 构造 hashmap
 * @param context
 * @param init_capacity hashmap 初始大小 (默认 DEFAULT_HASH_SET_MAP_INIT_CAPACITY)
 * @param load_factor 负载因子 (默认大小 DEFAULT_HASH_SET_MAP_LOAD_FACTOR)
 * @return
 */
EXPORT_API OUT NOTNULL GC object
hashmap_make_op(REF NOTNULL context_t context, IN size_t init_capacity, IN double load_factor) {
    assert(context != NULL);
    assert(load_factor >= 0);

    gc_var2(context, hashmap, table);

    table = vector_make_op(context, init_capacity);
    hashmap = raw_object_make(context, OBJ_HASH_MAP, object_sizeof_base(hashmap));
    hashmap->value.hashmap.table = table;
    hashmap->value.hashmap.size = 0;
    hashmap->value.hashmap.load_factor = load_factor;
    hashmap->value.hashmap.threshold = init_capacity;
    hashmap->value.hashmap.hash = pointer_with_type_to_hash(hashmap, OBJ_HASH_MAP);

    gc_release_var(context);
    return hashmap;
}

/**
 * 构造 weak ref
 * @param context
 * @param obj
 * @return
 */
EXPORT_API OUT NOTNULL GC object
weak_ref_make_op(REF NOTNULL context_t context, REF NULLABLE object obj) {
    assert(context != NULL);

    gc_param1(context, obj);
    gc_var1(context, weak);

    weak = raw_object_make(context, OBJ_WEAK_REF, object_sizeof_base(weak_ref));
    weak->value.weak_ref._internal_next_ref = NULL;
    weak->value.weak_ref.ref = obj;
    weak->value.weak_ref.hash = pointer_with_type_to_hash(weak, OBJ_WEAK_REF);

    gc_release_param(context);
    return weak;
}

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
string_append_op(REF NOTNULL context_t context, COPY NULLABLE object string_a, COPY NULLABLE object string_b) {
    assert(context != NULL);
    assert(is_string(string_a));
    assert(is_string(string_b));

    gc_param2(context, string_a, string_b);
    gc_var1(context, ret);

    size_t string_a_len = string_len(string_a);
    assert(string_len(string_a) == strlen(string_a->value.string.data));
    size_t string_b_len = string_len(string_b);
    assert(string_len(string_b) == strlen(string_b->value.string.data));

    size_t new_len_with_null = string_a_len + string_b_len + 1u;

    ret = raw_object_make(context, OBJ_STRING,
                          object_sizeof_base(string) + new_len_with_null * sizeof(char));

    // 先复制第一个字符串, 不包括 '\0'
    memcpy(string_get_cstr(ret), string_get_cstr(string_a), string_a_len);
    // 再复制第二个字符串, 末尾包括 '\0'
    memcpy(((char *) string_get_cstr(ret)) + string_a_len, string_get_cstr(string_b), string_b_len + 1);
    // 保险起见, 末尾填充 '\0'
    ((char *) string_get_cstr(ret))[new_len_with_null - 1] = '\0';
    // 修正字符串长度
    ret->value.string.len = new_len_with_null;
    ret->value.string.hash = string_hash_helper(ret);

    gc_release_param(context);
    return ret;
}

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
        IN NULLABLE object str_buffer, COPY NULLABLE object str) {

    assert(context != NULL);
    assert(is_string_buffer(str_buffer));
    assert(is_string(str));

    gc_param2(context, str_buffer, str);
    gc_var1(context, tmp);

    size_t str_buffer_len = string_buffer_length(str_buffer);
    size_t str_len = string_len(str);
    size_t buffer_size = string_buffer_capacity(str_buffer);

    assert(string_get_cstr(str)[str_len] == '\0');

    if (str_len + str_buffer_len > buffer_size) {
        // 容量不足
        size_t add_size = (str_len + str_buffer_len + STRING_BUFFER_DEFAULT_GROWTH_SIZE) - buffer_size;
        str_buffer = string_buffer_capacity_increase(context, str_buffer, add_size);

        assert(add_size + buffer_size == string_buffer_capacity(str_buffer));
    }

    // 不复制 '\0'
    memcpy(string_buffer_bytes_data(str_buffer) + str_buffer_len, string_get_cstr(str), str_len);
    // 修正长度
    str_buffer->value.string_buffer.buffer_length = str_buffer_len + str_len;

    gc_release_param(context);
    return str_buffer;
}

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
        IN NULLABLE object str_buffer, COPY NOTNULL object imm_char) {

    assert(context != NULL);
    assert(is_imm_char(imm_char));

    gc_param1(context, str_buffer);
    str_buffer = string_buffer_append_char_op(context, str_buffer, char_imm_getvalue(imm_char));
    gc_release_param(context);
    return str_buffer;
}

/**
 * string_buffer 拼接 char, 深拷贝
 * @param context
 * @param str_buffer
 * @param ch
 * @return
 */
EXPORT_API OUT NOTNULL GC object
string_buffer_append_char_op(REF NOTNULL context_t context, IN NULLABLE object str_buffer, COPY char ch) {
    assert(context != NULL);
    assert(ch != '\0');

    gc_param1(context, str_buffer);

    size_t buffer_len = string_buffer_length(str_buffer);
    if (buffer_len + 1 > string_buffer_capacity(str_buffer)) {
        // 当前 string_buffer_length(str_buffer) + 1, 其中 '+1' 为 新增加的 char
        str_buffer = string_buffer_capacity_increase(context, str_buffer, STRING_BUFFER_DEFAULT_GROWTH_SIZE + 1);
    }

    // char 拼接
    string_buffer_bytes_data(str_buffer)[buffer_len] = ch;
    // string_buffer 长度 +1
    string_buffer_length(str_buffer)++;

    gc_release_param(context);
    return str_buffer;
}


/**
 * hashset 是否包含指定的对象
 * <p>不会触发 GC</p>
 * @param context
 * @param hashset
 * @param obj object 不能为 NULL
 * @return IMM_TRUE / IMM_FALSE
 */
EXPORT_API OUT NOTNULL object
hashset_contains_op(REF NOTNULL context_t context, REF NOTNULL object hashset, REF NOTNULL object obj) {
    assert(context != NULL);
    assert(is_hashset(hashset));
    assert(!is_null(obj));
    gc_set_flag(context);

    object ret = hashmap_contains_key_op(context, hashset->value.hashset.map, obj);

    assert(is_imm_true(ret) || is_imm_false(ret));
    gc_assert_no_gc(context);
    return ret;
}

/**
 * obj 放入 hashset
 * @param context
 * @param obj
 * @return 如果已经存在旧值, 将存入新值返回旧值, 否则返回 IMM_UNIT
 */
EXPORT_API GC void
hashset_put_op(REF NOTNULL context_t context, REF NOTNULL object hashset, REF NOTNULL object obj) {
    assert(context != NULL);
    assert(is_hashset(hashset));
    assert(!is_null(obj));

    gc_param2(context, hashset, obj);
    // 所有的 key 对应的 value 都是 IMM_TRUE
    hashmap_put_op(context, hashset->value.hashset.map, obj, IMM_TRUE);
    gc_release_param(context);
}

/**
 * hashset_b 全部放入 hashset_a, 浅拷贝
 * @param context
 * @param hashset_a 不能为空
 * @param hashset_b 不能为空
 */
EXPORT_API GC void
hashset_put_all_op(REF NOTNULL context_t context, REF NOTNULL object hashset_a, REF NOTNULL object hashset_b) {
    assert(context != NULL);
    assert(hashset_a != hashset_b);
    assert(is_hashset(hashset_a));
    assert(is_hashset(hashset_b));

    gc_param2(context, hashset_a, hashset_b);
    hashmap_put_all_op(context, hashset_a->value.hashset.map, hashset_b->value.hashset.map);
    gc_release_param(context);
}

/**
 * 清空 hashset
 * <p>不会触发 GC</p>
 * @param context
 * @param hashset 不能为空
 * @return
 */
EXPORT_API void hashset_clear_op(REF NOTNULL context_t context, REF NOTNULL object hashset) {
    assert(context != NULL);
    assert(is_hashset(hashset));
    gc_set_flag(context);

    hashmap_clear_op(context, hashset->value.hashset.map);

    gc_assert_no_gc(context);
}

/**
 * 从 hashset 中移除 object
 * <p>不会触发 GC</p>
 * @param context
 * @param hashset
 * @param obj 不能为空, 可以为 IMM_UNIT
 */
EXPORT_API void
hashset_remove_op(REF NOTNULL context_t context, REF NOTNULL object hashset, REF NOTNULL object obj) {
    assert(context != NULL);
    assert(is_hashset(hashset));
    assert(!is_null(obj));
    gc_set_flag(context);

    hashmap_remove_op(context, hashset->value.hashset.map, obj);

    gc_assert_no_gc(context);
}


/**
 * hashmap 是否包含指定的对象
 * <p>不会触发 GC</p>
 * @param context
 * @param hashmap
 * @param key object 不能为 NULL
 * @return IMM_TRUE / IMM_FALSE
 */
EXPORT_API OUT NOTNULL object
hashmap_contains_key_op(REF NOTNULL context_t context, REF NOTNULL object hashmap, REF NOTNULL object key) {
    assert(context != NULL);
    assert(is_hashmap(hashmap));
    assert(!is_null(key));
    gc_set_flag(context);

    if (hashmap->value.hashmap.size == 0) {
        return IMM_FALSE;
    }

    size_t map_vector_len = vector_len(hashmap->value.hashmap.table);
    size_t hash = 0;
    size_t index = 0;
    hash_code_fn hash_fn = object_hash_helper(context, key);
    equals_fn equals = object_equals_helper(context, key);
    assert(equals != NULL);
    if (hash_fn != NULL) {
        hash = hash_fn(context, key);
    }
    index = hash % map_vector_len;

    for (object entry_list = vector_ref(hashmap->value.hashmap.table, index);
         entry_list != IMM_UNIT; entry_list = pair_cdr(entry_list)) {
        object found_key = pair_caar(entry_list);
        if (found_key == key || equals(context, found_key, key)) {
            return IMM_TRUE;
        }
    }

    gc_assert_no_gc(context);
    // 找不到节点
    return IMM_FALSE;
}

/**
 * obj 放入 hashmap
 * @param context
 * @param hashmap
 * @param k 键
 * @param v 值
 * @return 如果 k 已经存在, 则返回旧的 v, 否则返回 IMM_UNIT
 */
EXPORT_API OUT NOTNULL GC object
hashmap_put_op(REF NOTNULL context_t context, object hashmap, REF NOTNULL object k, REF NOTNULL object v) {
    assert(context != NULL);
    assert(hashmap != NULL);
    assert(!is_null(k));
    assert(!is_null(v));

    gc_param3(context, hashmap, k, v);
    gc_var7(context, vector, new_vector, old_key, old_value, new_entry, old_entry, entry_list);

    hash_code_fn hash_fn = object_hash_helper(context, k);
    equals_fn equals = object_equals_helper(context, k);
    assert(equals != NULL);

    // 1. 计算 hash 值
    uint32_t hash = 0;

    // hash 函数为空, hash 值为 0
    if (hash_fn != NULL) {
        hash = hash_fn(context, k);
    }

    // 2. 根据 hash 值计算索引位置
    vector = hashmap->value.hashmap.table;
    size_t vec_len = vector_len(vector);
    size_t index = hash % vec_len;

    // 3. 检查索引位置是否有旧值, 如果存在则替换, 返回旧值
    for (entry_list = vector_ref(vector, index); entry_list != IMM_UNIT; entry_list = pair_cdr(entry_list)) {
        assert(is_pair(entry_list));
        old_entry = pair_car(entry_list);
        assert(is_pair(old_entry));

        // 取出旧值
        old_key = pair_car(old_entry);
        if (k == old_key || equals(context, k, old_key)) {
            old_value = pair_cdr(old_entry);

            // 创建新节点替换链表
            new_entry = pair_make_op(context, k, v);
            pair_car(entry_list) = new_entry;

            return old_value;
        }
    }


    // 4. 没有找到旧值, 说明要创建新的 entry, 检查是否需要扩容
    if (hashmap->value.hashmap.size >= hashmap->value.hashmap.threshold &&
        (vector_ref(vector, index) != IMM_UNIT)) {
        // 此时容量大于等于扩容阈值, 且 index 位置不为 IMM_UNIT
        // 则需要扩容
        size_t new_len = vec_len * 2;
        new_vector = vector_make_op(context, new_len);

        // 转移旧的内容, 需要重新计算 index
        for (size_t i = 0; i < vec_len; i++) {

            // 取出旧的 entry
            entry_list = vector_ref(vector, i);

            if (entry_list != IMM_UNIT) {

                // 不为空的时候需要进行 entry 重新 hash 并转移
                // 否则不需要处理, 因为 vector 默认初始化为 IMM_UNIT
                old_entry = pair_car(entry_list);
                old_key = pair_car(old_entry);
                size_t tmp_hash = 0;
                hash_code_fn tmp_hash_fn = object_hash_helper(context, old_key);
                if (tmp_hash_fn != NULL) {
                    tmp_hash = tmp_hash_fn(context, old_key);
                }
                size_t new_index = tmp_hash % new_len;

                // 旧的 entry_list 的 hash 值应该都是相同的
                vector_ref(new_vector, new_index) = entry_list;
            }
        }
        // 修改 vector

        // 转移结束, 重新计算 hash 值
        hash = 0;
        if (hash_fn != NULL) {
            hash = hash_fn(context, k);
        }
        index = hash % new_len;

        // 最后插入新节点
        new_entry = pair_make_op(context, k, v);
        entry_list = pair_make_op(context, new_entry, vector_ref(new_vector, index));
        vector_set(new_vector, index, entry_list);
        hashmap->value.hashmap.table = new_vector;
        hashmap->value.hashmap.threshold = (size_t) (vector_len(new_vector) * hashmap->value.hashmap.load_factor);
    } else {
        // 5. 否则不需要扩容, 插入新节点
        vector = hashmap->value.hashmap.table;
        new_entry = pair_make_op(context, k, v);
        entry_list = pair_make_op(context, new_entry, vector_ref(vector, index));
        vector_set(vector, index, entry_list);
    }

    // 此时插入过新的键值对, 返回 IMM_UNIT
    hashmap->value.hashmap.size++;
    gc_release_param(context);
    return IMM_UNIT;
}

/**
 * hashmap 取出 key 对应的 value
 * <p>不会触发 GC</p>
 * @param context
 * @param hashmap
 * @param key
 * @return 如果 key 存在, 则返回对应的 value, 否则返回 IMM_UNIT
 */
EXPORT_API OUT NOTNULL object
hashmap_get_op(REF NOTNULL context_t context, object hashmap, REF NOTNULL object key) {
    assert(context != NULL);
    assert(is_hashmap(hashmap));
    assert(!is_null(key));
    gc_set_flag(context);

    if (hashmap->value.hashmap.size == 0) {
        return IMM_UNIT;
    }

    size_t map_vector_len = vector_len(hashmap->value.hashmap.table);
    size_t hash = 0;
    size_t index = 0;
    hash_code_fn hash_fn = object_hash_helper(context, key);
    equals_fn equals = object_equals_helper(context, key);
    assert(equals != NULL);
    if (hash_fn != NULL) {
        hash = hash_fn(context, key);
    }
    index = hash % map_vector_len;

    for (object entry_list = vector_ref(hashmap->value.hashmap.table, index);
         entry_list != IMM_UNIT; entry_list = pair_cdr(entry_list)) {
        object found_key = pair_caar(entry_list);
        object found_value = pair_cdar(entry_list);
        if (found_key == key || equals(context, found_key, key)) {
            return found_value;
        }
    }

    gc_assert_no_gc(context);
    // 找不到节点
    return IMM_UNIT;
}

/**
 * hashmap_b 全部放入 hashmap_a, 浅拷贝
 * @param context
 * @param hashmap_a
 * @param hashmap_b
 */
EXPORT_API void
hashmap_put_all_op(REF NOTNULL context_t context, REF NOTNULL object hashmap_a, REF NOTNULL object hashmap_b) {
    assert(context != NULL);
    assert(hashmap_a != hashmap_b);
    assert(is_hashmap(hashmap_a));
    assert(is_hashmap(hashmap_b));

    gc_param2(context, hashmap_a, hashmap_b);
    gc_var4(context, entry_list, entry, key, value);

    size_t map_b_vector_len = vector_len(hashmap_b->value.hashmap.table);
    for (size_t i = 0; i < map_b_vector_len; i++) {

        entry_list = vector_ref(hashmap_b->value.hashmap.table, i);
        for (; entry_list != IMM_UNIT; entry_list = pair_cdr(entry_list)) {

            entry = pair_car(entry_list);
            key = pair_car(entry);
            value = pair_cdr(entry);

            hashmap_put_op(context, hashmap_a, key, value);
        }
    }

    gc_release_param(context);
}

/**
 * 清空 hashmap
 * <p>不会触发 GC</p>
 * @param context
 * @param hashmap
 */
EXPORT_API void hashmap_clear_op(REF NOTNULL context_t context, REF NOTNULL object hashmap) {
    assert(context != NULL);
    assert(is_hashmap(hashmap));
    gc_set_flag(context);

    object vector = hashmap->value.hashmap.table;
    size_t len = vector_len(vector);

    for (size_t i = 0; i < len; i++) {
        vector_set(vector, i, IMM_UNIT);
    }

    gc_assert_no_gc(context);
    hashmap->value.hashmap.size = 0;
}

/**
 * 从 hashmap 移除指定的 key
 * <p>不会触发 GC</p>
 * @param context
 * @param hashmap
 * @param key
 * @return 如果 key 已经存在, 返回被移除的 value, 否则返回 IMM_UNIT
 */
EXPORT_API OUT NOTNULL object
hashmap_remove_op(REF NOTNULL context_t context, REF NOTNULL object hashmap, REF NOTNULL object key) {
    assert(context != NULL);
    assert(is_hashmap(hashmap));
    assert(!is_null(key));
    gc_set_flag(context);

    // 空表
    if (hashmap->value.hashmap.size == 0) {
        return IMM_UNIT;
    }

    size_t map_vector_len = vector_len(hashmap->value.hashmap.table);
    size_t hash = 0;
    size_t index = 0;
    hash_code_fn hash_fn = object_hash_helper(context, key);
    equals_fn equals = object_equals_helper(context, key);
    assert(equals != NULL);
    if (hash_fn != NULL) {
        hash = hash_fn(context, key);
    }
    index = hash % map_vector_len;

    object entry_list = vector_ref(hashmap->value.hashmap.table, index);
    if (entry_list == IMM_UNIT) {
        return IMM_UNIT;
    }
    // 此时至少有一个 entry
    object pre_entry_list = NULL;
    do {
        object deleted_key = pair_caar(entry_list);
        object deleted_value = pair_cdar(entry_list);

        if (deleted_key == key || equals(context, deleted_key, key)) {
            if (pre_entry_list == NULL) {
                // entry_list 是第一个节点
                vector_ref(hashmap->value.hashmap.table, index) = pair_cdr(entry_list);
            } else {
                // entry_list 是第二个或以后节点
                pair_cdr(pre_entry_list) = pair_cdr(entry_list);
            }

            hashmap->value.hashmap.size--;
            return deleted_value;
        }

        pre_entry_list = entry_list;
        entry_list = pair_cdr(entry_list);
    } while (entry_list != IMM_UNIT);

    gc_assert_no_gc(context);
    // 找不到节点
    return IMM_UNIT;
}


/******************************************************************************
                                对象扩容 API
******************************************************************************/

/**
 * bytes 扩容, 深拷贝
 * @param context
 * @param bytes
 * @param add_size 增加的大小
 * @return 会返回新对象
 */
EXPORT_API OUT NOTNULL GC object
bytes_capacity_increase(REF NOTNULL context_t context,
                        NOTNULL IN object bytes, size_t add_size) {
    assert(context != NULL);
    assert(is_bytes(bytes));

    gc_param1(context, bytes);
    gc_var1(context, new_bytes);

    size_t new_size = bytes_capacity(bytes) + add_size;
    new_bytes = bytes_make_op(context, new_size);
    // 复制原始内容
    memcpy(bytes_data(new_bytes), bytes_data(bytes), bytes_capacity(bytes));

    gc_release_param(context);
    return new_bytes;
}

/**
 * string_buffer 扩容, 深拷贝
 * @param context
 * @param str_buffer
 * @param add_size 新增大小
 * @return 返回原始 string_buffer
 */
EXPORT_API OUT NOTNULL GC object
string_buffer_capacity_increase(REF NOTNULL context_t context,
                                NOTNULL IN object str_buffer, size_t add_size) {
    assert(context != NULL);
    assert(is_string_buffer(str_buffer));

    gc_param1(context, str_buffer);
    gc_var1(context, new_buffer);

    // 内部 bytes 对象扩容, 自动复制旧内容
    new_buffer = bytes_capacity_increase(context, string_buffer_bytes_obj(str_buffer), add_size);
    string_buffer_bytes_obj(str_buffer) = new_buffer;
    // 修正容量
    string_buffer_capacity(str_buffer) = bytes_capacity(new_buffer);

    gc_release_param(context);
    return str_buffer;
}

/**
 * vector 扩容, 深拷贝
 * @param context
 * @param vec
 * @param add_size
 * @return 会返回新 vector
 */
EXPORT_API OUT NOTNULL GC object
vector_capacity_increase(REF NOTNULL context_t context, NOTNULL IN object vec, size_t add_size) {
    assert(context != NULL);
    assert(is_vector(vec));

    gc_param1(context, vec);
    gc_var1(context, new_vector);

    size_t old_size = vector_len(vec);
    size_t new_size = old_size + add_size;
    new_vector = vector_make_op(context, new_size);
    for (size_t i = 0; i < old_size; i++) {
        vector_set(new_vector, i, vector_ref(vec, i));
    }
    for (size_t i = old_size; i < new_size; i++) {
        vector_set(new_vector, i, IMM_UNIT);
    }

    gc_release_param(context);
    return new_vector;
}

/**
 * stack 扩容, 深拷贝
 * @param context
 * @param stack
 * @param add_size
 * @return 会返回新 stack
 */
EXPORT_API OUT NOTNULL GC object
stack_capacity_increase(REF NOTNULL context_t context, NOTNULL IN object stack, size_t add_size) {
    assert(context != NULL);
    assert(is_stack(stack));

    gc_param1(context, stack);
    gc_var1(context, new_stack);

    size_t old_size = stack_capacity(stack);
    size_t new_size = old_size + add_size;
    size_t len = stack_len(stack);

    new_stack = stack_make_op(context, new_size);
    new_stack->value.stack.length = len;

    for (size_t i = 0; i < len; i++) {
        new_stack->value.stack.data[i] = stack->value.stack.data[i];
    }

    gc_release_param(context);
    return new_stack;
}

/**
 * stack 自动增长的入栈
 * <p>注意, push 后应当重新给原来的栈赋值</p>
 * @param context
 * @param stack
 * @param element
 * @param extern_growth_size 如果栈满, 会自动增长 extern_growth_size + 1 (填 0 则自动增长 1)
 * @return 可能返回新 stack
 */
EXPORT_API OUT NOTNULL GC object
stack_push_auto_increase(REF NOTNULL context_t context,
                         NOTNULL REF object stack, NOTNULL REF object element,
                         size_t extern_growth_size) {
    assert(context != NULL);
    assert(is_stack(stack));

    gc_param2(context, stack, element);

    if (stack_full(stack)) {
        stack = stack_capacity_increase(context, stack, extern_growth_size + 1);
    }
    int push_result = stack_push(stack, element);
    assert(push_result == 1);

    gc_release_param(context);
    return stack;
}


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
imm_char_to_string(REF NOTNULL context_t context, NOTNULL COPY object imm_char) {
    assert(context != NULL);
    assert(is_imm_char(imm_char));

    char buffer[2] = {char_imm_getvalue(imm_char), '\0'};
    return string_make_from_cstr_op(context, buffer);
}

/**
 * char 转 string
 * @param context
 * @param ch
 * @return string
 */
EXPORT_API OUT NOTNULL GC object
char_to_string(REF NOTNULL context_t context, COPY char ch) {
    assert(context != NULL);

    char buffer[2] = {ch, '\0'};
    return string_make_from_cstr_op(context, buffer);
}

/**
 * symbol 转 string
 * @param context
 * @param symbol
 * @return
 */
EXPORT_API OUT NOTNULL GC object
symbol_to_string(REF NOTNULL context_t context, NOTNULL COPY object symbol) {
    assert(context != NULL);
    assert(is_symbol(symbol));

    gc_param1(context, symbol);
    gc_var1(context, str);

    size_t len = symbol_len(symbol) + 1;
    str = raw_object_make(context, OBJ_STRING, object_sizeof_base(string) + sizeof(char) * len);
    str->value.string.len = len;
    memcpy(string_get_cstr(str), symbol_get_cstr(symbol), len);
    str->value.string.hash = string_hash_helper(str);

    gc_release_param(context);
    return str;
}

/**
 * string 转 symbol
 * @param context
 * @param string
 * @return
 */
EXPORT_API OUT NOTNULL GC object
string_to_symbol(REF NOTNULL context_t context, NOTNULL COPY object str) {
    assert(context != NULL);
    assert(is_string(str));

    gc_param1(context, str);
    gc_var1(context, symbol);

    size_t len = string_len(str) + 1;
    symbol = raw_object_make(context, OBJ_SYMBOL, object_sizeof_base(symbol) + sizeof(char) * len);
    symbol->value.symbol.len = len;
    memcpy(symbol_get_cstr(symbol), string_get_cstr(str), len);
    symbol->value.symbol.hash = symbol_hash_helper(symbol);

    gc_release_param(context);
    return symbol;
}

/**
 * string_buffer 转换为 string, 深拷贝
 * @param context
 * @param str_buffer string_buffer
 * @return string
 */
EXPORT_API OUT NOTNULL GC object
string_buffer_to_string(REF NOTNULL context_t context, NOTNULL COPY object str_buffer) {
    assert(context != 0);
    assert(is_string_buffer(str_buffer));

    gc_param1(context, str_buffer);
    gc_var1(context, str);

    // string 长度要比 string_buffer 长度多一个 '\0'
    size_t char_length = string_buffer_length(str_buffer);
    size_t size_with_null = char_length + 1;
    str = raw_object_make(context, OBJ_STRING,
                          object_sizeof_base(string) + size_with_null * sizeof(char));
    str->value.string.len = size_with_null;
    // string_buffer 末尾没有 '\0', 需要手动添加
    memcpy(string_get_cstr(str), string_buffer_bytes_data(str_buffer), char_length);
    string_get_cstr(str)[char_length] = '\0';
    str->value.string.hash = string_hash_helper(str);

    gc_release_param(context);
    return str;
}

/**
 * string_buffer 转换为 symbol, 深拷贝
 * @param context
 * @param str_buffer string_buffer
 * @return symbol
 */
EXPORT_API OUT NOTNULL GC object
string_buffer_to_symbol(REF NOTNULL context_t context, NOTNULL COPY object str_buffer) {
    assert(context != 0);
    assert(is_string_buffer(str_buffer));

    gc_param1(context, str_buffer);
    gc_var1(context, symbol);

    // string 长度要比 string_buffer 长度多一个 '\0'
    size_t char_length = string_buffer_length(str_buffer);
    size_t size_with_null = char_length + 1;
    symbol = raw_object_make(context, OBJ_SYMBOL,
                             object_sizeof_base(symbol) + size_with_null * sizeof(char));
    symbol->value.symbol.len = size_with_null;
    // string_buffer 末尾没有 '\0', 需要手动添加
    memcpy(symbol_get_cstr(symbol), string_buffer_bytes_data(str_buffer), char_length);
    string_get_cstr(symbol)[char_length] = '\0';
    symbol->value.symbol.hash = symbol_hash_helper(symbol);

    gc_release_param(context);
    return symbol;
}

/**
 * hashset 转为 vector, 无序
 * @param context
 * @param hashset
 * @return vector: #(key1, key2, ...)
 */
EXPORT_API OUT NOTNULL GC object
hashset_to_vector(REF NOTNULL context_t context, NOTNULL COPY object hashset) {
    assert(context != NULL);
    assert(is_hashset(hashset));

    gc_param1(context, hashset);
    gc_var2(context, vector, entry);

    vector = hashmap_to_vector(context, hashset->value.hashset.map);
    for (size_t i = 0; i < vector_len(vector); i++) {
        entry = vector_ref(vector, i);
        assert(is_pair(entry));
        // entry: (k, v)
        vector_ref(vector, i) = pair_car(entry);
    }

    gc_release_param(context);
    return vector;
}

/**
 * hashmap 转为 vector, 无序
 * @param context
 * @param hashmap
 * @return vector: #((k1, v1), (k2, v2), ...)
 */
EXPORT_API OUT NOTNULL GC object
hashmap_to_vector(REF NOTNULL context_t context, NOTNULL COPY object hashmap) {
    assert(context != NULL);
    assert(is_hashmap(hashmap));

    gc_param1(context, hashmap);
    gc_var5(context, vector, entry_list, entry, key, value);

    // map 中对象数量
    size_t map_size = hashmap_size(hashmap);
    if (map_size == 0) {
        // hashmap 大小为 0, 直接返回不再遍历
        return vector_make_op(context, 0);
    } else {
        vector = vector_make_op(context, map_size);
    }

    // vector 索引
    size_t vector_index = 0;
    size_t map_table_vector_len = vector_len(hashmap->value.hashmap.table);
    for (size_t i = 0; i < map_table_vector_len; i++) {

        entry_list = vector_ref(hashmap->value.hashmap.table, i);
        for (; entry_list != IMM_UNIT; entry_list = pair_cdr(entry_list)) {

            // 获取 hashmap 中的 entry
            entry = pair_car(entry_list);
            key = pair_car(entry);
            assert(is_symbol(key));
            value = pair_cdr(entry);

            // 填充 vector
            entry = pair_make_op(context, key, value);
            vector_ref(vector, vector_index) = entry;

            // 每找到一个 (k, v), 游标向后移动
            vector_index++;
        }
    }
    assert(map_size == vector_index);

    gc_release_param(context);
    return vector;
}