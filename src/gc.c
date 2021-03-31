#include <paper-scheme/gc.h>


/**
                               �������� API
******************************************************************************/
/**
 * ��һ�������һ��
 * @param context
 * @param start
 * @param end
 */
static void gc_mark_stack_push(context_t context, object *start, object *end) {
    assert(context != NULL);

    if (context->mark_stack_top == NULL) {
        // ��ʼ״̬, ջ��ָ��Ϊ��, ��ջ��ָ��ָ��ջ��, ��������һ���ڵ�Ϊ��
        context->mark_stack_top = context->mark_stack;
        context->mark_stack_top->prev = NULL;
    } else if (context->mark_stack_top >= context->mark_stack &&
               context->mark_stack_top + 1 < context->mark_stack + GC_MAX_MARK_STACK_DEEP) {
        // ���Ĭ��ջ��ȹ���, ֱ������ջָ�뼴��
        gc_mark_stack_ptr old = context->mark_stack_top;
        context->mark_stack_top++;
        context->mark_stack_top->prev = old;
    } else {
        // Ĭ��ջ��Ȳ���, ��������ڴ�, ����ʧ����ͣ��
        gc_mark_stack_ptr new_mark_stack_node = (gc_mark_stack_ptr) raw_alloc(sizeof(struct gc_mark_stack_node_t));
        if (new_mark_stack_node == NULL) {
            fprintf(context->err_out_port, "gc stack too deep.\n");
            exit(EXIT_FAILURE_MALLOC_FAILED);
        }
        new_mark_stack_node->prev = context->mark_stack_top;
        context->mark_stack_top = new_mark_stack_node;
    }

    context->mark_stack_top->start = start;
    context->mark_stack_top->end = end;
}

static void gc_mark_stack_pop(context_t context) {
    assert(context != NULL);

    // ����ջָ��
    gc_mark_stack_ptr old = context->mark_stack_top;
    context->mark_stack_top = context->mark_stack_top->prev;

    // �ɵ�ջ��ָ�����ָ��Ĭ��ջ����Ŀռ�, ��˵����Ҫ�ͷ�
    if (old < context->mark_stack || old >= context->mark_stack + GC_MAX_MARK_STACK_DEEP) {
        printf("old %p, stack %p, top %p\n", old, context->mark_stack, context->mark_stack_top);
        raw_free(old);
    }
}

/**
 * ���һ������, ����ѹ�� mark stack
 * @param context
 * @param obj
 */
static void gc_mark_one(context_t context, object obj) {
    assert(context != NULL);

    object *field_start_ptr;
    object *field_end_ptr;
    size_t field_len = 0;

    do {
        if (!is_object(obj) || is_marked(obj)) return;

        // ����������
        if (is_weak_ref(obj)) {
            obj->marked = 1;
            // �����ö���ĳ�Ա����Ҫ���, ͬʱ��ɨ�赽�������ü�������������
            obj->value.weak_ref._internal_next_ref = context->weak_ref_chain;
            context->weak_ref_chain = obj;
            return;
        }

        obj->marked = 1;
        object_type_info_ptr t = context_get_object_type(context, obj);

        field_len = object_type_info_member_slots_of(t, obj);
        if (field_len > 0) {
            field_start_ptr = type_info_get_object_ptr_of_first_member(t, obj);
            field_end_ptr = field_start_ptr + (field_len - 1);

            // �����Ƕ���ͱ�ǹ��Ķ���
            while (field_start_ptr < field_end_ptr && (is_object(field_end_ptr[0]) ? is_marked(field_end_ptr[0]) : 1))
                field_end_ptr--;
            while (field_start_ptr < field_end_ptr &&
                   (is_object(field_start_ptr[0]) ? is_marked(field_start_ptr[0]) : 1))
                field_start_ptr++;

            // �����ظ�����
            while (field_start_ptr < field_end_ptr && field_end_ptr[0] == field_end_ptr[-1])
                field_end_ptr--;
            while (field_start_ptr < field_end_ptr && field_start_ptr[0] == field_start_ptr[1])
                field_start_ptr++;

            if (field_start_ptr < field_end_ptr) {
                gc_mark_stack_push(context, field_start_ptr, field_end_ptr);
            }
            // ���ǵ���ʱ����Ϊ pair �ṹ (���� scheme ��˵�����ټ�), ����ѭ��, ���� mark stack ��ռ��
            // �����������
            obj = *field_end_ptr;
        }
    } while (1);
}

/**
 * ��һ������ʼ����
 * @param context
 * @param obj
 */
