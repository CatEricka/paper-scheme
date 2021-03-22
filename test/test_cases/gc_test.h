#ifndef BASE_SCHEME_GC_TEST_H
#define BASE_SCHEME_GC_TEST_H
#pragma once

#include <paper-scheme/object.h>
#include <paper-scheme/heap.h>
#include <paper-scheme/context.h>
#include <paper-scheme/gc.h>
#include <paper-scheme/vm.h>


/**
 * 测试内存分配与垃圾回收
 */

#define heap_make_free_test_print_on 0

// test case here
UTEST(gc_test, hello_utest) {
    ASSERT_TRUE(1);
}


UTEST(gc_test, make_context_test) {
    context_t context = context_make(0x100, 2, 0x10000);
    heap_grow(context->heap);
    ASSERT_EQ(context->heap->total_size, context->heap->init_size + context->heap->init_size * 2);
    printf("heap total size: %zd\n", context->heap->total_size);
    ASSERT_EQ(context->global_type_table_len, (size_t) OBJECT_TYPE_ENUM_MAX);
    context_destroy(context);
}


UTEST(gc_test, alloc_test) {
    size_t init_size = 16u;
    size_t scale = 2u;
    size_t max_size = 500u;
    size_t n = 8;
    context_t context = context_make(init_size, scale, max_size);
    gc_collect_disable(context);
    heap_t heap = context->heap;
    printf("heap init total size: %zd\n", context->heap->total_size);

    int count = 0;

    // 增长测试
    heap_grow(context->heap);
    ASSERT_TRUE(heap->first_node->next != NULL);
    ASSERT_TRUE(heap->first_node->next->chunk_size == heap->init_size * heap->growth_scale);
    ASSERT_TRUE(heap->first_node->next->chunk_size == init_size * scale);
    ASSERT_TRUE(heap->first_node->data == heap->first_node->free_ptr);
    ASSERT_TRUE(heap->first_node->next->data == heap->first_node->next->free_ptr);
    count = 0;
    for (heap_node_t node = heap->first_node; node != NULL; node = node->next, count++);
    UTEST_PRINTF("after first heap_grow(), heap node count: %d\n", count);
    ASSERT_EQ(count, 2);
    object *objs = malloc(n * (sizeof(object)));

    // 分配测试
    for (size_t i = 0; i < n; i++) {
        objs[i] = i64_make_real_object_op(context, 200);
        ASSERT_TRUE(objs[i]);
    }

    // 容量限制
    ASSERT_LE(context->heap->total_size, max_size);

    // 堆信息
    UTEST_PRINTF("heap data: 0x%p, free_ptr: 0x%p\n", context->heap->first_node->data,
                 context->heap->first_node->free_ptr);
    count = 0;
    for (heap_node_t node = heap->first_node; node != NULL; node = node->next, count++);

    // 对象信息
    for (size_t i = 0; i < n; i++) {
        ASSERT_EQ(i64_getvalue(objs[i]), 200);
        //printf("object %zd: address=%p, value=%"PRId64"\n", i, objs[i], i64_getvalue(objs[i]));
    }

    // 分配容量计算
    size_t allocated = 0u;
    for (heap_node_t node = context->heap->first_node; node != NULL; node = node->next) {
        allocated += node->free_ptr - node->data;
    }
    printf("allocated: %td\n", allocated);
    printf("heap total size: %zd\n", context->heap->total_size);
    UTEST_PRINTF("heap node count: %d\n", count);

    // 对象值变更测试
    for (size_t i = 0; i < n; i++) {
        objs[i]->value.i64 = 3000;
    }
    for (size_t i = 0; i < n; i++) {
        //printf("object %zd: address=%p, value=%"PRId64"\n", i, objs[i], objs[i]->value.i64);
        ASSERT_EQ(objs[i]->value.i64, 3000);
    }

    // 释放堆结构
    context_destroy(context);
}


#if heap_make_free_test_print_on
#define HEAP_DUMP_PRINTF(...) UTEST_PRINTF(__VA_ARGS__)
#else
#define HEAP_DUMP_PRINTF(...) ((void)0)
#endif

