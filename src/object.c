#include <paper-scheme/object.h>


/**
                                ����ֵ����
******************************************************************************/
/**
 * ����ջ��Ԫ��
 * @param stack
 * @return object ���ջΪ��, ���� NULL
 */
NULLABLE CHECKED REF object stack_peek_op(object stack) {
    assert(is_object(stack));
    assert(is_stack(stack));

    if (stack_empty(stack)) return NULL;
    else {
        size_t top = stack->value.stack.length - 1;
        return stack->value.stack.data[top];
    }
}
/**
 * ��ջ
 * @param stack
 * @param obj
 * @return ���ջ��, ���� 0, ���򷵻� 1
 */
NULLABLE OUT int stack_push_op(REF object stack, REF object obj) {
    assert(is_object(stack));
    assert(is_stack(stack));

    if (!stack_full(stack)) {
        size_t length = stack->value.stack.length;
        stack->value.stack.data[length] = obj;
        stack->value.stack.length++;
        return 1;
    } else {
        // ջ��
        return 0;
    }
}
/**
 * ��ջ
 * @param stack
 * @return ���ջ��, ���� 0; ���򷵻� 1
 */
CHECKED OUT int stack_pop_op(REF object stack) {
    assert(is_object(stack));
    assert(is_stack(stack));

    if (!stack_empty(stack)) {
        stack->value.stack.length--;
        return 1;
    } else {
        return 0;
    }
}


/**
                               ������� API
******************************************************************************/


/**
 * �����������Ĵ�С
 * �÷�: aligned_size(object_sizeof_base(value_field))
 * ����һ����С, ���ض��뵽 1<<ALIGN_BITS �Ĵ�С
 * @param unaligned_size
 * @return
 */
EXPORT_API OUT size_t aligned_size(IN size_t unaligned_size) {
    size_t need_aligned = unaligned_size & (size_t) ALIGN_MASK;
    size_t real_alloc_size = unaligned_size & (~((size_t) ALIGN_MASK));
    if (need_aligned) {
        // �����Ҫ����, ����λһ�� ALIGN_SIZE
        real_alloc_size += ALIGN_SIZE;
    }

    return real_alloc_size;
}

/**
 * ����ʱ��������С, ��λ bytes, �ṩ�� gc ʹ��
 * @param object NOTNULL ���������������ָ��
 * @return ��������Ƿ�, ���� 0
 */
EXPORT_API OUT OUT size_t object_bootstrap_sizeof(REF NOTNULL object obj) {
    assert(obj != NULL);
    assert(is_object(obj));

    if (is_imm(obj)) {
        // ��Ӧ�ö�������ִ���������
        assert(!is_imm(obj));
        return 0;
    }

#pragma push_macro("size_helper")
#define size_helper(value_field, flexible_array_size) \
    (object_sizeof_base(value_field) + (flexible_array_size) + (obj)->padding_size)

    if (is_i64_real(obj)) {
        return size_helper(i64, 0);
    } else if (is_doublenum(obj)) {
        return size_helper(doublenum, 0);
    } else if (is_imm_char(obj)) {
        return 0;
    } else if (is_imm_true(obj)) {
        return 0;
    } else if (is_imm_false(obj)) {
        return 0;
    } else if (is_imm_unit(obj)) {
        return 0;
    } else if (is_imm_eof(obj)) {
        return 0;
    } else if (is_pair(obj)) {
        return size_helper(pair, 0);
    } else if (is_bytes(obj)) {
        return size_helper(bytes, sizeof(char) * obj->value.bytes.capacity);
    } else if (is_symbol(obj)) {
        return size_helper(symbol, sizeof(char) * obj->value.symbol.len);
    } else if (is_string(obj)) {
        return size_helper(string, sizeof(char) * obj->value.string.len);
    } else if (is_string_buffer(obj)) {
        return size_helper(string_buffer, 0);
    } else if (is_vector(obj)) {
        return size_helper(vector, sizeof(object) * obj->value.vector.len);
    } else if (is_stack(obj)) {
        return size_helper(stack, sizeof(object) * obj->value.stack.size);
    } else if (is_string_port(obj)) {
        return size_helper(string_port, 0);
    } else if (is_stdio_port(obj)) {
        return size_helper(stdio_port, 0);
    } else if (is_hashset(obj)) {
        return size_helper(hashset, 0);
    } else if (is_hashmap(obj)) {
        return size_helper(hashmap, 0);
    } else if (is_weak_ref(obj)) {
        return size_helper(weak_ref, 0);
    } else if (is_weak_hashset(obj)) {
        return size_helper(weak_hashset, 0);
    } else if (is_stack_frame(obj)) {
        return size_helper(stack_frame, 0);
    } else if (is_env_slot(obj)) {
        return size_helper(env_slot, 0);
    } else if (is_proc(obj)) {
        return size_helper(proc, 0);
    } else if (is_syntax(obj)) {
        return size_helper(syntax, 0);
    }
    // todo ��ʵ�ֵ����ͼǵ��޸� object_bootstrap_sizeof

    // δ֪������
    assert((0 && "UNKNOWN_OBJECT_TYPE"));
#pragma pop_macro("size_helper")
    return 0;
}


/**
                             malloc & free ��װ
******************************************************************************/

/**
 * malloc() �ķ�װ
 * @param size �ֽ���
 * @return ������ڴ��, Ϊ�������ʧ��
 */
EXPORT_API OUT NULLABLE void *raw_alloc(IN size_t size) {
    void *obj = malloc(size);
    notnull_or_return(obj, "raw_alloc() failed", NULL);
    // �����ڴ�
    memset(obj, 0, size);
    // ��ַ���뵽 8bytes
    assert_aligned_ptr_check(obj);
    return obj;
}

/**
 * free() �ķ�װ
 * @param obj raw_alloc() ������ڴ�
 */
EXPORT_API void raw_free(IN NOTNULL void *obj) {
    assert(obj != NULL);
    assert_aligned_ptr_check(obj);
    free(obj);
}

/**
 * realloc() �ķ�װ
 * @param obj raw_alloc() ������ڴ�
 */
EXPORT_API void *raw_realloc(IN NOTNULL void *obj, size_t new_size) {
    assert(obj != NULL);
    assert_aligned_ptr_check(obj);

    void *mem = realloc(obj, new_size);
    notnull_or_return(obj, "raw_realloc() failed", NULL);
    // ��ַ���뵽 8bytes
    assert_aligned_ptr_check(obj);

    return mem;
}

/**
                               ����ֵ����
******************************************************************************/

/**
 * ��ȡ i64 �����ֵ
 * @param i64
 * @return
 */
EXPORT_API OUT int64_t i64_getvalue(REF NOTNULL object i64) {
    assert(i64 != NULL);

    if (is_object(i64)) {
        return i64->value.i64;
    } else {
        // ��������
        return i64_arithmetic_right_shift(ptr_to_intptr(i64), I64_EXTENDED_BITS);
    }
}

/**
 * �ж� object �Ƿ�Ϊ i64
 * @param i64
 * @return !0 -> true, 0 -> false
 */
EXPORT_API OUT int is_i64(REF NULLABLE object i64) {
    if (is_imm_i64(i64)) return 1;
    else if (is_object(i64)) return i64->type == OBJ_I64;
    else return 0;
}