static void gc_mark_one_start(context_t context, object obj) {
    assert(context != NULL);

    gc_mark_one(context, obj);
    while (context->mark_stack_top != NULL) {
        object *start = context->mark_stack_top->start;
        object *end = context->mark_stack_top->end;
        gc_mark_stack_pop(context);
        for (; start <= end; start++) {
            gc_mark_one(context, *start);
        }
    }
}

/**
 * �ڲ����� 1. ��Ǵ�����
 * <p>��һ��ͼ����</p>
 * @param context
 * @return <li>IMM_TRUE: ���гɹ�</li>
 * <li>IMM_FALSE: ����ʧ��</li>
 */
static object gc_mark(context_t context) {
    assert(context != NULL);

    // todo context �޸ĺ�, �޸�gc_mark

    // ����������������
    context->weak_ref_chain = NULL;

    // ��ǼĴ���
    gc_mark_one_start(context, context->args);
    gc_mark_one_start(context, context->code);
    gc_mark_one_start(context, context->current_env);
    gc_mark_one_start(context, context->scheme_stack);

    // ��Ƿ���ֵ
    gc_mark_one_start(context, context->value);
    // ��� load stack
    gc_mark_one_start(context, context->load_stack);

    // ���ȫ�ַ��ű�
    gc_mark_one_start(context, context->global_symbol_table);
    // ���ȫ�� environment
    gc_mark_one_start(context, context->global_environment);


    // ȫ������
    for (size_t i = 0; i < context->global_type_table_len; i++) {
        gc_mark_one_start(context, context->global_type_table[i].name);
        gc_mark_one_start(context, context->global_type_table[i].getter);
        gc_mark_one_start(context, context->global_type_table[i].setter);
        gc_mark_one_start(context, context->global_type_table[i].to_string);
    }

    // C ����ջ�ϱ���������
    for (gc_saves_list_t save = context->saves; save != NULL; save = save->next) {
        // ʹ�� macro gc_var_n() ʱ, ���뱣֤��һ�ε��ûᴥ�� GC �ĺ���ǰ�����ʼ������ֵ
        if (save->illusory_object != NULL && is_object(*save->illusory_object)) {
            gc_mark_one_start(context, *save->illusory_object);
        }
    }
    return IMM_TRUE;
}

/**
 * �ڲ����� 2. ����������
 * <p>ע��, �������޷�������Ƿ�����, ����������������������ǲ��ɿ���</p>
 * @param context
 * @param context
 */
static void gc_reset_weak_references(context_t context) {
    object weak_ref = context->weak_ref_chain;
    object tmp = NULL;
    while (weak_ref != NULL) {
        assert(is_weak_ref(weak_ref));
        object ref = weak_ref_get(weak_ref);
        if (is_object(ref) && !is_marked(ref)) {
            // weak ref ���õĶ���δ���ʱ����Ϊ��
            weak_ref_get(weak_ref) = NULL;
        }
        tmp = weak_ref;
        weak_ref = weak_ref->value.weak_ref._internal_next_ref;
        tmp->value.weak_ref._internal_next_ref = NULL;
    }

    context->weak_ref_chain = NULL;
}

/**
 * �ڲ����� 3. ������ն�����ַ������ object->forwarding �ֶ�,
 * ͬʱ���ж���� finalize
 * <p>��һ�ζѱ���</p>
 * @param context
 */
static void gc_set_forwarding(context_t context) {
    /*
     * ͼ��:
     *      =====: �ѷ����ڴ�����
     *           : �����ڴ�����
     *      *****: ѹ������Ķ���
     * ��ʼ:
     *            _____________________________________
     * node->data |======================|               | node->free_ptr
     *            -------------------------------------
     *            |
     *         to from
     * ����������:
     *            _____________________________________
     * node->data |*****|================|               | node->free_ptr
     *            -------------------------------------
     *                  |
     *               to & from
     * ������������:
     *            _____________________________________
     * node->data |*****|------|========|               | node->free_ptr
     *            -------------------------------------
     *                  |     |
     *                 to    from
     * ɨ�����:
     *            _____________________________________
     * node->data |*******|-------------|              | node->free_ptr
     *            -------------------------------------
     *                   |            |
     *                  to           from
     */
    heap_t heap = context->heap;
    for (heap_node_t node = heap->first_node; node != NULL; node = node->next) {
        char *to = node->data;
        char *from = node->data;

        while (from < node->free_ptr) {
            object obj = (object) from;
            assert(is_object(obj));

            size_t size = context_object_sizeof(context, obj);
            if (is_marked(obj)) {
                //printf("alive: type=%d\n", obj->type);
                obj->forwarding = (object) to;
                to += size;
            } else {
                // ִ�ж�����������
                //printf("died:  type=%d\n", obj->type);
                proc_1 finalizer = context_get_object_finalize(context, obj);
                if (finalizer != NULL) {
                    finalizer(context, obj);
                }
            }
            from += size;
        }

        assert(from == node->free_ptr);
    }
}

