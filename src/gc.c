#include <paper-scheme/gc.h>


/**
                               �������� API
******************************************************************************/

static void gc_mark_stack_push(context_t context, object *start, object *end) {
    if (context->mark_stack_top == NULL) {
        // ��ʼ״̬, ջ��ָ��Ϊ��, ��ջ��ָ��ָ��ջ��, ��������һ���ڵ�Ϊ��
        context->mark_stack_top = context->mark_stack;
        context->mark_stack_top->prev = NULL;
    } else if (context->mark_stack_top >= context->mark_stack &&
               context->mark_stack_top + 1 < context->mark_stack + MAX_MARK_STACK_DEEP) {
        // ���Ĭ��ջ��ȹ���, ֱ������ջָ�뼴��
        gc_mark_stack_ptr old = context->mark_stack_top;
        context->mark_stack_top++;
        context->mark_stack_top->prev = old;
    } else {
        // Ĭ��ջ��Ȳ���, ��������ڴ�, ����ʧ����ͣ��
        gc_mark_stack_ptr new_mark_stack_node = (gc_mark_stack_ptr) raw_alloc(sizeof(struct gc_mark_stack_node_t));
        if (new_mark_stack_node == NULL) {
            fprintf(context->context_stderr, "gc stack too deep.\n");
            exit(EXIT_FAILURE_MALLOC_FAILED);
        }
        new_mark_stack_node->prev = context->mark_stack_top;
        context->mark_stack_top = new_mark_stack_node;
    }

    context->mark_stack_top->start = start;
    context->mark_stack_top->end = end;
}

static void gc_mark_stack_pop(context_t context) {
    // ����ջָ��
    gc_mark_stack_ptr old = context->mark_stack_top;
    context->mark_stack_top = context->mark_stack_top->prev;

    // �ɵ�ջ��ָ�����ָ��Ĭ��ջ����Ŀռ�, ��˵����Ҫ�ͷ�
    if (old < context->mark_stack || old >= context->mark_stack + MAX_MARK_STACK_DEEP) {
        printf("old %p, stack %p, top %p\n", old, context->mark_stack, context->mark_stack_top);
        raw_free(old);
    }
}

static void gc_mark_one(context_t context, object obj) {
    object *field_start_ptr;
    object *field_end_ptr;
    size_t field_len = 0;

    do {
        if (!is_object(obj) || is_marked(obj)) return;

        obj->marked = 1;
        object_type_info_ptr t = context_get_object_type(context, obj);

        field_len = object_type_info_member_slots_of(t, obj);
        if (field_len > 0) {
            field_start_ptr = type_info_get_object_ptr_of_first_member(t, obj);
            field_end_ptr = field_start_ptr + (field_len - 1);

            while (field_start_ptr < field_end_ptr && (is_object(*field_end_ptr) ? is_marked(*field_end_ptr) : 1))
                field_end_ptr--;    // �����Ƕ���ͱ�ǹ��Ķ���

            while (field_start_ptr < field_end_ptr && *field_end_ptr == field_end_ptr[-1])
                field_end_ptr--;    // �����ظ�����

            // ��ʱҪô  (field_end_ptr - field_start_ptr) > 0      =>    ʣ�����ֶ�
            // Ҫô      (field_end_ptr - field_start_ptr) == 0     =>    ʣ��һ�� �ֶ�
            // ���ں���, obj = *field_end_ptr,
            //      ���ǵ���ʱ����Ϊ������ṹ (���� scheme ��˵�����ټ�), ����ѭ��, ���� mark stack ��ռ��
            // ����ǰ��,
            //      gc_mark_stack_push(), break;
            if (field_start_ptr < field_end_ptr) {
                gc_mark_stack_push(context, field_start_ptr, field_end_ptr);
                return;
            } else {
                obj = *field_end_ptr;
            }
        } else {
            // field_len <= 0, ����Ҫ�������
            return;
        }
    } while (1);
}

static void gc_mark_one_start(context_t context, object obj) {
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
 * �ڲ����� ��Ǵ�����
 * @param context
 * @return <li>IMM_TRUE: ���гɹ�</li><li>IMM_FALSE: ����ʧ��</li>
 */
static object gc_mark(context_t context) {
    assert(context != NULL);

    // todo context �޸ĺ�, �޸�gc_mark

    // ȫ�ַ��ű�
    for (size_t i = 0; i < context->global_type_table_len; i++) {
        gc_mark_one_start(context, context->global_type_table[0].name);
        gc_mark_one_start(context, context->global_type_table[0].getter);
        gc_mark_one_start(context, context->global_type_table[0].setter);
        gc_mark_one_start(context, context->global_type_table[0].to_string);
    }

    // env & dump stack ?

    // ��ʱ����������
    for (gc_saves_list_t save = context->saves; save != NULL; save = save->next) {
        gc_mark_one_start(context, *save->illusory_object);
    }
    return IMM_TRUE;
}

/**
 * ����������������
 * @param context
 * @return <li>IMM_TRUE: ���гɹ�</li><li>IMM_FALSE: ����ʧ��</li>
 */
EXPORT_API CHECKED object gc_collect(REF NOTNULL context_t context) {
    assert(context != NULL);
    if (!context->gc_collect_on) return IMM_TRUE;

    // TODO ʵ�� gc_collect
    gc_mark(context);
    return IMM_TRUE;
}

/**
 * ���ԴӶ��з����ڴ�, ���ᴥ�� gc, ʧ�ܷ��� NULL
 * @param heap �ѽṹ
 * @param size Ҫ����Ķ����С
 * @return <li>NULL: �Ҳ����㹻��Ŀ��пؼ�</li>
 */
EXPORT_API OUT CHECKED object gc_try_alloc(REF NOTNULL context_t context, IN size_t size) {
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
EXPORT_API OUT object gc_alloc(REF NOTNULL context_t context, IN size_t size) {
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
        fprintf(context->context_stderr, "[ERROR] Out of Memory:");
        fprintf(context->context_stderr, " heap total size 0x%zx, try to growth to 0x%zx, max heap size 0x%zx\n",
                heap->total_size, heap->last_node->chunk_size * heap->growth_scale + heap->total_size, heap->max_size);
        exit(EXIT_FAILURE_OUT_OF_MEMORY);
        //return IMM_FALSE;
    } else {
        fprintf(context->context_stderr, "[ERROR] malloc() failed:");
        fprintf(context->context_stderr, " heap total size 0x%zx, try to growth to 0x%zx, max heap size 0x%zx\n",
                heap->total_size, heap->last_node->chunk_size * heap->growth_scale + heap->total_size, heap->max_size);
        exit(EXIT_FAILURE_MALLOC_FAILED);
        //return IMM_UNIT;
    }

}