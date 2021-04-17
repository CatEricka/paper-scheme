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
    set_ext_type_tag_none(ret);
    set_mutable(ret);
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
 * <p>不要直接使用, 参见 interpreter.h: symbol_make_from_cstr_op()</p>
 * @param context
 * @param cstr C字符串, '\0'结尾
 * @return
 */
EXPORT_API OUT NOTNULL GC object
symbol_make_from_cstr_untracked_op(REF NOTNULL context_t context, COPY const char *cstr) {
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
string_make_from_cstr_op(REF NOTNULL context_t context, COPY const char *cstr) {
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
 * 构造 string 类型对象
 * @param context
 * @param count 字符数量
 * @param fill 填充
 * @return
 */
EXPORT_API OUT NOTNULL GC object
string_make_empty(REF NOTNULL context_t context, int64_t count, char fill) {
    assert(context != NULL);
    assert(count >= 0);
    size_t cstr_len = (size_t) (count + 1);

    object ret = raw_object_make(context, OBJ_STRING,
                                 object_sizeof_base(string) + sizeof(char) * cstr_len);
    ret->value.string.len = cstr_len;
    memset(string_get_cstr(ret), fill, cstr_len);
    string_get_cstr(ret)[cstr_len - 1] = '\0';
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
    // string_buffer 增长策略也许需要修改
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
string_port_input_from_string_op(REF NOTNULL context_t context, REF NULLABLE object str) {
    assert(context != NULL);
    assert(is_string(str));

    gc_param1(context, str);

    object ret = raw_object_make(context, OBJ_STRING_PORT, object_sizeof_base(string_port));
    ret->value.string_port.kind = PORT_INPUT;
    ret->value.string_port.string_buffer_data = str;
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
string_port_output_use_buffer_op(REF NOTNULL context_t context) {
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
 * 打开 input-output port, 深拷贝, 具有内部缓冲
 * <p>(open-input-output-string)</p>
 * @param context
 * @return 打开失败返回 IMM_UNIT, 否则返回值满足 is_srfi6_port(obj), is_string_port_in_out_put(obj)
 */
EXPORT_API OUT NOTNULL GC object
string_port_in_out_put_use_buffer_op(REF NOTNULL context_t context) {
    assert(context != NULL);

    gc_var1(context, str_buffer);

    object ret = raw_object_make(context, OBJ_STRING_PORT, object_sizeof_base(string_port));

    ret->value.string_port.kind = (unsigned) PORT_OUTPUT | (unsigned) PORT_INPUT | (unsigned) PORT_SRFI6;
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
string_port_in_out_put_from_string_use_buffer_op(REF NOTNULL context_t context, REF NULLABLE object str) {
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
stdio_port_from_filename_op(REF NOTNULL context_t context, REF NULLABLE object filename, IN enum port_kind kind) {
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
        // 注意释放
        gc_release_param(context);
        return IMM_UNIT;
    }
    port = raw_object_make(context, OBJ_STDIO_PORT, object_sizeof_base(stdio_port));

    port->value.stdio_port.kind = kind;
    port->value.stdio_port.file = file;
    port->value.stdio_port.filename = filename;
    port->value.stdio_port.is_released = 0;
    // 打开的文件需要关闭
    port->value.stdio_port.need_close = 1;
    port->value.stdio_port.current_line = 1;
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
stdio_port_from_file_op(REF NOTNULL context_t context, REF NOTNULL FILE *file, enum port_kind kind) {
    assert(context != NULL);

    gc_var2(context, port, filename);

    port = raw_object_make(context, OBJ_STDIO_PORT, object_sizeof_base(stdio_port));

    port->value.stdio_port.kind = kind;
    port->value.stdio_port.file = file;
    filename = string_make_from_cstr_op(context, "<unknown>");
    port->value.stdio_port.filename = filename;
    port->value.stdio_port.is_released = 0;
    // 外部打开的文件外部负责关闭
    port->value.stdio_port.need_close = 0;
    port->value.stdio_port.current_line = 1;
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
    assert(load_factor > 0);
    assert(load_factor < 1);

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
    assert(load_factor > 0);
    assert(load_factor < 1);

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

/**
 * 构造 weak_hashset
 * <p>弱引用 hashset</p>
 * @param context
 * @param init_capacity hashset 初始大小 (默认 DEFAULT_HASH_SET_MAP_INIT_CAPACITY)
 * @param load_factor 负载因子 (默认大小 DEFAULT_HASH_SET_MAP_LOAD_FACTOR)
 * @return
 */
EXPORT_API OUT NOTNULL GC object
weak_hashset_make_op(REF NOTNULL context_t context, IN size_t init_capacity, IN double load_factor) {
    assert(context != NULL);
    assert(load_factor > 0);

    gc_var2(context, weak_hashset, table);

    table = vector_make_op(context, init_capacity);
    weak_hashset = raw_object_make(context, OBJ_WEAK_HASH_SET, object_sizeof_base(weak_hashset));
    weak_hashset->value.weak_hashset.table = table;
    weak_hashset->value.weak_hashset.size = 0;
    weak_hashset->value.weak_hashset.load_factor = load_factor;
    weak_hashset->value.weak_hashset.threshold = init_capacity;
    weak_hashset->value.weak_hashset.hash = pointer_with_type_to_hash(weak_hashset, OBJ_WEAK_HASH_SET);

    gc_release_var(context);
    return weak_hashset;
}

/**
 * 构造 stack frame
 * @param context
 * @param op int
 * @param args pair
 * @param env env_slot
 * @param code pair
 * @return
 */
EXPORT_API OUT NOTNULL GC object
stack_frame_make_op(REF NOTNULL context_t context, enum opcode_e op, object args, object code, object env) {
    assert(context != NULL);
    assert(op >= 0);

    gc_param3(context, args, code, env);
    gc_var1(context, frame);

    frame = raw_object_make(context, OBJ_STACK_FRAME, object_sizeof_base(stack_frame));
    stack_frame_op(frame) = op;
    stack_frame_args(frame) = args;
    stack_frame_code(frame) = code;
    stack_frame_env(frame) = env;
    frame->value.stack_frame.hash = pointer_with_type_to_hash(frame, OBJ_STACK_FRAME);

    gc_release_param(context);
    return frame;
}

/**
 * 构造 environment slot
 * @param context
 * @param var symbol
 * @param value any
 * @param pre_env_frame env_slot
 * @return
 */
EXPORT_API OUT NOTNULL GC object
env_slot_make_op(REF NOTNULL context_t context, object var, object value, object pre_env_frame) {
    assert(context != NULL);
    assert(is_symbol(var));

    gc_param3(context, var, value, pre_env_frame);
    gc_var1(context, frame);

    frame = raw_object_make(context, OBJ_ENV_SLOT, object_sizeof_base(env_slot));
    env_slot_var(frame) = var;
    env_slot_value(frame) = value;
    env_slot_next(frame) = pre_env_frame;
    frame->value.env_slot.hash = pointer_with_type_to_hash(frame, OBJ_ENV_SLOT);

    gc_release_param(context);
    return frame;
}

/**
 * 构造 proc
 * @param context
 * @param symbol
 * @param opcode enum opcode_e
 * @return
 */
EXPORT_API OUT NOTNULL GC object
proc_make_internal(REF NOTNULL context_t context, object symbol, enum opcode_e opcode) {
    assert(context != NULL);
    assert(is_symbol(symbol));
    assert(opcode >= 0);

    gc_param1(context, symbol);
    gc_var1(context, proc);

    proc = raw_object_make(context, OBJ_PROC, object_sizeof_base(proc));
    proc_get_symbol(proc) = symbol;
    proc_get_opcode(proc) = opcode;
    proc->value.proc.hash = pointer_with_type_to_hash(proc, OBJ_PROC);

    gc_release_param(context);
    return proc;
}

/**
 * 构造 syntax
 * @param context
 * @param symbol 关键字名
 * @param opcode opcode
 * @return
 */
EXPORT_API OUT NOTNULL GC object
syntax_make_internal(REF NOTNULL context_t context, object symbol, enum opcode_e opcode) {
    assert(context != NULL);
    assert(is_symbol(symbol));

    gc_param1(context, symbol);
    gc_var1(context, syntax);

    syntax = raw_object_make(context, OBJ_SYNTAX, object_sizeof_base(syntax));
    syntax_get_symbol(syntax) = symbol;
    syntax_get_opcode(syntax) = opcode;
    syntax->value.syntax.hash = symbol_hash_code(context, symbol);

    gc_release_param(context);
    return syntax;
}

/**
 * 构造 promise
 * @param context
 * @param value
 * @param env
 * @return promise
 */
EXPORT_API OUT NOTNULL GC object
promise_make_op(REF NOTNULL context_t context, object value) {
    assert(context != NULL);

    gc_param1(context, value);
    gc_var1(context, promise);

    promise = raw_object_make(context, OBJ_PROMISE, object_sizeof_base(syntax));
    promise_forced(promise) = 0;
    promise_get_value(promise) = value;
    promise->value.promise.hash = pointer_with_type_to_hash(promise, OBJ_PROMISE);

    gc_release_param(context);
    return promise;
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
        string_buffer_capacity_increase_op(context, str_buffer, add_size);

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
 * string_buffer 拼接 c_str, 深拷贝
 * @param context
 * @param str_buffer string_buffer
 * @param str string
 * @return 原始 string_buffer
 */
EXPORT_API OUT NOTNULL GC object
string_buffer_append_cstr_op(
        REF NOTNULL context_t context,
        IN NULLABLE object str_buffer, COPY NULLABLE const char *cstr) {

    assert(context != NULL);
    assert(is_string_buffer(str_buffer));

    gc_param1(context, str_buffer);
    gc_var1(context, tmp);

    if (cstr == NULL) {
        gc_release_param(context);
        return str_buffer;
    }

    size_t str_buffer_len = string_buffer_length(str_buffer);
    size_t str_len = strlen(cstr);
    size_t buffer_size = string_buffer_capacity(str_buffer);

    assert(cstr[str_len] == '\0');

    if (str_len + str_buffer_len > buffer_size) {
        // 容量不足
        size_t add_size = (str_len + str_buffer_len + STRING_BUFFER_DEFAULT_GROWTH_SIZE) - buffer_size;
        string_buffer_capacity_increase_op(context, str_buffer, add_size);

        assert(add_size + buffer_size == string_buffer_capacity(str_buffer));
    }

    // 不复制 '\0'
    memcpy(string_buffer_bytes_data(str_buffer) + str_buffer_len, cstr, str_len);
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
    string_buffer_append_char_op(context, str_buffer, char_imm_getvalue(imm_char));
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
string_buffer_append_char_op(REF NOTNULL context_t context, IN NOTNULL object str_buffer, COPY char ch) {
    assert(context != NULL);
    assert(ch != '\0');

    gc_param1(context, str_buffer);

    size_t buffer_len = string_buffer_length(str_buffer);
    if (buffer_len + 1 > string_buffer_capacity(str_buffer)) {
        // 当前 string_buffer_length(str_buffer) + 1, 其中 '+1' 为 新增加的 char
        string_buffer_capacity_increase_op(context, str_buffer, STRING_BUFFER_DEFAULT_GROWTH_SIZE + 1);
    }

    // char 拼接
    string_buffer_bytes_data(str_buffer)[buffer_len] = ch;
    // string_buffer 长度 +1
    string_buffer_length(str_buffer)++;

    gc_release_param(context);
    return str_buffer;
}

/**
 * vector 填充
 * @param vector
 * @param obj 被填充, 浅拷贝
 */
EXPORT_API OUT void
vector_fill(REF NOTNULL object vector, REF NOTNULL object obj) {
    assert(vector != NULL);
    assert(is_vector(vector));

    size_t l = vector_len(vector);
    for (size_t i = 0; i < l; i++) {
        vector_set(vector, i, obj);
    }
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
    gc_set_no_gc_assert_flag(context);

    object ret = hashmap_contains_key_op(context, hashset->value.hashset.map, obj);

    assert(is_imm_true(ret) || is_imm_false(ret));
    gc_assert_no_gc(context);
    return ret;
}

/**
 * obj 放入 hashset
 * @param context
 * @param obj
 * @return 如果已经存在旧值, 则返回旧值, 否则返回刚刚放入的值
 */
EXPORT_API GC object
hashset_put_op(REF NOTNULL context_t context, REF NOTNULL object hashset, REF NOTNULL object obj) {
    assert(context != NULL);
    assert(is_hashset(hashset));
    assert(!is_null(obj));

    gc_param2(context, hashset, obj);
    gc_var1(context, ret);

    // 所有的 key 对应的 value 都是 IMM_TRUE
    if (is_imm_true(hashmap_contains_key_op(context, hashset->value.hashset.map, obj))) {
        ret = hashmap_get_op(context, hashset->value.hashset.map, obj);
    } else {
        hashmap_put_op(context, hashset->value.hashset.map, obj, obj);
        ret = obj;
    }

    gc_release_param(context);
    return ret;
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
    gc_set_no_gc_assert_flag(context);

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
    gc_set_no_gc_assert_flag(context);

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
    gc_set_no_gc_assert_flag(context);

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
    assert(is_hashmap(hashmap));
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

            // 注意释放 C 调用栈保护链
            gc_release_param(context);
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
    gc_set_no_gc_assert_flag(context);

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
EXPORT_API GC void
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
    gc_set_no_gc_assert_flag(context);

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
    gc_set_no_gc_assert_flag(context);

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


/**
 * 清除 weak_hashset 中指定 table[index] 中失效的弱引用对象
 * <p>不会触发 GC</p>
 * @param context
 * @param weak_hashset
 */
static void weak_hashset_expunge_stale_ref(context_t context, object weak_hashset, size_t index) {
    assert(context != NULL);
    assert(is_weak_hashset(weak_hashset));
    gc_set_no_gc_assert_flag(context);

    object table = weak_hashset->value.weak_hashset.table;
    assert(index < vector_len(table));

    if (vector_ref(table, index) == IMM_UNIT) {
        return;
    }

    object pre, next;
    pre = IMM_UNIT;
    next = vector_ref(table, index);

    while (next != IMM_UNIT) {
        assert(is_pair(next));
        object value = pair_car(next);
        assert(is_weak_ref(value));

        if (!weak_ref_is_valid(value)) {
            if (pre == IMM_UNIT) {
                vector_ref(table, index) = pair_cdr(next);
                next = pair_cdr(next);
            } else {
                pair_cdr(pre) = pair_cdr(next);
                next = pair_cdr(next);
            }

            // 移除失效对象
            weak_hashset->value.weak_hashset.size--;
        } else {
            pre = next;
            next = pair_cdr(next);
        }
    }

    gc_assert_no_gc(context);
}

/**
 * weak_hashset 是否包含指定的对象
 * <p>清除部分无效引用</p>
 * <p>不会触发 GC</p>
 * @param context
 * @param weak_hashset
 * @param obj object 不能为 NULL
 * @return IMM_TRUE / IMM_FALSE
 */
EXPORT_API OUT NOTNULL object
weak_hashset_contains_op(REF NOTNULL context_t context, REF NOTNULL object weak_hashset, REF NOTNULL object obj) {
    assert(context != NULL);
    assert(is_weak_hashset(weak_hashset));
    assert(!is_null(obj));
    gc_set_no_gc_assert_flag(context);

    size_t hash = 0;
    size_t index = 0;
    size_t length = vector_len(weak_hashset->value.weak_hashset.table);

    equals_fn equals = object_equals_helper(context, obj);
    hash_code_fn hash_fn = object_hash_helper(context, obj);

    assert(equals != NULL);
    if (hash_fn != NULL) {
        hash = hash_fn(context, obj);
    }

    index = hash % length;
    weak_hashset_expunge_stale_ref(context, weak_hashset, index);

    object table = weak_hashset->value.weak_hashset.table;
    for (object entry_list = vector_ref(table, index);
         entry_list != IMM_UNIT;
         entry_list = pair_cdr(entry_list)) {

        assert(is_pair(entry_list));
        object ref = pair_car(entry_list);
        assert(is_weak_ref(ref));

        if (weak_ref_is_valid(ref)) {
            return equals(context, obj, weak_ref_get(ref)) ? IMM_TRUE : IMM_FALSE;
        }
    }

    // 找不到值
    gc_assert_no_gc(context);
    return IMM_FALSE;
}

/**
 * obj 放入 weak_hashset
 * <p>清除部分无效引用</p>
 * @param context
 * @param obj
 * @return 添加后的 object, 如果存在则返回原始 object
 */
EXPORT_API GC object
weak_hashset_put_op(REF NOTNULL context_t context, REF NOTNULL object weak_hashset, REF NOTNULL object obj) {
    assert(context != NULL);
    assert(is_weak_hashset(weak_hashset));
    assert(!is_null(obj));

    gc_param2(context, weak_hashset, obj);
    gc_var7(context, table, new_table, entry_list, new_entry, new_weak_ref, ref, old_object);

    hash_code_fn hash_fn = object_hash_helper(context, obj);
    equals_fn equals = object_equals_helper(context, obj);
    assert(equals != NULL);

    // 预先构造 可能插入的弱引用
    // GC point
    new_weak_ref = weak_ref_make_op(context, IMM_UNIT);
    new_entry = pair_make_op(context, IMM_UNIT, IMM_UNIT);

    // 1. 计算 hash 值
    uint32_t hash = 0;

    // hash 函数为空, hash 值为 0
    if (hash_fn != NULL) {
        hash = hash_fn(context, obj);
    }

    // 2. 根据 hash 值计算索引位置
    table = weak_hashset->value.hashmap.table;
    size_t vector_length = vector_len(table);
    size_t index = hash % vector_length;
    //  清理 index 位置的空值, 不考虑其他位置, contains() 函数自然会清理
    weak_hashset_expunge_stale_ref(context, weak_hashset, index);

    // 3. 检查索引位置是否有旧值, 如果存在则返回旧值
    for (entry_list = vector_ref(table, index);
         entry_list != IMM_UNIT;
         entry_list = pair_cdr(entry_list)) {

        assert(is_pair(entry_list));
        ref = pair_car(entry_list);
        assert(is_weak_ref(ref));

        // 检查是否存在
        if (weak_ref_is_valid(ref)) {
            old_object = weak_ref_get(ref);
            assert(!is_null(old_object));

            if (equals(context, obj, old_object)) {
                // 找到已经存在的对象, 直接返回
                // 注意释放 C 调用栈保护链
                gc_release_param(context);
                return old_object;
            }
        }
    }


    // 4. 没有找到旧值, 说明要创建新的 entry, 检查是否需要扩容
    //  超过临界容量且遇到 hash 冲突时进行扩容
    if (weak_hashset->value.weak_hashset.size >= weak_hashset->value.weak_hashset.threshold &&
        (vector_ref(table, index) != IMM_UNIT)) {

        // 此时容量大于等于扩容阈值, 且 index 位置不为 IMM_UNIT
        // 则需要扩容
        size_t new_length = vector_length * 2;
        // GC point
        new_table = vector_make_op(context, new_length);

        // 清理 GC 后可能出现的无效值
        for (size_t i = 0; i < vector_length; i++) {
            weak_hashset_expunge_stale_ref(context, weak_hashset, i);
        }

        // 转移旧的内容, 需要重新计算 index
        for (size_t i = 0; i < vector_length; i++) {

            // 取出旧的 entry
            entry_list = vector_ref(table, i);

            if (entry_list != IMM_UNIT) {
                assert(is_pair(entry_list));
                // 不为空的时候需要进行 entry 重新 hash 并转移
                // 否则不需要处理, 因为 vector 默认初始化为 IMM_UNIT
                ref = pair_car(entry_list);
                assert(weak_ref_is_valid(ref));
                old_object = weak_ref_get(ref);
                assert(!is_null(old_object));

                size_t tmp_hash = 0;
                hash_code_fn tmp_hash_fn = object_hash_helper(context, old_object);
                if (tmp_hash_fn != NULL) {
                    tmp_hash = tmp_hash_fn(context, old_object);
                }
                size_t new_index = tmp_hash % new_length;

                // 旧的 entry_list 的 hash 值应该都是相同的
                vector_ref(new_table, new_index) = entry_list;
            }
        } // 扩容结束

        // 修改 table
        weak_hashset->value.weak_hashset.table = new_table;
        weak_hashset->value.weak_hashset.threshold =
                (size_t) (vector_len(new_table) * weak_hashset->value.weak_hashset.load_factor);

        // 重新计算 hash 值
        hash = 0;
        if (hash_fn != NULL) {
            hash = hash_fn(context, obj);
        }
        index = hash % new_length;

        // 最后插入新节点
        entry_list = vector_ref(new_table, index);
        weak_ref_get(new_weak_ref) = obj;

        pair_car(new_entry) = new_weak_ref;
        pair_cdr(new_entry) = entry_list;
        vector_set(new_table, index, new_entry);

    } else {

        // 5. 否则不需要扩容, 插入新节点
        entry_list = vector_ref(table, index);
        weak_ref_get(new_weak_ref) = obj;

        pair_car(new_entry) = new_weak_ref;
        pair_cdr(new_entry) = entry_list;
        vector_set(table, index, new_entry);
    }

    // 此时插入过新的键值对, 返回 IMM_UNIT
    weak_hashset->value.weak_hashset.size++;
    gc_release_param(context);
    return obj;
}

/**
 * 清空 weak_hashset
 * <p>不会触发 GC</p>
 * @param context
 * @param weak_hashset 不能为空
 * @return
 */
EXPORT_API void weak_hashset_clear_op(REF NOTNULL context_t context, REF NOTNULL object weak_hashset) {
    assert(context != NULL);
    assert(is_weak_hashset(weak_hashset));
    gc_set_no_gc_assert_flag(context);

    object vector = weak_hashset->value.weak_hashset.table;
    size_t len = vector_len(vector);

    for (size_t i = 0; i < len; i++) {
        vector_set(vector, i, IMM_UNIT);
    }

    gc_assert_no_gc(context);
    weak_hashset->value.weak_hashset.size = 0;

    gc_assert_no_gc(context);
}

/**
 * 从 weak_hashset 中移除 object
 * <p>清除部分无效引用</p>
 * <p>不会触发 GC</p>
 * @param context
 * @param weak_hashset
 * @param obj 不能为空, 可以为 IMM_UNIT
 */
EXPORT_API void
weak_hashset_remove_op(REF NOTNULL context_t context, REF NOTNULL object weak_hashset, REF NOTNULL object obj) {
    assert(context != NULL);
    assert(is_weak_hashset(weak_hashset));
    assert(!is_null(obj));
    gc_set_no_gc_assert_flag(context);

    // 空表
    if (weak_hashset->value.weak_hashset.size == 0) {
        return;
    }

    object table = weak_hashset->value.weak_hashset.table;
    size_t vector_length = vector_len(table);
    size_t hash = 0;
    size_t index = 0;
    hash_code_fn hash_fn = object_hash_helper(context, obj);
    equals_fn equals = object_equals_helper(context, obj);
    assert(equals != NULL);
    if (hash_fn != NULL) {
        hash = hash_fn(context, obj);
    }
    index = hash % vector_length;

    if (vector_ref(table, index) == IMM_UNIT) {
        return;
    }

    // 此时至少有一个 entry
    object pre, next;
    pre = IMM_UNIT;
    next = vector_ref(table, index);

    while (next != IMM_UNIT) {
        assert(is_pair(next));
        object ref = pair_car(next);
        assert(is_weak_ref(ref));

        if (!weak_ref_is_valid(ref)) {
            // 移除无效弱引用
            if (pre == IMM_UNIT) {
                vector_ref(table, index) = pair_cdr(next);
            } else {
                pair_cdr(pre) = pair_cdr(next);
            }
            next = pair_cdr(next);
            weak_hashset->value.weak_hashset.size--;
        } else {
            // 有效弱引用, 检查是否是需要被移除的对象
            assert(weak_ref_is_valid(ref));
            object old_object = weak_ref_get(ref);

            if (equals(context, obj, old_object)) {
                // 找到对象则删除并返回
                if (pre == IMM_UNIT) {
                    vector_ref(table, index) = pair_cdr(next);
                } else {
                    pair_cdr(pre) = pair_cdr(next);
                }
                weak_hashset->value.weak_hashset.size--;
                // 查找结束
                return;
            } else {
                // 没找到, 跳到下一个对象
                pre = next;
                next = pair_cdr(next);
            }
        } // 无效弱引用判断
    }

    gc_assert_no_gc(context);
}

/**
 * 返回 weak_hashset 元素数量
 * <p>自动清除全部无效引用</p>
 * <p>不会触发 GC</p>
 * @param context
 * @param weak_hashset
 * @return 元素数量
 */
EXPORT_API size_t
weak_hashset_size_op(REF NOTNULL context_t context, REF NOTNULL object weak_hashset) {
    gc_set_no_gc_assert_flag(context);

    for (size_t i = 0; i < vector_len(weak_hashset->value.weak_hashset.table); i++) {
        weak_hashset_expunge_stale_ref(context, weak_hashset, i);
    }

    gc_assert_no_gc(context);
    return weak_hashset->value.weak_hashset.size;
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
bytes_capacity_increase_op(REF NOTNULL context_t context,
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
 */
EXPORT_API OUT NOTNULL GC void
string_buffer_capacity_increase_op(REF NOTNULL context_t context,
                                   NOTNULL IN object str_buffer, size_t add_size) {
    assert(context != NULL);
    assert(is_string_buffer(str_buffer));

    gc_param1(context, str_buffer);
    gc_var1(context, new_buffer);

    // 内部 bytes 对象扩容, 自动复制旧内容
    new_buffer = bytes_capacity_increase_op(context, string_buffer_bytes_obj(str_buffer), add_size);
    string_buffer_bytes_obj(str_buffer) = new_buffer;
    // 修正容量
    string_buffer_capacity(str_buffer) = bytes_capacity(new_buffer);

    gc_release_param(context);
}

/**
 * vector 扩容, 深拷贝
 * @param context
 * @param vec
 * @param add_size
 * @return 会返回新 vector
 */
EXPORT_API OUT NOTNULL GC object
vector_capacity_increase_op(REF NOTNULL context_t context, NOTNULL IN object vec, size_t add_size) {
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
stack_capacity_increase_op(REF NOTNULL context_t context, NOTNULL IN object stack, size_t add_size) {
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
stack_push_auto_increase_op(REF NOTNULL context_t context,
                            NOTNULL REF object stack, NOTNULL REF object element,
                            size_t extern_growth_size) {
    assert(context != NULL);
    assert(is_stack(stack));

    gc_param2(context, stack, element);

    if (stack_full(stack)) {
        stack = stack_capacity_increase_op(context, stack, extern_growth_size + 1);
    }
    int push_result = stack_push_op(stack, element);
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
imm_char_to_string_op(REF NOTNULL context_t context, NOTNULL COPY object imm_char) {
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
char_to_string_op(REF NOTNULL context_t context, COPY char ch) {
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
symbol_to_string_op(REF NOTNULL context_t context, NOTNULL COPY object symbol) {
    assert(context != NULL);
    assert(is_symbol(symbol));

    gc_param1(context, symbol);
    gc_var1(context, str);

    size_t len = symbol_len(symbol) + 1;
    str = raw_object_make(context, OBJ_STRING, object_sizeof_base(string) + sizeof(char) * len);
    str->value.string.len = len;
    memcpy(string_get_cstr(str), symbol_get_cstr(symbol), len);
    str->value.string.hash = string_hash_helper(str);
    set_immutable(str);

    gc_release_param(context);
    return str;
}


/**
 * string 转 symbol
 * <p>不要直接使用, 参见 interpreter.h: string_to_symbol_op()</p>
 * @param context
 * @param string
 * @return
 */
EXPORT_API OUT NOTNULL GC object
string_to_symbol_untracked_op(REF NOTNULL context_t context, NOTNULL COPY object str) {
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
string_buffer_to_string_op(REF NOTNULL context_t context, NOTNULL COPY object str_buffer) {
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
 * <p>不要直接使用, 参见 interpreter.h: string_buffer_to_symbol_op()</p>
 * @param context
 * @param str_buffer string_buffer
 * @return symbol
 */
EXPORT_API OUT NOTNULL GC object
string_buffer_to_symbol_untracked_op(REF NOTNULL context_t context, NOTNULL COPY object str_buffer) {
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
hashset_to_vector_op(REF NOTNULL context_t context, NOTNULL COPY object hashset) {
    assert(context != NULL);
    assert(is_hashset(hashset));

    gc_param1(context, hashset);
    gc_var2(context, vector, entry);

    vector = hashmap_to_vector_op(context, hashset->value.hashset.map);
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
hashmap_to_vector_op(REF NOTNULL context_t context, NOTNULL COPY object hashmap) {
    assert(context != NULL);
    assert(is_hashmap(hashmap));

    gc_param1(context, hashmap);
    gc_var5(context, vector, entry_list, entry, key, value);

    // map 中对象数量
    size_t map_size = hashmap_size(hashmap);
    if (map_size == 0) {
        // hashmap 大小为 0, 直接返回不再遍历
        // 注意释放
        gc_release_param(context);
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


/**
 * weak_hashset 转为 vector, 无序
 * <p>返回的 vector 中可能包含 IMM_UNIT</p>
 * @param context
 * @param weak_hashset
 * @return vector: #(v1, v2, ... IMM_UNIT, ...)
 */
EXPORT_API OUT NOTNULL GC object
weak_hashset_to_vector_op(REF NOTNULL context_t context, NOTNULL COPY object weak_hashset) {
    assert(context != NULL);
    assert(is_weak_hashset(weak_hashset));

    gc_param1(context, weak_hashset);
    gc_var5(context, vector, entry_list, entry, ref, obj);

    // map 中对象数量
    size_t set_size = weak_hashset_size_op(context, weak_hashset);
    if (set_size == 0) {
        // hashmap 大小为 0, 直接返回不再遍历
        // 注意释放
        gc_release_param(context);
        return vector_make_op(context, 0);
    } else {
        vector = vector_make_op(context, set_size);
    }

    // vector 索引
    size_t vector_index = 0;
    size_t map_table_vector_len = vector_len(weak_hashset->value.weak_hashset.table);
    for (size_t i = 0; i < map_table_vector_len; i++) {

        entry_list = vector_ref(weak_hashset->value.weak_hashset.table, i);
        for (; entry_list != IMM_UNIT; entry_list = pair_cdr(entry_list)) {

            // 获取 hashmap 中的 entry
            ref = pair_car(entry_list);
            assert(is_weak_ref(ref));

            if (weak_ref_is_valid(ref)) {
                obj = weak_ref_get(ref);
                vector_set(vector, vector_index, obj);
                // 每找到一个有效对象, 游标向后移动
                vector_index++;
            }
        }
    }
    assert(vector_index <= set_size);

    gc_release_param(context);
    return vector;
}

/******************************************************************************
                             输入输出 API
******************************************************************************/
/**
 * 从 port 读入一个 char
 * <p>不会触发 GC</p>
 * @param port
 * @return IMM_CHAR 或 IMM_EOF
 */
EXPORT_API object port_get_char(REF NOTNULL object port) {
    assert(is_port(port));
    assert(is_port_input(port));

    object ch;

    if (is_port_eof(port)) {
        ch = IMM_EOF;
    } else if (is_stdio_port(port)) {
        // FILE port

        int raw_ch = fgetc(port->value.stdio_port.file);
        if (raw_ch == EOF) {
            port->value.stdio_port.kind |= PORT_EOF;
            ch = IMM_EOF;
        } else {
            ch = char_imm_make((char) raw_ch);
        }
    } else if (is_srfi6_port(port)) {
        // string_buffer port
        assert(is_string_buffer(port->value.string_port.string_buffer_data));

        object str_buff = port->value.string_port.string_buffer_data;
        if (port->value.string_port.current < string_buffer_length(str_buff)) {
            ch = char_imm_make(string_buffer_index(str_buff, port->value.string_port.current));
            port->value.string_port.current++;
        } else {
            port->value.stdio_port.kind |= PORT_EOF;
            ch = IMM_EOF;
        }
    } else {
        // string port
        assert(is_string(port->value.string_port.string_buffer_data));

        object str = port->value.string_port.string_buffer_data;
        if (port->value.string_port.current < string_len(str)) {
            ch = char_imm_make(string_index(str, port->value.string_port.current));
            port->value.string_port.current++;
        } else {
            port->value.stdio_port.kind |= PORT_EOF;
            ch = IMM_EOF;
        }
    }

    return ch;
}
/**
 * char 重新放回 port
 * <p>不会触发 GC</p>
 * @param port PORT_INPUT
 * @param ch IMM_CHAR / IMM_EOF, 后者不做任何事
 */
EXPORT_API void port_unget_char(REF NOTNULL object port, object ch) {
    assert(is_port(port));
    assert(is_imm_char(ch) || is_imm_eof(ch));

    if (ch == IMM_EOF) {
        return;
    }

    if (is_stdio_port(port)) {
        ungetc(char_imm_getvalue(ch), port->value.stdio_port.file);
    } else {
        if (port->value.string_port.current != 0) {
            // 忽略 ch 值
            port->value.string_port.current--;
        }
    }

    // 如果字符被退回, 则必定不是 EOF
    if (is_port_eof(port)) {
        if (is_stdio_port_eof(port)) {
            port->value.stdio_port.kind ^= PORT_EOF;
        } else {
            port->value.string_port.kind ^= PORT_EOF;
        }
    }
}

/**
 * 向 port 输出 char
 * @param context
 * @param port PORT_OUTPUT
 * @param ch IMM_CHAR
 */
EXPORT_API GC void
port_put_char(REF NOTNULL context_t context, REF NOTNULL object port, COPY object ch) {
    assert(context != NULL);
    assert(port != NULL);
    assert(is_port_output(port));
    assert(is_imm_char(ch));

    gc_param1(context, port);
    gc_var1(context, str_buff);

    if (is_stdio_port(port)) {
        fputc(char_imm_getvalue(ch), port->value.stdio_port.file);
    } else if (is_srfi6_port(port)) {
        str_buff = port->value.string_port.string_buffer_data;

        size_t current = port->value.string_port.current;
        size_t str_buff_len = string_buffer_length(str_buff);

        if (current < str_buff_len) {
            string_buffer_index(str_buff, current) = char_imm_getvalue(ch);
            port->value.string_port.current++;
        } else {
            assert(current == str_buff_len);

            string_buffer_append_imm_char_op(context, str_buff, ch);
            port->value.string_port.current = string_buffer_length(str_buff);
            port->value.string_port.string_buffer_data = str_buff;
        }
    } else {
        assert((0 && "UNKNOWN TYPE PORT"));
    }

    gc_release_param(context);
}
/**
 * 向 port 输出 c str
 * @param context
 * @param port PORT_OUTPUT
 * @param cstr
 */
EXPORT_API GC void
port_put_cstr(REF NOTNULL context_t context, REF NOTNULL object port, COPY const char *cstr) {
    assert(context != NULL);
    assert(port != NULL);
    assert(is_port_output(port));

    gc_param1(context, port);
    gc_var1(context, str_buff);

    if (cstr == NULL) {
        gc_release_param(context);
        return;
    }

    if (is_stdio_port(port)) {
        fputs(cstr, port->value.stdio_port.file);
    } else if (is_srfi6_port(port)) {

        str_buff = port->value.string_port.string_buffer_data;

        size_t current = port->value.string_port.current;
        size_t str_buff_len = string_buffer_length(str_buff);
        size_t cstr_len = strlen(cstr);

        if (cstr_len == 0) {
            gc_release_param(context);
            return;
        }

        if (current + cstr_len <= str_buff_len) {
            for (size_t i = 0; i < cstr_len; i++) {
                assert(current + i <= str_buff_len);
                string_buffer_index(str_buff, current + i) = cstr[i];
            }
            port->value.string_port.current += cstr_len;
        } else {
            assert(current == str_buff_len);

            string_buffer_append_cstr_op(context, str_buff, cstr);
            port->value.string_port.current = string_buffer_length(str_buff);
            port->value.string_port.string_buffer_data = str_buff;
        }
    } else {
        assert((0 && "UNKNOWN TYPE PORT"));
    }

    gc_release_param(context);
}
/**
 * 向 port 输出 string
 * @param context
 * @param port PORT_OUTPUT
 * @param string
 */
EXPORT_API GC void
port_put_string(REF NOTNULL context_t context, REF NOTNULL object port, COPY object string) {
    assert(context != NULL);
    assert(port != NULL);
    assert(is_port_output(port));

    gc_param2(context, port, string);
    gc_var1(context, str_buff);

    if (string_len(string) == 0) {
        gc_release_param(context);
        return;
    }

    if (is_stdio_port(port)) {
        fputs(string_get_cstr(string), port->value.stdio_port.file);
    } else if (is_srfi6_port(port)) {

        str_buff = port->value.string_port.string_buffer_data;

        size_t current = port->value.string_port.current;
        size_t str_buff_len = string_buffer_length(str_buff);
        size_t str_len = string_len(string);

        if (str_len == 0) {
            gc_release_param(context);
            return;
        }

        if (current + str_len <= str_buff_len) {
            for (size_t i = 0; i < str_len; i++) {
                assert(current + i <= str_buff_len);
                string_buffer_index(str_buff, current + i) = string_index(string, i);
            }
            port->value.string_port.current += str_len;
        } else {
            // current + str_len > str_buff_len

            string_buffer_append_string_op(context, str_buff, string);
            port->value.string_port.current = string_buffer_length(str_buff);
            port->value.string_port.string_buffer_data = str_buff;
        }
    } else {
        assert((0 && "UNKNOWN TYPE PORT"));
    }

    gc_release_param(context);
}


/**
 * port 定位
 * <p>不会触发 GC</p>
 * @param port
 * @param offset 偏移量
 * @param origin 起始位置: 0, 起始; 1, 当前位置; 2, 结束位置
 * @return IMM_TRUE / IMM_FALSE 移动是否成功
 */
EXPORT_API object port_seek(REF NOTNULL object port, long offset, int origin) {
    if (is_stdio_port(port)) {
        switch (origin) {
            case 1:
                fseek(port->value.stdio_port.file, offset, SEEK_CUR);
                break;
            case 2:
                fseek(port->value.stdio_port.file, offset, SEEK_END);
                break;
            case 0:
            default:
                fseek(port->value.stdio_port.file, offset, SEEK_SET);
                break;
        }
    } else {
        uint64_t end = 0;
        uint64_t current = port->value.string_port.current;
        if (is_srfi6_port(port)) {
            end = string_buffer_length(port->value.string_port.string_buffer_data);
        } else {
            end = string_len(port->value.string_port.string_buffer_data);
        }
        switch (origin) {
            case 1:
                if (current + offset >= 0 && current + offset <= end) {
                    port->value.string_port.current += offset;
                } else return IMM_FALSE;
                break;
            case 2:
                if (end + offset >= 0 && end + offset <= end) {
                    port->value.string_port.current = (size_t) (end + offset);
                } else return IMM_FALSE;
                break;
            case 0:
            default:
                if (offset >= 0 && offset <= end) {
                    port->value.string_port.current = offset;
                } else return IMM_FALSE;
                break;
        }
    }

    // fseek 结束, 去掉 EOF 标志
    // 如果字符被退回, 则必定不是 EOF
    if (is_port_eof(port)) {
        if (is_stdio_port_eof(port)) {
            port->value.stdio_port.kind ^= PORT_EOF;
        } else {
            port->value.string_port.kind ^= PORT_EOF;
        }
    }

    return IMM_TRUE;
}


/**
 * 返回当前 port 位置
 * <p>不会触发 GC</p>
 * @param port
 * @return port 当前游标位置
 */
EXPORT_API size_t port_tail(REF NOTNULL object port) {
    if (is_stdio_port(port)) {
        return ftell(port->value.stdio_port.file);
    } else {
        return port->value.string_port.current;
    }
}

/******************************************************************************
                              list 操作 API
******************************************************************************/
/**
 * <p>list 原地逆序</p>
 * <p>不会触发 GC</p>
 * @param context
 * @param term 结束符号
 * @param list
 * @return list
 */
EXPORT_API object reverse_in_place(context_t context, object term, object list) {
    assert(context != NULL);
    object p = list, result = term, q;

    while (p != IMM_UNIT) {
        q = pair_cdr(p);
        pair_cdr(p) = result;
        result = p;
        p = q;
    }
    return result;
}

/**
 * list 逆序, 创建新的 pair
 * @param context
 * @param list
 * @return
 */
EXPORT_API GC object reverse(context_t context, object list) {
    assert(context != NULL);
    gc_param1(context, list);
    gc_var1(context, p);
    p = IMM_UNIT;

    for (; is_pair(list); list = pair_cdr(list)) {
        p = pair_make_op(context, pair_car(list), p);
    }

    gc_release_param(context);
    return p;
}

/**
 * list*
 * <p>返回不以 IMM_UNIT 结尾的 list</p>
 * <p>换句话说去掉原始 list 结尾的 IMM_UNIT</p>
 * @param context
 * @param list
 * @return
 */
EXPORT_API GC object list_star(context_t context, object list) {
    assert(is_pair(list));
    gc_param1(context, list);
    gc_var2(context, cur, list_head);

    if (pair_cdr(list) == IMM_UNIT) {
        gc_release_param(context);
        return pair_cdr(list);
    }

    list_head = pair_make_op(context, pair_car(list), pair_cdr(list));
    cur = list_head;
    while (pair_cddr(cur) != IMM_UNIT) {
        list = pair_make_op(context, pair_car(cur), pair_cdr(cur));
        if (pair_cddr(cur) != IMM_UNIT) {
            cur = pair_cdr(list);
        }
    }

    pair_cdr(cur) = pair_cadr(cur);
    gc_release_param(context);
    return list_head;
}


/******************************************************************************
                             运行时类型构造
******************************************************************************/
EXPORT_API GC object closure_make_op(context_t context, object sexp, object env) {
    assert(context != NULL);
    gc_param2(context, sexp, env);
    gc_var2(context, args_code, closure);

    closure = pair_make_op(context, sexp, env);
    set_ext_type_closure(closure);

    gc_release_param(context);
    return closure;
}

// 其实就是 closure
EXPORT_API GC object macro_make_op(context_t context, object sexp, object env) {
    assert(context != NULL);
    gc_param2(context, sexp, env);
    gc_var1(context, macro);

    macro = closure_make_op(context, sexp, env);
    set_ext_type_macro(macro);

    gc_release_param(context);
    return macro;
}

EXPORT_API GC object continuation_make_op(context_t context, object stack) {
    assert(context != NULL);
    gc_param1(context, stack);
    gc_var1(context, cont);

    cont = pair_make_op(context, IMM_UNIT, stack);
    set_ext_type_continuation(cont);

    gc_release_param(context);
    return cont;
}

/**
                           对象值操作: eqv
******************************************************************************/
/**
 * 只作浅比较
 * @param a
 * @param b
 * @return 0 / !0
 */
int eqv(context_t context, object a, object b) {
    if (is_string(a)) {
        return string_equals(context, a, b);
    } else if (is_i64(a)) {
        return i64_equals(context, a, b);
    } else if (is_doublenum(a)) {
        return d64_equals(context, a, b);
    } else if (is_syntax(a)) {
        return is_syntax(b) && (syntax_get_opcode(a) == syntax_get_opcode(b));
    } else if (is_proc(a)) {
        return is_proc(b) && (proc_get_opcode(a) == proc_get_opcode(b));
    } else if (is_pair(a)) {
        if (is_pair(b)) {
            return (a == b) || ((pair_car(a) == pair_car(b)) && (pair_cdr(a) == pair_cdr(b)));
        } else {
            return 0;
        }
    } else {
        return a == b;
    }
}