UTEST(gc_test, heap_make_free_test) {
    context_t context = context_make(16, 2, 0x10000);
    gc_collect_disable(context);
    object i64 = i64_make_real_object_op(context, 20);
    object i64max = i64_make_op(context, INT64_MAX);
    object i64min = i64_make_real_object_op(context, INT64_MIN);
    object doublenum = doublenum_make_op(context, 200.0);
    object pair_carImmI64_cdrDouble = pair_make_op(context, i64, doublenum);
    object string_obj = string_make_from_cstr_op(context, "this is a string object");
    object string_null_object = string_make_from_cstr_op(context, NULL);
    object string_empty = string_make_from_cstr_op(context, "");
    object symbol_obj = symbol_make_from_cstr_op(context, "this is a symbol object");
    object vector10 = vector_make_op(context, 10);
    vector_ref(vector10, 1) = doublenum;
    vector_ref(vector10, 9) = IMM_TRUE;

    object objs[] = {
            i64, i64max, i64min, doublenum, pair_carImmI64_cdrDouble,
            string_obj, string_null_object, string_empty, symbol_obj, vector10,
    };

    // 开始拆解 heap_t 结构
    heap_t heap = context->heap;

    // 遍历所有块
    HEAP_DUMP_PRINTF("heap dump:");
    size_t count = 1;
    size_t objc = 1;
    for (heap_node_t node = heap->first_node; node != NULL; node = node->next, count++) {
        // 第一个块
        HEAP_DUMP_PRINTF(" [\n    heap node %zd: [\n", count);
        for (char *heap_ptr = node->data;
             heap_ptr < node->free_ptr; heap_ptr += object_bootstrap_sizeof((object) heap_ptr)) {
            object obj = (object) heap_ptr;
            ASSERT_TRUE(is_object(obj));
            int flag = 0;
            for (size_t c = 0; c < (sizeof(objs) / sizeof(object)); c++) {
                if (objs[c] == obj) {
                    flag = 1;
                    break;
                }
            }
            //所有分配过的对象应当都能在堆里找到
            ASSERT_TRUE(("objects address check" && flag));
            // 输出对象信息
            HEAP_DUMP_PRINTF("        object %zd: {\n", objc);
            objc++;

#define P(...) HEAP_DUMP_PRINTF("            "__VA_ARGS__)
#define MORE_SPACE(...) HEAP_DUMP_PRINTF("                "__VA_ARGS__)
#define address() MORE_SPACE("address:    0x%p\n", obj);
#define obj_size() MORE_SPACE("size:       %zd\n", object_bootstrap_sizeof(obj))
#define header_size() MORE_SPACE("header:     %zd\n", object_sizeof_header())
#define magic() MORE_SPACE("magic:      0x%"PRIX8"\n", obj->magic)
#define type() MORE_SPACE("type:       %d\n", obj->type)
#define marked() MORE_SPACE("marked:     0x%"PRIX8"\n", obj->marked)
#define padding_size() MORE_SPACE("padding:    %"PRId8"\n", obj->padding_size)
#define forwarding() MORE_SPACE("forward:    0x%p\n", obj->forwarding)

#define header() address(); obj_size(); header_size(); padding_size(); magic(); type(); marked(); forwarding()
            if (is_i64(obj)) {
                P("i64: {\n");
                header();
                MORE_SPACE("value: %"
                                   PRId64
                                   "\n", obj->value.i64);
                P("}\n");
            } else if (is_doublenum(obj)) {
                P("double: {\n");
                header();
                MORE_SPACE("value: %f\n", obj->value.doublenum);
                P("}\n");
            } else if (is_pair(obj)) {
                P("pair: {\n");
                header();
                MORE_SPACE("value: {\n");
                MORE_SPACE("    car addr: %p\n", obj->value.pair.car);
                MORE_SPACE("    cdr addr: %p\n", obj->value.pair.cdr);
                MORE_SPACE("}\n");
                P("}\n");
            } else if (is_string(obj)) {
                P("string: {\n");
                header();
                MORE_SPACE("value: {\n");
                MORE_SPACE("    len:      %zd\n", obj->value.string.len);
                MORE_SPACE("    cstr:     %s\n", obj->value.string.data);
                MORE_SPACE("}\n");
                P("}\n");
            } else if (is_symbol(obj)) {
                P("symbol: {\n");
                header();
                MORE_SPACE("value: {\n");
                MORE_SPACE("    len:      %zd\n", obj->value.symbol.len);
                MORE_SPACE("    cstr:     %s\n", obj->value.symbol.data);
                MORE_SPACE("}\n");
                P("}\n");
            } else if (is_vector(obj)) {
                P("vector: [\n");
                header();
                MORE_SPACE("value: {\n");
                MORE_SPACE("    len:     %zd\n", obj->value.vector.len);
                MORE_SPACE("    data: {\n");
                for (size_t index = 0; index < obj->value.vector.len; index++) {
                    object v = obj->value.vector.data[index];
                    if (is_i64(v)) {
                        MORE_SPACE("        i64:        value=%"
                                           PRId64
                                           "\n", i64_getvalue(v));
                    } else if (is_imm_char(v)) {
                        MORE_SPACE("        char:       value=%c\n", char_imm_getvalue(v));
                    } else if (is_imm_unit(v)) {
                        MORE_SPACE("        '()\n");
                    } else if (is_imm_true(v)) {
                        MORE_SPACE("        true\n");
                    } else if (is_imm_false(v)) {
                        MORE_SPACE("        false\n");
                    } else if (is_string(v)) {
                        MORE_SPACE("        string:     value=%s\n", string_get_cstr(v));
                    } else if (is_symbol(v)) {
                        MORE_SPACE("        symbol:     value=%s\n", symbol_get_cstr(v));
                    } else if (is_vector(v)) {
                        MORE_SPACE("        vector:     addr=0x%p\n", v);
                    } else if (is_doublenum(v)) {
                        MORE_SPACE("        doublenum:  value=%f\n", doublenum_getvalue(v));
                    } else if (is_pair(v)) {
                        MORE_SPACE("        pair:       addr=0x%p\n", v);
                    } else {
                        MORE_SPACE("        unknown:    addr=0x%p\n", v);
                    }
                }
                MORE_SPACE("    }\n");
                MORE_SPACE("}\n");
                P("]\n");
            } else {
                P("unknown object: {}");
            }

            HEAP_DUMP_PRINTF("        }\n");

        }
        if (node->data == node->free_ptr) {
            HEAP_DUMP_PRINTF("        empty_heap_node;\n");
        }
        HEAP_DUMP_PRINTF("    ]\n]");
    }
    HEAP_DUMP_PRINTF("\n");

    size_t allocated = 0u;
    for (heap_node_t node = context->heap->first_node; node != NULL; node = node->next) {
        allocated += node->free_ptr - node->data;
    }
    printf("allocated: %td\n", allocated);
    printf("heap total size: %zd\n", context->heap->total_size);

    context_destroy(context);
}

