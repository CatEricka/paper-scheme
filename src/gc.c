#include <paper-scheme/gc.h>


/**
                               垃圾回收 API
******************************************************************************/
/**
 * 第一个和最后一个
 * @param context
 * @param start
 * @param end
 */
static void gc_mark_stack_push(context_t context, object *start, object *end) {
    assert(context != NULL);

    if (context->mark_stack_top == NULL) {
        // 初始状态, 栈顶指针为空, 让栈顶指针指向栈底, 再设置上一个节点为空
        context->mark_stack_top = context->mark_stack;
        context->mark_stack_top->prev = NULL;
    } else if (context->mark_stack_top >= context->mark_stack &&
               context->mark_stack_top + 1 < context->mark_stack + GC_MAX_MARK_STACK_DEEP) {
        // 如果默认栈深度够大, 直接增加栈指针即可
        gc_mark_stack_ptr old = context->mark_stack_top;
        context->mark_stack_top++;
        context->mark_stack_top->prev = old;
    } else {
        // 默认栈深度不足, 额外分配内存, 分配失败则停机
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

    // 更新栈指针
    gc_mark_stack_ptr old = context->mark_stack_top;
    context->mark_stack_top = context->mark_stack_top->prev;

    // 旧的栈顶指针如果指向默认栈以外的空间, 则说明需要释放
    if (old < context->mark_stack || old >= context->mark_stack + GC_MAX_MARK_STACK_DEEP) {
        printf("old %p, stack %p, top %p\n", old, context->mark_stack, context->mark_stack_top);
        raw_free(old);
    }
}

/**
 * 标记一个对象, 引用压入 mark stack
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

        // 处理弱引用
        if (is_weak_ref(obj)) {
            obj->marked = 1;
            // 弱引用对象的成员不需要标记, 同时将扫描到的弱引用加入弱引用链表
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

            // 跳过非对象和标记过的对象
            while (field_start_ptr < field_end_ptr && (is_object(field_end_ptr[0]) ? is_marked(field_end_ptr[0]) : 1))
                field_end_ptr--;
            while (field_start_ptr < field_end_ptr &&
                   (is_object(field_start_ptr[0]) ? is_marked(field_start_ptr[0]) : 1))
                field_start_ptr++;

            // 跳过重复对象
            while (field_start_ptr < field_end_ptr && field_end_ptr[0] == field_end_ptr[-1])
                field_end_ptr--;
            while (field_start_ptr < field_end_ptr && field_start_ptr[0] == field_start_ptr[1])
                field_start_ptr++;

            if (field_start_ptr < field_end_ptr) {
                gc_mark_stack_push(context, field_start_ptr, field_end_ptr);
            }
            // 考虑到此时可能为 pair 结构 (对于 scheme 来说并不少见), 继续循环, 减少 mark stack 的占用
            // 深度优先搜索
            obj = *field_end_ptr;
        }
    } while (1);
}

/**
 * 从一个根开始遍历
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
 * 内部方法 1. 标记存活对象
 * <p>第一次图遍历</p>
 * @param context
 * @return <li>IMM_TRUE: 运行成功</li>
 * <li>IMM_FALSE: 运行失败</li>
 */
static object gc_mark(context_t context) {
    assert(context != NULL);

    // todo context 修改后, 修改gc_mark

    // 先重置弱引用链表
    context->weak_ref_chain = NULL;

    // 标记寄存器
    gc_mark_one_start(context, context->args);
    gc_mark_one_start(context, context->code);
    gc_mark_one_start(context, context->current_env);
    gc_mark_one_start(context, context->scheme_stack);

    // 标记返回值
    gc_mark_one_start(context, context->value);
    // 标记 load stack
    gc_mark_one_start(context, context->load_stack);

    // 标记全局符号表
    gc_mark_one_start(context, context->global_symbol_table);
    // 标记全局 environment
    gc_mark_one_start(context, context->global_environment);


    // 全局类型
    for (size_t i = 0; i < context->global_type_table_len; i++) {
        gc_mark_one_start(context, context->global_type_table[i].name);
        gc_mark_one_start(context, context->global_type_table[i].getter);
        gc_mark_one_start(context, context->global_type_table[i].setter);
        gc_mark_one_start(context, context->global_type_table[i].to_string);
    }

    // C 调用栈上变量保护表
    for (gc_saves_list_t save = context->saves; save != NULL; save = save->next) {
        // 使用 macro gc_var_n() 时, 必须保证第一次调用会触发 GC 的函数前必须初始化变量值
        if (save->illusory_object != NULL && is_object(*save->illusory_object)) {
            gc_mark_one_start(context, *save->illusory_object);
        }
    }
    return IMM_TRUE;
}

/**
 * 内部方法 2. 重置弱引用
 * <p>注意, 立即数无法被检查是否引用, 因此引用立即数的弱引用是不可靠的</p>
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
            // weak ref 引用的对象未标记时设置为空
            weak_ref_get(weak_ref) = NULL;
        }
        tmp = weak_ref;
        weak_ref = weak_ref->value.weak_ref._internal_next_ref;
        tmp->value.weak_ref._internal_next_ref = NULL;
    }

    context->weak_ref_chain = NULL;
}

/**
 * 内部方法 3. 计算紧凑对象后地址并设置 object->forwarding 字段,
 * 同时运行对象的 finalize
 * <p>第一次堆遍历</p>
 * @param context
 */
static void gc_set_forwarding(context_t context) {
    /*
     * 图例:
     *      =====: 已分配内存区域
     *           : 空闲内存区域
     *      *****: 压缩后存活的对象
     * 初始:
     *            _____________________________________
     * node->data |======================|               | node->free_ptr
     *            -------------------------------------
     *            |
     *         to from
     * 遇到存活对象:
     *            _____________________________________
     * node->data |*****|================|               | node->free_ptr
     *            -------------------------------------
     *                  |
     *               to & from
     * 遇到死亡对象:
     *            _____________________________________
     * node->data |*****|------|========|               | node->free_ptr
     *            -------------------------------------
     *                  |     |
     *                 to    from
     * 扫描结束:
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
                // 执行对象析构方法
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
 * 内部方法 4. 根据 object->forwarding 字段更新对象引用
 * <p>注意! 这会破坏旧的引用关系</p>
 * <p>第二次堆遍历</p>
 * @param context
 */
static void gc_adjust_ref(context_t context) {
    assert(context != NULL);

    // todo context 修改后, 修改 gc_adjust_ref, 更新根引用

    // 更新根引用

    // 寄存器
    gc_adjust_ref_one(&(context->args));
    gc_adjust_ref_one(&(context->code));
    gc_adjust_ref_one(&(context->current_env));
    gc_adjust_ref_one(&(context->scheme_stack));

    // 返回值
    gc_adjust_ref_one(&(context->value));
    // load stack
    gc_adjust_ref_one(&(context->load_stack));

    // 调整 global_symbol_table
    gc_adjust_ref_one(&(context->global_symbol_table));
    // 调整 global_environment
    gc_adjust_ref_one(&(context->global_environment));

    // 全局类型信息
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

    // C 调用栈上变量保护表
    for (gc_saves_list_t save = context->saves; save != NULL; save = save->next) {
        if (save->illusory_object != NULL && is_object(*save->illusory_object)) {
            *save->illusory_object = (*save->illusory_object)->forwarding;
        }
    }

    // 最后遍历堆更新所有引用
    heap_t heap = context->heap;
    for (heap_node_t node = heap->first_node; node != NULL; node = node->next) {
        char *ptr = node->data;

        while (ptr < node->free_ptr) {
            object obj = (object) ptr;
            assert(is_object(obj));
            size_t size = context_object_sizeof(context, obj);

            if (is_marked(obj)) {
                // 对于存活的对象, 检查所有成员引用
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
 * 内部方法 5. 移动对象
 * <p>第三次堆遍历</p>
 * @param context
 */
static void move_objects(context_t context) {
    heap_t heap = context->heap;
    for (heap_node_t node = heap->first_node; node != NULL; node = node->next) {
        char *new_free_ptr = node->data;
        char *ptr = node->data;

        // 移动对象
        while (ptr < node->free_ptr) {
            object obj = (object) ptr;
            assert(is_object(obj));

            size_t size = context_object_sizeof(context, obj);
            if (is_marked(obj)) {
                assert(obj->forwarding != NULL);
//                printf("alive: type=%d\n", obj->type);

                // 只有当对象被标记, 且转发地址不等于当前地址时才进行移动
                obj->marked = 0;    // 清除标记
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
        // 清理空间
        memset(new_free_ptr, 0, node->free_ptr - new_free_ptr);
        // 更新 free_ptr 指针
        node->free_ptr = new_free_ptr;
    }
}

/**
 * 启动回收垃圾回收
 * @param context
 * @return <li>IMM_TRUE: 运行成功</li><li>IMM_FALSE: 运行失败</li>
 */
EXPORT_API CHECKED GC object gc_collect(REF NOTNULL context_t context) {
    assert(context != NULL);
    if (!context->gc_collect_on) return IMM_TRUE;

#if USE_DEBUG_GC
#endif

    context->gc_happened = 1;

    gc_mark(context);
    // 重设弱引用
    gc_reset_weak_references(context);
    gc_set_forwarding(context);
    gc_adjust_ref(context);
    move_objects(context);

#if USE_DEBUG_GC
#endif

    return IMM_TRUE;
}

/**
 * 尝试从堆中分配内存, 不会触发 gc, 失败返回 NULL
 * @param heap 堆结构
 * @param size 要分配的对象大小
 * @return <li>NULL: 找不到足够大的空闲控件</li>
 */
EXPORT_API OUT CHECKED GC object gc_try_alloc(REF NOTNULL context_t context, IN size_t size) {
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
EXPORT_API OUT GC object gc_alloc(REF NOTNULL context_t context, IN size_t size) {
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