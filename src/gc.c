#include <paper-scheme/gc.h>


/**
                               垃圾回收 API
******************************************************************************/

static void gc_mark_stack_push(context_t context, object *start, object *end) {
    if (context->mark_stack_top == NULL) {
        // 初始状态, 栈顶指针为空, 让栈顶指针指向栈底, 再设置上一个节点为空
        context->mark_stack_top = context->mark_stack;
        context->mark_stack_top->prev = NULL;
    } else if (context->mark_stack_top >= context->mark_stack &&
               context->mark_stack_top + 1 < context->mark_stack + MAX_MARK_STACK_DEEP) {
        // 如果默认栈深度够大, 直接增加栈指针即可
        gc_mark_stack_ptr old = context->mark_stack_top;
        context->mark_stack_top++;
        context->mark_stack_top->prev = old;
    } else {
        // 默认栈深度不足, 额外分配内存, 分配失败则停机
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
    // 更新栈指针
    gc_mark_stack_ptr old = context->mark_stack_top;
    context->mark_stack_top = context->mark_stack_top->prev;

    // 旧的栈顶指针如果指向默认栈以外的空间, 则说明需要释放
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
                field_end_ptr--;    // 跳过非对象和标记过的对象

            while (field_start_ptr < field_end_ptr && *field_end_ptr == field_end_ptr[-1])
                field_end_ptr--;    // 跳过重复对象

            // 此时要么  (field_end_ptr - field_start_ptr) > 0      =>    剩余多个字段
            // 要么      (field_end_ptr - field_start_ptr) == 0     =>    剩余一个 字段
            // 对于后者, obj = *field_end_ptr,
            //      考虑到此时可能为单链表结构 (对于 scheme 来说并不少见), 继续循环, 减少 mark stack 的占用
            // 对于前者,
            //      gc_mark_stack_push(), break;
            if (field_start_ptr < field_end_ptr) {
                gc_mark_stack_push(context, field_start_ptr, field_end_ptr);
                return;
            } else {
                obj = *field_end_ptr;
            }
        } else {
            // field_len <= 0, 不需要继续检查
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
 * 内部方法 标记存活对象
 * @param context
 * @return <li>IMM_TRUE: 运行成功</li><li>IMM_FALSE: 运行失败</li>
 */
static object gc_mark(context_t context) {
    assert(context != NULL);

    // todo context 修改后, 修改gc_mark

    // 全局符号表
    for (size_t i = 0; i < context->global_type_table_len; i++) {
        gc_mark_one_start(context, context->global_type_table[0].name);
        gc_mark_one_start(context, context->global_type_table[0].getter);
        gc_mark_one_start(context, context->global_type_table[0].setter);
        gc_mark_one_start(context, context->global_type_table[0].to_string);
    }

    // env & dump stack ?

    // 临时变量保护表
    for (gc_saves_list_t save = context->saves; save != NULL; save = save->next) {
        gc_mark_one_start(context, *save->illusory_object);
    }
    return IMM_TRUE;
}

/**
 * 启动回收垃圾回收
 * @param context
 * @return <li>IMM_TRUE: 运行成功</li><li>IMM_FALSE: 运行失败</li>
 */
EXPORT_API CHECKED object gc_collect(REF NOTNULL context_t context) {
    assert(context != NULL);
    if (!context->gc_collect_on) return IMM_TRUE;

    // TODO 实现 gc_collect
    gc_mark(context);
    return IMM_TRUE;
}

/**
 * 尝试从堆中分配内存, 不会触发 gc, 失败返回 NULL
 * @param heap 堆结构
 * @param size 要分配的对象大小
 * @return <li>NULL: 找不到足够大的空闲控件</li>
 */
EXPORT_API OUT CHECKED object gc_try_alloc(REF NOTNULL context_t context, IN size_t size) {
    assert(context != NULL);
    assert(context->heap != NULL);
    assert(context->heap->first_node != NULL);
    assert(context->heap->last_node != NULL);
    assert_aligned_size_check(size);

    heap_t heap = context->heap;
    object obj = NULL;

    // 链表中搜索
    for (heap_node_t node = heap->first_node; node != NULL; node = node->next) {
        assert(node->data != NULL);
        assert(node->free_ptr != NULL);
        assert(node->free_ptr >= node->data);

        size_t used_space = node->free_ptr - node->data;
        size_t free_space = node->chunk_size - used_space;
        if (free_space >= size) {
            // 找到空闲位置
            obj = (object) node->free_ptr;
            // 后移 free_ptr
            node->free_ptr += size;
            break;
        }
    }

    // 找不到的话会返回 NULL
    return obj;
}

/**
 * 从堆中分配内存
 * <p>注意, 到了底层与托管内存的分界线了</p>
 * <p>!此函数失败会直接结束进程</p>
 * @param heap 堆结构
 * @param size 要分配的对象大小
 * @return
 * <li>exit(EXIT_FAILURE_OUT_OF_MEMORY): 达到最大堆大小</li>
 * <li>exit(EXIT_FAILURE_MALLOC_FAILED): 未达到最大堆大小, 但是系统内存不足</li>
 */
EXPORT_API OUT object gc_alloc(REF NOTNULL context_t context, IN size_t size) {
    assert(context != NULL);
    assert(context->heap != NULL);
    assert_aligned_size_check(size);

    // 1. 第一次尝试分配
    object obj = gc_try_alloc(context, size);
    // 分配成功, 返回
    if (obj != NULL) {
        return obj;
    }

    // 2. obj 为空, 且需要 gc, 进行一次 gc
    gc_collect(context);

    // 3. gc 后第二次尝试分配
    obj = gc_try_alloc(context, size);
    // 分配成功, 反回
    if (obj != NULL) {
        return obj;
    }

    // 4. 堆空间不足, 尝试增长堆空间后分配
    object heap_grow_result = heap_grow(context->heap);
    obj = gc_try_alloc(context, size);
    // 多次尝试, 直到增长失败或分配成功为止
    while ((obj == NULL) && (heap_grow_result == IMM_TRUE)) {
        heap_grow_result = heap_grow(context->heap);
        obj = gc_try_alloc(context, size);
    }

    // 5. 最终阶段.
    // 如果发现分配成功, 返回 obj
    if (obj != NULL) {
        return obj;
    }

    // 6. 永远不会分配失败. 失败时结束运行
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