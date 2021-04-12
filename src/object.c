#include <paper-scheme/object.h>


/**
                                对象值操作
******************************************************************************/
/**
 * 返回栈顶元素
 * @param stack
 * @return object 如果栈为空, 返回 NULL
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
 * 入栈
 * @param stack
 * @param obj
 * @return 如果栈满, 返回 0, 否则返回 1
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
        // 栈满
        return 0;
    }
}
/**
 * 出栈
 * @param stack
 * @return 如果栈空, 返回 0; 否则返回 1
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
                               对象操作 API
******************************************************************************/


/**
 * 计算对齐后对象的大小
 * 用法: aligned_size(object_sizeof_base(value_field))
 * 给定一个大小, 返回对齐到 1<<ALIGN_BITS 的大小
 * @param unaligned_size
 * @return
 */
EXPORT_API OUT size_t aligned_size(IN size_t unaligned_size) {
    size_t need_aligned = unaligned_size & (size_t) ALIGN_MASK;
    size_t real_alloc_size = unaligned_size & (~((size_t) ALIGN_MASK));
    if (need_aligned) {
        // 如果需要对齐, 向大进位一个 ALIGN_SIZE
        real_alloc_size += ALIGN_SIZE;
    }

    return real_alloc_size;
}

/**
 * 运行时计算对象大小, 单位 bytes, 提供给 gc 使用
 * @param object NOTNULL 不能是立即数或空指针
 * @return 如果参数非法, 返回 0
 */
EXPORT_API OUT OUT size_t object_bootstrap_sizeof(REF NOTNULL object obj) {
    assert(obj != NULL);
    assert(is_object(obj));

    if (is_imm(obj)) {
        // 不应该对立即数执行这个方法
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
    // todo 新实现的类型记得修改 object_bootstrap_sizeof

    // 未知的类型
    assert((0 && "UNKNOWN_OBJECT_TYPE"));
#pragma pop_macro("size_helper")
    return 0;
}


/**
                             malloc & free 封装
******************************************************************************/

/**
 * malloc() 的封装
 * @param size 字节数
 * @return 分配的内存块, 为空则分配失败
 */
EXPORT_API OUT NULLABLE void *raw_alloc(IN size_t size) {
    void *obj = malloc(size);
    notnull_or_return(obj, "raw_alloc() failed", NULL);
    // 清零内存
    memset(obj, 0, size);
    // 地址对齐到 8bytes
    assert_aligned_ptr_check(obj);
    return obj;
}

/**
 * free() 的封装
 * @param obj raw_alloc() 分配的内存
 */
EXPORT_API void raw_free(IN NOTNULL void *obj) {
    assert(obj != NULL);
    assert_aligned_ptr_check(obj);
    free(obj);
}

/**
 * realloc() 的封装
 * @param obj raw_alloc() 分配的内存
 */
EXPORT_API void *raw_realloc(IN NOTNULL void *obj, size_t new_size) {
    assert(obj != NULL);
    assert_aligned_ptr_check(obj);

    void *mem = realloc(obj, new_size);
    notnull_or_return(obj, "raw_realloc() failed", NULL);
    // 地址对齐到 8bytes
    assert_aligned_ptr_check(obj);

    return mem;
}

/**
                               对象值操作
******************************************************************************/

/**
 * 获取 i64 对象的值
 * @param i64
 * @return
 */
EXPORT_API OUT int64_t i64_getvalue(REF NOTNULL object i64) {
    assert(i64 != NULL);

    if (is_object(i64)) {
        return i64->value.i64;
    } else {
        // 算术右移
        return i64_arithmetic_right_shift(ptr_to_intptr(i64), I64_EXTENDED_BITS);
    }
}

/**
 * 判断 object 是否为 i64
 * @param i64
 * @return !0 -> true, 0 -> false
 */
EXPORT_API OUT int is_i64(REF NULLABLE object i64) {
    if (is_imm_i64(i64)) return 1;
    else if (is_object(i64)) return i64->type == OBJ_I64;
    else return 0;
}