#define gc_adjust_ref_one(obj) \
    do { \
        if (is_object(*(obj))) { \
            *(obj) = (*(obj))->forwarding; \
        } \
    } while (0);

/**
 * �ڲ����� 4. ���� object->forwarding �ֶθ��¶�������
 * <p>ע��! ����ƻ��ɵ����ù�ϵ</p>
 * <p>�ڶ��ζѱ���</p>
 * @param context
 */
static void gc_adjust_ref(context_t context) {
    assert(context != NULL);

    // todo context �޸ĺ�, �޸� gc_adjust_ref, ���¸�����

    // ���¸�����

    // �Ĵ���
    gc_adjust_ref_one(&(context->args));
    gc_adjust_ref_one(&(context->code));
    gc_adjust_ref_one(&(context->current_env));
    gc_adjust_ref_one(&(context->scheme_stack));

    // ����ֵ
    gc_adjust_ref_one(&(context->value));
    // load stack
    gc_adjust_ref_one(&(context->load_stack));

    // ���� global_symbol_table
    gc_adjust_ref_one(&(context->global_symbol_table));
    // ���� global_environment
    gc_adjust_ref_one(&(context->global_environment));

    // ȫ��������Ϣ
    for (size_t i = 0; i < context->global_type_table_len; i++) {
        object_type_info_ptr t = &context->global_type_table[i];
        if (is_object(t->name))
            t->name = t->name->forwarding;
        if (is_object(t->getter))
            t->getter = t->getter->forwarding;
        if (is_object(t->setter))
            t->setter = t->setter->forwarding;
        if (is_object(t->to_string))
            t->to_string = t->to_string->forwarding;
    }

    // C ����ջ�ϱ���������
    for (gc_saves_list_t save = context->saves; save != NULL; save = save->next) {
        if (save->illusory_object != NULL && is_object(*save->illusory_object)) {
            *save->illusory_object = (*save->illusory_object)->forwarding;
        }
    }

    // �������Ѹ�����������
    heap_t heap = context->heap;
    for (heap_node_t node = heap->first_node; node != NULL; node = node->next) {
        char *ptr = node->data;

        while (ptr < node->free_ptr) {
            object obj = (object) ptr;
            assert(is_object(obj));
            size_t size = context_object_sizeof(context, obj);

            if (is_marked(obj)) {
                // ���ڴ��Ķ���, ������г�Ա����
                object_type_info_ptr t = context_get_object_type(context, obj);
                size_t field_len = object_type_info_member_slots_of(t, obj);
                if (field_len > 0) {
                    object *field_start_ptr = type_info_get_object_ptr_of_first_member(t, obj);
                    object *field_end_ptr = field_start_ptr + (field_len - 1);

                    for (object *obj_ptr = field_start_ptr; obj_ptr <= field_end_ptr; obj_ptr++) {
                        if (is_object(*obj_ptr)) {
                            *obj_ptr = (*obj_ptr)->forwarding;
                        }
                    }
                }
            }

            ptr += size;
        }

        assert(ptr == node->free_ptr);
    }
}

/**
 * �ڲ����� 5. �ƶ�����
 * <p>�����ζѱ���</p>
 * @param context
 */
static void move_objects(context_t context) {
    heap_t heap = context->heap;
    for (heap_node_t node = heap->first_node; node != NULL; node = node->next) {
        char *new_free_ptr = node->data;
        char *ptr = node->data;

        // �ƶ�����
        while (ptr < node->free_ptr) {
            object obj = (object) ptr;
            assert(is_object(obj));

            size_t size = context_object_sizeof(context, obj);
            if (is_marked(obj)) {
                assert(obj->forwarding != NULL);
//                printf("alive: type=%d\n", obj->type);

                // ֻ�е����󱻱��, ��ת����ַ�����ڵ�ǰ��ַʱ�Ž����ƶ�
                obj->marked = 0;    // ������
                if (obj->forwarding != obj) {
                    memcpy(obj->forwarding, obj, size);
                }
                new_free_ptr += size;
            } else {
//                printf("dead: type = %d\n", obj->type);
            }
            ptr += size;
        }

        assert(node->free_ptr >= new_free_ptr);
        // ����ռ�
        memset(new_free_ptr, 0, node->free_ptr - new_free_ptr);
        // ���� free_ptr ָ��
        node->free_ptr = new_free_ptr;
    }
}

/**
 * ����������������
 * @param context
 * @return <li>IMM_TRUE: ���гɹ�</li><li>IMM_FALSE: ����ʧ��</li>
 */