static void heap_dump(context_t context) {
    // 开始拆解 heap_t 结构
    heap_t heap = context->heap;

    // 遍历所有块
    HEAP_DUMP_PRINTF("heap dump:");
    size_t count = 1;
    size_t objc = 1;
    for (heap_node_t node = heap->first_node; node != NULL; node = node->next, count++) {
        // 第一个块
        HEAP_DUMP_PRINTF(" [\n    heap node %zd: [\n", count);
        for (char *heap_ptr = node->data;
             heap_ptr < node->free_ptr; heap_ptr += object_bootstrap_sizeof((object) heap_ptr)) {
            object obj = (object) heap_ptr;
            assert(is_object(obj));
            // 输出对象信息
            HEAP_DUMP_PRINTF("        object %zd: {\n", objc);
            objc++;

#define P(...) HEAP_DUMP_PRINTF("            "__VA_ARGS__)
#define MORE_SPACE(...) HEAP_DUMP_PRINTF("                "__VA_ARGS__)
#define address() MORE_SPACE("address:    0x%p\n", obj);
#define obj_size() MORE_SPACE("size:       %zd\n", object_bootstrap_sizeof(obj))
#define header_size() MORE_SPACE("header:     %zd\n", object_sizeof_header())
#define magic() MORE_SPACE("magic:      0x%"PRIX8"\n", obj->magic)
#define type() MORE_SPACE("type:       %d\n", obj->type)
#define marked() MORE_SPACE("marked:     0x%"PRIX8"\n", obj->marked)
#define padding_size() MORE_SPACE("padding:    %"PRId8"\n", obj->padding_size)
#define forwarding() MORE_SPACE("forward:    0x%p\n", obj->forwarding)
#define header() address(); obj_size(); header_size(); padding_size(); magic(); type(); marked(); forwarding()
            if (is_i64(obj)) {
                P("i64: {\n");
                header();
                MORE_SPACE("value: %"
                                   PRId64
                                   "\n", obj->value.i64);
                P("}\n");
            } else if (is_doublenum(obj)) {
                P("double: {\n");
                header();
                MORE_SPACE("value: %f\n", obj->value.doublenum);
                P("}\n");
            } else if (is_pair(obj)) {
                P("pair: {\n");
                header();
                MORE_SPACE("value: {\n");
                MORE_SPACE("    car addr: %p\n", obj->value.pair.car);
                MORE_SPACE("    cdr addr: %p\n", obj->value.pair.cdr);
                MORE_SPACE("}\n");
                P("}\n");
            } else if (is_string(obj)) {
                P("string: {\n");
                header();
                MORE_SPACE("value: {\n");
                MORE_SPACE("    len:      %zd\n", obj->value.string.len);
                MORE_SPACE("    cstr:     %s\n", obj->value.string.data);
                MORE_SPACE("}\n");
                P("}\n");
            } else if (is_symbol(obj)) {
                P("symbol: {\n");
                header();
                MORE_SPACE("value: {\n");
                MORE_SPACE("    len:      %zd\n", obj->value.symbol.len);
                MORE_SPACE("    cstr:     %s\n", obj->value.symbol.data);
                MORE_SPACE("}\n");
                P("}\n");
            } else if (is_vector(obj)) {
                P("vector: [\n");
                header();
                MORE_SPACE("value: {\n");
                MORE_SPACE("    len:     %zd\n", obj->value.vector.len);
                MORE_SPACE("    data: {\n");
                for (size_t index = 0; index < obj->value.vector.len; index++) {
                    object v = obj->value.vector.data[index];
                    if (is_i64(v)) {
                        MORE_SPACE("        i64:        value=%"
                                           PRId64
                                           "\n", i64_getvalue(v));
                    } else if (is_imm_char(v)) {
                        MORE_SPACE("        char:       value=%c\n", char_imm_getvalue(v));
                    } else if (is_imm_unit(v)) {
                        MORE_SPACE("        '()\n");
                    } else if (is_imm_true(v)) {
                        MORE_SPACE("        true\n");
                    } else if (is_imm_false(v)) {
                        MORE_SPACE("        false\n");
                    } else if (is_string(v)) {
                        MORE_SPACE("        string:     value=%s\n", string_get_cstr(v));
                    } else if (is_symbol(v)) {
                        MORE_SPACE("        symbol:     value=%s\n", symbol_get_cstr(v));
                    } else if (is_vector(v)) {
                        MORE_SPACE("        vector:     addr=0x%p\n", v);
                    } else if (is_doublenum(v)) {
                        MORE_SPACE("        doublenum:  value=%f\n", doublenum_getvalue(v));
                    } else if (is_pair(v)) {
                        MORE_SPACE("        pair:       addr=0x%p\n", v);
                    } else {
                        MORE_SPACE("        unknown:    addr=0x%p\n", v);
                    }
                }
                MORE_SPACE("    }\n");
                MORE_SPACE("}\n");
                P("]\n");
            } else {
                P("unknown object: {}");
            }

            HEAP_DUMP_PRINTF("        }\n");

        }
        if (node->data == node->free_ptr) {
            HEAP_DUMP_PRINTF("        empty_heap_node;\n");
        }
        HEAP_DUMP_PRINTF("    ]\n]");
    }
    HEAP_DUMP_PRINTF("\n");
}


