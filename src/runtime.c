#include <paper-scheme/runtime.h>


/******************************************************************************
                                ������ API
******************************************************************************/

/**
 * �ڲ�����, ���� object
 * @param context
 * @param type ���� enum object_type_enum
 * @param unaligned_object_size
 * @return
 */
static OUT NOTNULL GC object
raw_object_make(REF NOTNULL context_t context, IN object_type_tag type, IN size_t unaligned_object_size) {
    assert(context != NULL);

    // ��������С
    size_t real_size = aligned_size(unaligned_object_size);
    assert(unaligned_object_size <= real_size);

    size_t padding_size = real_size - unaligned_object_size;
    assert(padding_size <= UINT8_MAX);  // ����С���ܳ��� object->padding_size �������ֵ����

    // �������
    object ret = gc_alloc(context, real_size);
    memset(ret, 0, real_size);

    // Ԥ�������ͷ
    ret->magic = OBJECT_HEADER_MAGIC;
    set_ext_type_tag_none(ret);
    set_mutable(ret);
    ret->type = type;
    ret->marked = 0;
    ret->padding_size = (uint8_t) padding_size;   // ����С����Ϊ0
    ret->forwarding = NULL;

    return ret;
}

/**
 * ���� i64 ���Ͷ���, ������ֱ��ʹ��, ��ΪҪ���ǵ����ӵı߽�����, �μ� i64_make_op()
 * @param heap
 * @param v i64 ֵ
 * @return
 */
EXPORT_API OUT NOTNULL GC object i64_make_real_object_op(REF NOTNULL context_t context, IN int64_t v) {
    assert(context != NULL);
    object ret = raw_object_make(context, OBJ_I64, object_sizeof_base(i64));
    // ����ֵ
    ret->value.i64 = v;
    return ret;
}

/**
 * ���� i64 ���Ͷ���, ���ֵ��Χ���� [- 2^(63-1), 2^(63-1)-1] ����������
 * @param heap
 * @param v i64 ֵ
 * @return object ��������
 */
EXPORT_API OUT NOTNULL GC object i64_make_op(REF NOTNULL context_t context, IN int64_t v) {
    if (v >= I64_IMM_MIN && v <= I64_IMM_MAX) {
        return i64_imm_make(v);
    } else {
        return i64_make_real_object_op(context, v);
    }
}

/**
 * ���� doublenum ���Ͷ���
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
 * ���� pair ���Ͷ���
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
 * ���� bytes ����
 * @param context
 * @param capacity bytes ����, bytes_size * sizeof(char)
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
 * ���� symbol ���Ͷ���
 * <p>symbol_len() ������������ '\0', ���� object->symbol.len ���� '\0', ����Ϊ�˷�������ʱ��������С</p>
 * <p>��Ҫֱ��ʹ��, �μ� interpreter.h: symbol_make_from_cstr_op()</p>
 * @param context
 * @param cstr C�ַ���, '\0'��β
 * @return
 */