EXPORT_API CHECKED GC object gc_collect(REF NOTNULL context_t context) {
    assert(context != NULL);
    if (!context->gc_collect_on) return IMM_TRUE;

#if USE_DEBUG_GC
#endif

    context->gc_happened = 1;

    gc_mark(context);
    // ����������
    gc_reset_weak_references(context);
    gc_set_forwarding(context);
    gc_adjust_ref(context);
    move_objects(context);

#if USE_DEBUG_GC
#endif

    return IMM_TRUE;
}

/**
 * ���ԴӶ��з����ڴ�, ���ᴥ�� gc, ʧ�ܷ��� NULL
 * @param heap �ѽṹ
 * @param size Ҫ����Ķ����С
 * @return <li>NULL: �Ҳ����㹻��Ŀ��пؼ�</li>
 */
EXPORT_API OUT CHECKED GC object gc_try_alloc(REF NOTNULL context_t context, IN size_t size) {
    assert(context != NULL);
    assert(context->heap != NULL);
    assert(context->heap->first_node != NULL);
    assert(context->heap->last_node != NULL);
    assert_aligned_size_check(size);

    heap_t heap = context->heap;
    object obj = NULL;

    // ����������
    for (heap_node_t node = heap->first_node; node != NULL; node = node->next) {
        assert(node->data != NULL);
        assert(node->free_ptr != NULL);
        assert(node->free_ptr >= node->data);

        size_t used_space = node->free_ptr - node->data;
        size_t free_space = node->chunk_size - used_space;
        if (free_space >= size) {
            // �ҵ�����λ��
            obj = (object) node->free_ptr;
            // ���� free_ptr
            node->free_ptr += size;
            break;
        }
    }

    // �Ҳ����Ļ��᷵�� NULL
    return obj;
}

/**
 * �Ӷ��з����ڴ�
 * <p>ע��, ���˵ײ����й��ڴ�ķֽ�����</p>
 * <p>!�˺���ʧ�ܻ�ֱ�ӽ�������</p>
 * @param heap �ѽṹ
 * @param size Ҫ����Ķ����С
 * @return
 * <li>exit(EXIT_FAILURE_OUT_OF_MEMORY): �ﵽ���Ѵ�С</li>
 * <li>exit(EXIT_FAILURE_MALLOC_FAILED): δ�ﵽ���Ѵ�С, ����ϵͳ�ڴ治��</li>
 */
EXPORT_API OUT GC object gc_alloc(REF NOTNULL context_t context, IN size_t size) {
    assert(context != NULL);
    assert(context->heap != NULL);
    assert_aligned_size_check(size);

    // 1. ��һ�γ��Է���
    object obj = gc_try_alloc(context, size);
    // ����ɹ�, ����
    if (obj != NULL) {
        return obj;
    }

    // 2. obj Ϊ��, ����Ҫ gc, ����һ�� gc
    gc_collect(context);

    // 3. gc ��ڶ��γ��Է���
    obj = gc_try_alloc(context, size);
    // ����ɹ�, ����
    if (obj != NULL) {
        return obj;
    }

    // 4. �ѿռ䲻��, ���������ѿռ�����
    object heap_grow_result = heap_grow(context->heap);
    obj = gc_try_alloc(context, size);
    // ��γ���, ֱ������ʧ�ܻ����ɹ�Ϊֹ
    while ((obj == NULL) && (heap_grow_result == IMM_TRUE)) {
        heap_grow_result = heap_grow(context->heap);
        obj = gc_try_alloc(context, size);
    }

    // 5. ���ս׶�.
    // ������ַ���ɹ�, ���� obj
    if (obj != NULL) {
        return obj;
    }

    // 6. ��Զ�������ʧ��. ʧ��ʱ��������
    heap_t heap = context->heap;
    if (heap_grow_result == IMM_FALSE) {
        fprintf(context->err_out_port, "[ERROR] Out of Memory:");
        fprintf(context->err_out_port, " heap total size 0x%zx, try to growth to 0x%zx, max heap size 0x%zx\n",
                heap->total_size, heap->last_node->chunk_size * heap->growth_scale + heap->total_size, heap->max_size);
        exit(EXIT_FAILURE_OUT_OF_MEMORY);
        //return IMM_FALSE;
    } else {
        fprintf(context->err_out_port, "[ERROR] malloc() failed:");
        fprintf(context->err_out_port, " heap total size 0x%zx, try to growth to 0x%zx, max heap size 0x%zx\n",
                heap->total_size, heap->last_node->chunk_size * heap->growth_scale + heap->total_size, heap->max_size);
        exit(EXIT_FAILURE_MALLOC_FAILED);
        //return IMM_UNIT;
    }

}