UTEST(gc_test, gc_saves_list_test) {
    context_t context = context_make(0x100, 2, 0x10000);
    gc_var7(context, a, b, c, d, e, f, g);

    gc_saves_list_t p = context->saves;
    ASSERT_EQ(p->illusory_object, &g);
    p = p->next;
    ASSERT_EQ(p->illusory_object, &f);
    p = p->next;
    ASSERT_EQ(p->illusory_object, &e);
    p = p->next;
    ASSERT_EQ(p->illusory_object, &d);
    p = p->next;
    ASSERT_EQ(p->illusory_object, &c);
    p = p->next;
    ASSERT_EQ(p->illusory_object, &b);
    p = p->next;
    ASSERT_EQ(p->illusory_object, &a);
    p = p->next;
    ASSERT_EQ(p, NULL);

    a = i64_make_real_object_op(context, 1);
    b = i64_make_real_object_op(context, 2);
    c = i64_make_real_object_op(context, 3);
    d = i64_make_real_object_op(context, 4);
    e = pair_make_op(context, a, b);
    f = pair_make_op(context, c, d);
    g = pair_make_op(context, e, f);
    object h = pair_make_op(context, f, g);

    gc_collect(context);

    for (struct gc_illusory_dream *var = context->saves; var != NULL; var = var->next) {
        UTEST_PRINTF("saves: object? = %d, type = %d, marked = %d\n", is_object(*(var->illusory_object)),
                     (*(var->illusory_object))->type,
                     (*(var->illusory_object))->marked);
    }
    UTEST_PRINTF("died: type = %d, marked = %d\n", h->type, h->marked);

    heap_dump(context);

    ASSERT_FALSE(a->marked);
    ASSERT_FALSE(b->marked);
    ASSERT_FALSE(c->marked);
    ASSERT_FALSE(d->marked);
    ASSERT_FALSE(e->marked);
    ASSERT_FALSE(f->marked);
    ASSERT_FALSE(g->marked);
    ASSERT_FALSE(h->marked);

    gc_release_all(context);

    int64_t start = utest_ns();
    gc_collect(context);
    int64_t time = utest_ns() - start;
    UTEST_PRINTF("gc time: %"
                         PRId64
                         " ns\n", time);

    // 释放后应当回收暂存对象
    ASSERT_EQ(context->heap->first_node->data, context->heap->first_node->free_ptr);
    heap_dump(context);
    context_destroy(context);
}