EXPORT_API OUT NOTNULL GC object
symbol_make_from_cstr_untracked_op(REF NOTNULL context_t context, COPY const char *cstr) {
    assert(context != NULL);
    size_t cstr_len;

    // ��ָ�����ʼ��Ϊ���ַ���
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
 * ���� string ���Ͷ���
 * <p>string_len() ������������ '\0',
 * ���� object->string.len ���� '\0', ����Ϊ�˷�������ʱ��������С</p>
 * @param context
 * @param cstr C�ַ���, '\0'��β
 * @return
 */
EXPORT_API OUT NOTNULL GC object
string_make_from_cstr_op(REF NOTNULL context_t context, COPY const char *cstr) {
    assert(context != NULL);
    size_t cstr_len;

    // ��ָ�����ʼ��Ϊ���ַ���
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
 * ���� string ���Ͷ���
 * @param context
 * @param count �ַ�����
 * @param fill ���
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
 * ���� string_buffer ����
 * @param context
 * @param char_size char ����, ע�� string_buffer ���� '\0' ����
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
    // buffer ��û�� '\0' ��Ϊ��β
    ret->value.string_buffer.buffer_length = 0;
    ret->value.string_buffer.hash = pointer_with_type_to_hash(ret, OBJ_STRING_BUFFER);

    gc_release_var(context);
    return ret;
}

/**
 * �� string ���� string_buffer ����, ĩβ���� '\0'
 * @param context
 * @param str string, ���
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

    // ������������ '\0'
    size_t str_len = string_len(str);
    // ʵ�� buffer ��СҪ���� STRING_BUFFER_DEFAULT_INIT_SIZE, ��Ϊ buffer ������������
    // string_buffer ��������Ҳ����Ҫ�޸�
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
 * ���� vector ���Ͷ���, ��ʼ����� Unit, �� '()
 * @param context
 * @param vector_len vector ����
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
 * ���� stack ���Ͷ���, ��� Unit.
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
 * �������ַ����� input port, ����� string Ӧ���ǲ��ɱ����
 * <p>obj->value.string_port.length ���� ������ '\0'</p>
 * <p>(open-input-string "string here")</p>
 * @param context
 * @param str
 * @return ��ʧ�ܷ��� IMM_UNIT
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
 * �� output port, �����ڲ�����
 * <p>(open-output-string)</p>
 * @param context
 * @return ��ʧ�ܷ��� IMM_UNIT, ���򷵻�ֵ���� is_srfi6_port(obj),
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
 * �� input-output port, ���, �����ڲ�����
 * <p>(open-input-output-string)</p>
 * @param context
 * @return ��ʧ�ܷ��� IMM_UNIT, ���򷵻�ֵ���� is_srfi6_port(obj), is_string_port_in_out_put(obj)
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
 * �������ַ����� input-output port, ���, �����ڲ�����
 * <p>(open-input-output-string "string here")</p>
 * @param context
 * @param str
 * @return ��ʧ�ܷ��� IMM_UNIT, ���򷵻�ֵ���� is_srfi6_port(obj), is_string_port_in_out_put(obj)
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
 * ���ļ����� file port
 * @param context
 * @param filename
 * @param kind PORT_INPUT / PORT_OUTPUT / PORT_INPUT & PORT_OUTPUT
 * @return ��ʧ�ܷ��� IMM_UNIT
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
        // ע���ͷ�
        gc_release_param(context);
        return IMM_UNIT;
    }
    port = raw_object_make(context, OBJ_STDIO_PORT, object_sizeof_base(stdio_port));

    port->value.stdio_port.kind = kind;
    port->value.stdio_port.file = file;
    port->value.stdio_port.filename = filename;
    port->value.stdio_port.is_released = 0;
    // �򿪵��ļ���Ҫ�ر�
    port->value.stdio_port.need_close = 1;
    port->value.stdio_port.current_line = 1;
    port->value.stdio_port.hash = pointer_with_type_to_hash(port, OBJ_STDIO_PORT);

    gc_release_param(context);
    return port;
}

/**
 * �� FILE * �� file port
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
    // �ⲿ�򿪵��ļ��ⲿ����ر�
    port->value.stdio_port.need_close = 0;
    port->value.stdio_port.current_line = 1;
    port->value.stdio_port.hash = pointer_with_type_to_hash(port, OBJ_STDIO_PORT);

    gc_release_var(context);
    return port;
}

/**
 * ���� hashset
 * @param context
 * @param init_capacity hashset ��ʼ��С (Ĭ�� DEFAULT_HASH_SET_MAP_INIT_CAPACITY)
 * @param load_factor �������� (Ĭ�ϴ�С DEFAULT_HASH_SET_MAP_LOAD_FACTOR)
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
 * ���� hashmap
 * @param context
 * @param init_capacity hashmap ��ʼ��С (Ĭ�� DEFAULT_HASH_SET_MAP_INIT_CAPACITY)
 * @param load_factor �������� (Ĭ�ϴ�С DEFAULT_HASH_SET_MAP_LOAD_FACTOR)
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
 * ���� weak ref
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
 * ���� weak_hashset
 * <p>������ hashset</p>
 * @param context
 * @param init_capacity hashset ��ʼ��С (Ĭ�� DEFAULT_HASH_SET_MAP_INIT_CAPACITY)
 * @param load_factor �������� (Ĭ�ϴ�С DEFAULT_HASH_SET_MAP_LOAD_FACTOR)
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
 * ���� stack frame
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
 * ���� environment slot
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
 * ���� proc
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
 * ���� syntax
 * @param context
 * @param symbol �ؼ�����
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
 * ���� promise
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
                                ������� API
******************************************************************************/

/**
 * string ����ƴ��, ���
 * @param context
 * @param string_a
 * @param string_b
 * @return ƴ�Ӻ�� string
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

    // �ȸ��Ƶ�һ���ַ���, ������ '\0'
    memcpy(string_get_cstr(ret), string_get_cstr(string_a), string_a_len);
    // �ٸ��Ƶڶ����ַ���, ĩβ���� '\0'
    memcpy(((char *) string_get_cstr(ret)) + string_a_len, string_get_cstr(string_b), string_b_len + 1);
    // �������, ĩβ��� '\0'
    ((char *) string_get_cstr(ret))[new_len_with_null - 1] = '\0';
    // �����ַ�������
    ret->value.string.len = new_len_with_null;
    ret->value.string.hash = string_hash_helper(ret);

    gc_release_param(context);
    return ret;
}

/**
 * string_buffer ƴ�� string, ���
 * @param context
 * @param str_buffer string_buffer
 * @param str string
 * @return �޸ĺ�� string_buffer
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
        // ��������
        size_t add_size = (str_len + str_buffer_len + STRING_BUFFER_DEFAULT_GROWTH_SIZE) - buffer_size;
        string_buffer_capacity_increase_op(context, str_buffer, add_size);

        assert(add_size + buffer_size == string_buffer_capacity(str_buffer));
    }

    // ������ '\0'
    memcpy(string_buffer_bytes_data(str_buffer) + str_buffer_len, string_get_cstr(str), str_len);
    // ��������
    str_buffer->value.string_buffer.buffer_length = str_buffer_len + str_len;

    gc_release_param(context);
    return str_buffer;
}

/**
 * string_buffer ƴ�� c_str, ���
 * @param context
 * @param str_buffer string_buffer
 * @param str string
 * @return ԭʼ string_buffer
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
        // ��������
        size_t add_size = (str_len + str_buffer_len + STRING_BUFFER_DEFAULT_GROWTH_SIZE) - buffer_size;
        string_buffer_capacity_increase_op(context, str_buffer, add_size);

        assert(add_size + buffer_size == string_buffer_capacity(str_buffer));
    }

    // ������ '\0'
    memcpy(string_buffer_bytes_data(str_buffer) + str_buffer_len, cstr, str_len);
    // ��������
    str_buffer->value.string_buffer.buffer_length = str_buffer_len + str_len;

    gc_release_param(context);
    return str_buffer;
}

/**
 * string_buffer ƴ�� imm_char, ���
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
 * string_buffer ƴ�� char, ���
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
        // ��ǰ string_buffer_length(str_buffer) + 1, ���� '+1' Ϊ �����ӵ� char
        string_buffer_capacity_increase_op(context, str_buffer, STRING_BUFFER_DEFAULT_GROWTH_SIZE + 1);
    }

    // char ƴ��
    string_buffer_bytes_data(str_buffer)[buffer_len] = ch;
    // string_buffer ���� +1
    string_buffer_length(str_buffer)++;

    gc_release_param(context);
    return str_buffer;
}

/**
 * vector ���
 * @param vector
 * @param obj �����, ǳ����
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
 * hashset �Ƿ����ָ���Ķ���
 * <p>���ᴥ�� GC</p>
 * @param context
 * @param hashset
 * @param obj object ����Ϊ NULL
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
 * obj ���� hashset
 * @param context
 * @param obj
 * @return ����Ѿ����ھ�ֵ, �򷵻ؾ�ֵ, ���򷵻ظոշ����ֵ
 */
EXPORT_API GC object
hashset_put_op(REF NOTNULL context_t context, REF NOTNULL object hashset, REF NOTNULL object obj) {
    assert(context != NULL);
    assert(is_hashset(hashset));
    assert(!is_null(obj));

    gc_param2(context, hashset, obj);
    gc_var1(context, ret);

    // ���е� key ��Ӧ�� value ���� IMM_TRUE
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
 * hashset_b ȫ������ hashset_a, ǳ����
 * @param context
 * @param hashset_a ����Ϊ��
 * @param hashset_b ����Ϊ��
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
 * ��� hashset
 * <p>���ᴥ�� GC</p>
 * @param context
 * @param hashset ����Ϊ��
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
 * �� hashset ���Ƴ� object
 * <p>���ᴥ�� GC</p>
 * @param context
 * @param hashset
 * @param obj ����Ϊ��, ����Ϊ IMM_UNIT
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
 * hashmap �Ƿ����ָ���Ķ���
 * <p>���ᴥ�� GC</p>
 * @param context
 * @param hashmap
 * @param key object ����Ϊ NULL
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
    // �Ҳ����ڵ�
    return IMM_FALSE;
}

/**
 * obj ���� hashmap
 * @param context
 * @param hashmap
 * @param k ��
 * @param v ֵ
 * @return ��� k �Ѿ�����, �򷵻ؾɵ� v, ���򷵻� IMM_UNIT
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

    // 1. ���� hash ֵ
    uint32_t hash = 0;

    // hash ����Ϊ��, hash ֵΪ 0
    if (hash_fn != NULL) {
        hash = hash_fn(context, k);
    }

    // 2. ���� hash ֵ��������λ��
    vector = hashmap->value.hashmap.table;
    size_t vec_len = vector_len(vector);
    size_t index = hash % vec_len;

    // 3. �������λ���Ƿ��о�ֵ, ����������滻, ���ؾ�ֵ
    for (entry_list = vector_ref(vector, index); entry_list != IMM_UNIT; entry_list = pair_cdr(entry_list)) {
        assert(is_pair(entry_list));
        old_entry = pair_car(entry_list);
        assert(is_pair(old_entry));

        // ȡ����ֵ
        old_key = pair_car(old_entry);
        if (k == old_key || equals(context, k, old_key)) {
            old_value = pair_cdr(old_entry);

            // �����½ڵ��滻����
            new_entry = pair_make_op(context, k, v);
            pair_car(entry_list) = new_entry;

            // ע���ͷ� C ����ջ������
            gc_release_param(context);
            return old_value;
        }
    }


    // 4. û���ҵ���ֵ, ˵��Ҫ�����µ� entry, ����Ƿ���Ҫ����
    if (hashmap->value.hashmap.size >= hashmap->value.hashmap.threshold &&
        (vector_ref(vector, index) != IMM_UNIT)) {
        // ��ʱ�������ڵ���������ֵ, �� index λ�ò�Ϊ IMM_UNIT
        // ����Ҫ����
        size_t new_len = vec_len * 2;
        new_vector = vector_make_op(context, new_len);

        // ת�ƾɵ�����, ��Ҫ���¼��� index
        for (size_t i = 0; i < vec_len; i++) {

            // ȡ���ɵ� entry
            entry_list = vector_ref(vector, i);

            if (entry_list != IMM_UNIT) {

                // ��Ϊ�յ�ʱ����Ҫ���� entry ���� hash ��ת��
                // ������Ҫ����, ��Ϊ vector Ĭ�ϳ�ʼ��Ϊ IMM_UNIT
                old_entry = pair_car(entry_list);
                old_key = pair_car(old_entry);
                size_t tmp_hash = 0;
                hash_code_fn tmp_hash_fn = object_hash_helper(context, old_key);
                if (tmp_hash_fn != NULL) {
                    tmp_hash = tmp_hash_fn(context, old_key);
                }
                size_t new_index = tmp_hash % new_len;

                // �ɵ� entry_list �� hash ֵӦ�ö�����ͬ��
                vector_ref(new_vector, new_index) = entry_list;
            }
        }
        // �޸� vector

        // ת�ƽ���, ���¼��� hash ֵ
        hash = 0;
        if (hash_fn != NULL) {
            hash = hash_fn(context, k);
        }
        index = hash % new_len;

        // �������½ڵ�
        new_entry = pair_make_op(context, k, v);
        entry_list = pair_make_op(context, new_entry, vector_ref(new_vector, index));
        vector_set(new_vector, index, entry_list);
        hashmap->value.hashmap.table = new_vector;
        hashmap->value.hashmap.threshold = (size_t) (vector_len(new_vector) * hashmap->value.hashmap.load_factor);
    } else {
        // 5. ������Ҫ����, �����½ڵ�
        vector = hashmap->value.hashmap.table;
        new_entry = pair_make_op(context, k, v);
        entry_list = pair_make_op(context, new_entry, vector_ref(vector, index));
        vector_set(vector, index, entry_list);
    }

    // ��ʱ������µļ�ֵ��, ���� IMM_UNIT
    hashmap->value.hashmap.size++;
    gc_release_param(context);
    return IMM_UNIT;
}

/**
 * hashmap ȡ�� key ��Ӧ�� value
 * <p>���ᴥ�� GC</p>
 * @param context
 * @param hashmap
 * @param key
 * @return ��� key ����, �򷵻ض�Ӧ�� value, ���򷵻� IMM_UNIT
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
    // �Ҳ����ڵ�
    return IMM_UNIT;
}

/**
 * hashmap_b ȫ������ hashmap_a, ǳ����
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
 * ��� hashmap
 * <p>���ᴥ�� GC</p>
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
 * �� hashmap �Ƴ�ָ���� key
 * <p>���ᴥ�� GC</p>
 * @param context
 * @param hashmap
 * @param key
 * @return ��� key �Ѿ�����, ���ر��Ƴ��� value, ���򷵻� IMM_UNIT
 */
EXPORT_API OUT NOTNULL object
hashmap_remove_op(REF NOTNULL context_t context, REF NOTNULL object hashmap, REF NOTNULL object key) {
    assert(context != NULL);
    assert(is_hashmap(hashmap));
    assert(!is_null(key));
    gc_set_no_gc_assert_flag(context);

    // �ձ�
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
    // ��ʱ������һ�� entry
    object pre_entry_list = NULL;
    do {
        object deleted_key = pair_caar(entry_list);
        object deleted_value = pair_cdar(entry_list);

        if (deleted_key == key || equals(context, deleted_key, key)) {
            if (pre_entry_list == NULL) {
                // entry_list �ǵ�һ���ڵ�
                vector_ref(hashmap->value.hashmap.table, index) = pair_cdr(entry_list);
            } else {
                // entry_list �ǵڶ������Ժ�ڵ�
                pair_cdr(pre_entry_list) = pair_cdr(entry_list);
            }

            hashmap->value.hashmap.size--;
            return deleted_value;
        }

        pre_entry_list = entry_list;
        entry_list = pair_cdr(entry_list);
    } while (entry_list != IMM_UNIT);

    gc_assert_no_gc(context);
    // �Ҳ����ڵ�
    return IMM_UNIT;
}


/**
 * ��� weak_hashset ��ָ�� table[index] ��ʧЧ�������ö���
 * <p>���ᴥ�� GC</p>
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

            // �Ƴ�ʧЧ����
            weak_hashset->value.weak_hashset.size--;
        } else {
            pre = next;
            next = pair_cdr(next);
        }
    }

    gc_assert_no_gc(context);
}

/**
 * weak_hashset �Ƿ����ָ���Ķ���
 * <p>���������Ч����</p>
 * <p>���ᴥ�� GC</p>
 * @param context
 * @param weak_hashset
 * @param obj object ����Ϊ NULL
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

    // �Ҳ���ֵ
    gc_assert_no_gc(context);
    return IMM_FALSE;
}

/**
 * obj ���� weak_hashset
 * <p>���������Ч����</p>
 * @param context
 * @param obj
 * @return ��Ӻ�� object, ��������򷵻�ԭʼ object
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

    // Ԥ�ȹ��� ���ܲ����������
    // GC point
    new_weak_ref = weak_ref_make_op(context, IMM_UNIT);
    new_entry = pair_make_op(context, IMM_UNIT, IMM_UNIT);

    // 1. ���� hash ֵ
    uint32_t hash = 0;

    // hash ����Ϊ��, hash ֵΪ 0
    if (hash_fn != NULL) {
        hash = hash_fn(context, obj);
    }

    // 2. ���� hash ֵ��������λ��
    table = weak_hashset->value.hashmap.table;
    size_t vector_length = vector_len(table);
    size_t index = hash % vector_length;
    //  ���� index λ�õĿ�ֵ, ����������λ��, contains() ������Ȼ������
    weak_hashset_expunge_stale_ref(context, weak_hashset, index);

    // 3. �������λ���Ƿ��о�ֵ, ��������򷵻ؾ�ֵ
    for (entry_list = vector_ref(table, index);
         entry_list != IMM_UNIT;
         entry_list = pair_cdr(entry_list)) {

        assert(is_pair(entry_list));
        ref = pair_car(entry_list);
        assert(is_weak_ref(ref));

        // ����Ƿ����
        if (weak_ref_is_valid(ref)) {
            old_object = weak_ref_get(ref);
            assert(!is_null(old_object));

            if (equals(context, obj, old_object)) {
                // �ҵ��Ѿ����ڵĶ���, ֱ�ӷ���
                // ע���ͷ� C ����ջ������
                gc_release_param(context);
                return old_object;
            }
        }
    }


    // 4. û���ҵ���ֵ, ˵��Ҫ�����µ� entry, ����Ƿ���Ҫ����
    //  �����ٽ����������� hash ��ͻʱ��������
    if (weak_hashset->value.weak_hashset.size >= weak_hashset->value.weak_hashset.threshold &&
        (vector_ref(table, index) != IMM_UNIT)) {

        // ��ʱ�������ڵ���������ֵ, �� index λ�ò�Ϊ IMM_UNIT
        // ����Ҫ����
        size_t new_length = vector_length * 2;
        // GC point
        new_table = vector_make_op(context, new_length);

        // ���� GC ����ܳ��ֵ���Чֵ
        for (size_t i = 0; i < vector_length; i++) {
            weak_hashset_expunge_stale_ref(context, weak_hashset, i);
        }

        // ת�ƾɵ�����, ��Ҫ���¼��� index
        for (size_t i = 0; i < vector_length; i++) {

            // ȡ���ɵ� entry
            entry_list = vector_ref(table, i);

            if (entry_list != IMM_UNIT) {
                assert(is_pair(entry_list));
                // ��Ϊ�յ�ʱ����Ҫ���� entry ���� hash ��ת��
                // ������Ҫ����, ��Ϊ vector Ĭ�ϳ�ʼ��Ϊ IMM_UNIT
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

                // �ɵ� entry_list �� hash ֵӦ�ö�����ͬ��
                vector_ref(new_table, new_index) = entry_list;
            }
        } // ���ݽ���

        // �޸� table
        weak_hashset->value.weak_hashset.table = new_table;
        weak_hashset->value.weak_hashset.threshold =
                (size_t) (vector_len(new_table) * weak_hashset->value.weak_hashset.load_factor);

        // ���¼��� hash ֵ
        hash = 0;
        if (hash_fn != NULL) {
            hash = hash_fn(context, obj);
        }
        index = hash % new_length;

        // �������½ڵ�
        entry_list = vector_ref(new_table, index);
        weak_ref_get(new_weak_ref) = obj;

        pair_car(new_entry) = new_weak_ref;
        pair_cdr(new_entry) = entry_list;
        vector_set(new_table, index, new_entry);

    } else {

        // 5. ������Ҫ����, �����½ڵ�
        entry_list = vector_ref(table, index);
        weak_ref_get(new_weak_ref) = obj;

        pair_car(new_entry) = new_weak_ref;
        pair_cdr(new_entry) = entry_list;
        vector_set(table, index, new_entry);
    }

    // ��ʱ������µļ�ֵ��, ���� IMM_UNIT
    weak_hashset->value.weak_hashset.size++;
    gc_release_param(context);
    return obj;
}

/**
 * ��� weak_hashset
 * <p>���ᴥ�� GC</p>
 * @param context
 * @param weak_hashset ����Ϊ��
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
 * �� weak_hashset ���Ƴ� object
 * <p>���������Ч����</p>
 * <p>���ᴥ�� GC</p>
 * @param context
 * @param weak_hashset
 * @param obj ����Ϊ��, ����Ϊ IMM_UNIT
 */
EXPORT_API void
weak_hashset_remove_op(REF NOTNULL context_t context, REF NOTNULL object weak_hashset, REF NOTNULL object obj) {
    assert(context != NULL);
    assert(is_weak_hashset(weak_hashset));
    assert(!is_null(obj));
    gc_set_no_gc_assert_flag(context);

    // �ձ�
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

    // ��ʱ������һ�� entry
    object pre, next;
    pre = IMM_UNIT;
    next = vector_ref(table, index);

    while (next != IMM_UNIT) {
        assert(is_pair(next));
        object ref = pair_car(next);
        assert(is_weak_ref(ref));

        if (!weak_ref_is_valid(ref)) {
            // �Ƴ���Ч������
            if (pre == IMM_UNIT) {
                vector_ref(table, index) = pair_cdr(next);
            } else {
                pair_cdr(pre) = pair_cdr(next);
            }
            next = pair_cdr(next);
            weak_hashset->value.weak_hashset.size--;
        } else {
            // ��Ч������, ����Ƿ�����Ҫ���Ƴ��Ķ���
            assert(weak_ref_is_valid(ref));
            object old_object = weak_ref_get(ref);

            if (equals(context, obj, old_object)) {
                // �ҵ�������ɾ��������
                if (pre == IMM_UNIT) {
                    vector_ref(table, index) = pair_cdr(next);
                } else {
                    pair_cdr(pre) = pair_cdr(next);
                }
                weak_hashset->value.weak_hashset.size--;
                // ���ҽ���
                return;
            } else {
                // û�ҵ�, ������һ������
                pre = next;
                next = pair_cdr(next);
            }
        } // ��Ч�������ж�
    }

    gc_assert_no_gc(context);
}

/**
 * ���� weak_hashset Ԫ������
 * <p>�Զ����ȫ����Ч����</p>
 * <p>���ᴥ�� GC</p>
 * @param context
 * @param weak_hashset
 * @return Ԫ������
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
                                �������� API
******************************************************************************/

/**
 * bytes ����, ���
 * @param context
 * @param bytes
 * @param add_size ���ӵĴ�С
 * @return �᷵���¶���
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
    // ����ԭʼ����
    memcpy(bytes_data(new_bytes), bytes_data(bytes), bytes_capacity(bytes));

    gc_release_param(context);
    return new_bytes;
}

/**
 * string_buffer ����, ���
 * @param context
 * @param str_buffer
 * @param add_size ������С
 */
EXPORT_API OUT NOTNULL GC void
string_buffer_capacity_increase_op(REF NOTNULL context_t context,
                                   NOTNULL IN object str_buffer, size_t add_size) {
    assert(context != NULL);
    assert(is_string_buffer(str_buffer));

    gc_param1(context, str_buffer);
    gc_var1(context, new_buffer);

    // �ڲ� bytes ��������, �Զ����ƾ�����
    new_buffer = bytes_capacity_increase_op(context, string_buffer_bytes_obj(str_buffer), add_size);
    string_buffer_bytes_obj(str_buffer) = new_buffer;
    // ��������
    string_buffer_capacity(str_buffer) = bytes_capacity(new_buffer);

    gc_release_param(context);
}

/**
 * vector ����, ���
 * @param context
 * @param vec
 * @param add_size
 * @return �᷵���� vector
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
 * stack ����, ���
 * @param context
 * @param stack
 * @param add_size
 * @return �᷵���� stack
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
 * stack �Զ���������ջ
 * <p>ע��, push ��Ӧ�����¸�ԭ����ջ��ֵ</p>
 * @param context
 * @param stack
 * @param element
 * @param extern_growth_size ���ջ��, ���Զ����� extern_growth_size + 1 (�� 0 ���Զ����� 1)
 * @return ���ܷ����� stack
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
                                ����ת�� API
******************************************************************************/

/**
 * char ������ ת string
 * @param context
 * @param imm_char ������
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
 * char ת string
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
 * symbol ת string
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
 * string ת symbol
 * <p>��Ҫֱ��ʹ��, �μ� interpreter.h: string_to_symbol_op()</p>
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
 * string_buffer ת��Ϊ string, ���
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

    // string ����Ҫ�� string_buffer ���ȶ�һ�� '\0'
    size_t char_length = string_buffer_length(str_buffer);
    size_t size_with_null = char_length + 1;
    str = raw_object_make(context, OBJ_STRING,
                          object_sizeof_base(string) + size_with_null * sizeof(char));
    str->value.string.len = size_with_null;
    // string_buffer ĩβû�� '\0', ��Ҫ�ֶ����
    memcpy(string_get_cstr(str), string_buffer_bytes_data(str_buffer), char_length);
    string_get_cstr(str)[char_length] = '\0';
    str->value.string.hash = string_hash_helper(str);

    gc_release_param(context);
    return str;
}

/**
 * string_buffer ת��Ϊ symbol, ���
 * <p>��Ҫֱ��ʹ��, �μ� interpreter.h: string_buffer_to_symbol_op()</p>
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

    // string ����Ҫ�� string_buffer ���ȶ�һ�� '\0'
    size_t char_length = string_buffer_length(str_buffer);
    size_t size_with_null = char_length + 1;
    symbol = raw_object_make(context, OBJ_SYMBOL,
                             object_sizeof_base(symbol) + size_with_null * sizeof(char));
    symbol->value.symbol.len = size_with_null;
    // string_buffer ĩβû�� '\0', ��Ҫ�ֶ����
    memcpy(symbol_get_cstr(symbol), string_buffer_bytes_data(str_buffer), char_length);
    string_get_cstr(symbol)[char_length] = '\0';
    symbol->value.symbol.hash = symbol_hash_helper(symbol);

    gc_release_param(context);
    return symbol;
}

/**
 * hashset תΪ vector, ����
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
 * hashmap תΪ vector, ����
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

    // map �ж�������
    size_t map_size = hashmap_size(hashmap);
    if (map_size == 0) {
        // hashmap ��СΪ 0, ֱ�ӷ��ز��ٱ���
        // ע���ͷ�
        gc_release_param(context);
        return vector_make_op(context, 0);
    } else {
        vector = vector_make_op(context, map_size);
    }

    // vector ����
    size_t vector_index = 0;
    size_t map_table_vector_len = vector_len(hashmap->value.hashmap.table);
    for (size_t i = 0; i < map_table_vector_len; i++) {

        entry_list = vector_ref(hashmap->value.hashmap.table, i);
        for (; entry_list != IMM_UNIT; entry_list = pair_cdr(entry_list)) {

            // ��ȡ hashmap �е� entry
            entry = pair_car(entry_list);
            key = pair_car(entry);
            assert(is_symbol(key));
            value = pair_cdr(entry);

            // ��� vector
            entry = pair_make_op(context, key, value);
            vector_ref(vector, vector_index) = entry;

            // ÿ�ҵ�һ�� (k, v), �α�����ƶ�
            vector_index++;
        }
    }
    assert(map_size == vector_index);

    gc_release_param(context);
    return vector;
}


/**
 * weak_hashset תΪ vector, ����
 * <p>���ص� vector �п��ܰ��� IMM_UNIT</p>
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

    // map �ж�������
    size_t set_size = weak_hashset_size_op(context, weak_hashset);
    if (set_size == 0) {
        // hashmap ��СΪ 0, ֱ�ӷ��ز��ٱ���
        // ע���ͷ�
        gc_release_param(context);
        return vector_make_op(context, 0);
    } else {
        vector = vector_make_op(context, set_size);
    }

    // vector ����
    size_t vector_index = 0;
    size_t map_table_vector_len = vector_len(weak_hashset->value.weak_hashset.table);
    for (size_t i = 0; i < map_table_vector_len; i++) {

        entry_list = vector_ref(weak_hashset->value.weak_hashset.table, i);
        for (; entry_list != IMM_UNIT; entry_list = pair_cdr(entry_list)) {

            // ��ȡ hashmap �е� entry
            ref = pair_car(entry_list);
            assert(is_weak_ref(ref));

            if (weak_ref_is_valid(ref)) {
                obj = weak_ref_get(ref);
                vector_set(vector, vector_index, obj);
                // ÿ�ҵ�һ����Ч����, �α�����ƶ�
                vector_index++;
            }
        }
    }
    assert(vector_index <= set_size);

    gc_release_param(context);
    return vector;
}

/******************************************************************************
                             ������� API
******************************************************************************/
/**
 * �� port ����һ�� char
 * <p>���ᴥ�� GC</p>
 * @param port
 * @return IMM_CHAR �� IMM_EOF
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
 * char ���·Ż� port
 * <p>���ᴥ�� GC</p>
 * @param port PORT_INPUT
 * @param ch IMM_CHAR / IMM_EOF, ���߲����κ���
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
            // ���� ch ֵ
            port->value.string_port.current--;
        }
    }

    // ����ַ����˻�, ��ض����� EOF
    if (is_port_eof(port)) {
        if (is_stdio_port_eof(port)) {
            port->value.stdio_port.kind ^= PORT_EOF;
        } else {
            port->value.string_port.kind ^= PORT_EOF;
        }
    }
}

/**
 * �� port ��� char
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
 * �� port ��� c str
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
 * �� port ��� string
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
 * port ��λ
 * <p>���ᴥ�� GC</p>
 * @param port
 * @param offset ƫ����
 * @param origin ��ʼλ��: 0, ��ʼ; 1, ��ǰλ��; 2, ����λ��
 * @return IMM_TRUE / IMM_FALSE �ƶ��Ƿ�ɹ�
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

    // fseek ����, ȥ�� EOF ��־
    // ����ַ����˻�, ��ض����� EOF
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
 * ���ص�ǰ port λ��
 * <p>���ᴥ�� GC</p>
 * @param port
 * @return port ��ǰ�α�λ��
 */
EXPORT_API size_t port_tail(REF NOTNULL object port) {
    if (is_stdio_port(port)) {
        return ftell(port->value.stdio_port.file);
    } else {
        return port->value.string_port.current;
    }
}

/******************************************************************************
                              list ���� API
******************************************************************************/
/**
 * <p>list ԭ������</p>
 * <p>���ᴥ�� GC</p>
 * @param context
 * @param term ��������
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
 * list ����, �����µ� pair
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
 * <p>���ز��� IMM_UNIT ��β�� list</p>
 * <p>���仰˵ȥ��ԭʼ list ��β�� IMM_UNIT</p>
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
                             ����ʱ���͹���
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

// ��ʵ���� closure
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
                           ����ֵ����: eqv
******************************************************************************/
/**
 * ֻ��ǳ�Ƚ�
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