UTEST(gc_test, mark_test1) {
    context_t context = context_make(0x100, 2, 0x10000);

    gc_var3(context, root1, root2, root3);

    // root1: ((1 . 2) 3 . 4)
    root1 = pair_make_op(context, IMM_UNIT, IMM_UNIT);
    root2 = pair_make_op(context, IMM_UNIT, IMM_UNIT);
    root1 = pair_make_op(context, root1, root2);

    root3 = vector_make_op(context, 4);

    pair_caar(root1) = i64_make_real_object_op(context, 1);
    pair_cdar(root1) = i64_make_real_object_op(context, 2);
    pair_cadr(root1) = i64_make_real_object_op(context, 3);
    pair_cddr(root1) = i64_make_real_object_op(context, 4);

    vector_ref(root3, 0) = pair_caar(root1);
    vector_ref(root3, 1) = pair_cdar(root1);
    vector_ref(root3, 2) = pair_cadr(root1);
    vector_ref(root3, 3) = pair_cddr(root1);

    int64_t start = utest_ns();
    gc_collect(context);
    int64_t time = utest_ns() - start;
    UTEST_PRINTF("gc time: %"
                         PRId64
                         " ns\n", time);

    ASSERT_EQ(1, i64_getvalue(pair_caar(root1)));
    ASSERT_EQ(2, i64_getvalue(pair_cdar(root1)));
    ASSERT_EQ(3, i64_getvalue(pair_cadr(root1)));
    ASSERT_EQ(4, i64_getvalue(pair_cddr(root1)));

    ASSERT_FALSE(is_marked(pair_caar(root1)));
    ASSERT_FALSE(is_marked(pair_cdar(root1)));
    ASSERT_FALSE(is_marked(pair_cadr(root1)));
    ASSERT_FALSE(is_marked(pair_cddr(root1)));

    for (int i = 0; i < 4; i++) {
        ASSERT_TRUE(is_i64(vector_ref(root3, i)));
        UTEST_PRINTF("%"
                             PRId64
                             ": marked = %"
                             PRId8
                             "\n", i64_getvalue(vector_ref(root3, i)), is_marked(vector_ref(root3, i)));
    }

    gc_release_all(context);
    ASSERT_TRUE(context->saves == NULL);
    context_destroy(context);
}

UTEST(gc_test, gc_collect_move_test) {
    context_t context = context_make(0x100, 2, 0x10000);
    gc_var7(context, a, b, c, d, e, f, g);

    a = i64_make_real_object_op(context, 1);

    // died
    object h = pair_make_op(context, f, g);

    b = i64_make_real_object_op(context, 2);
    c = i64_make_real_object_op(context, 3);
    d = i64_make_real_object_op(context, 4);
    e = pair_make_op(context, a, b);
    f = pair_make_op(context, c, d);
    g = pair_make_op(context, e, f);

    int64_t start = utest_ns();
    gc_collect(context);
    int64_t time = utest_ns() - start;
    UTEST_PRINTF("gc time: %"
                         PRId64
                         " ns\n", time);

    ASSERT_FALSE(is_pair(h));
    ASSERT_TRUE(is_i64(h));
    ASSERT_EQ(2, i64_getvalue(h));
}

#endif // BASE_SCHEME_GC_TEST_H