#ifndef BASE_SCHEME_GC_TEST_H
#define BASE_SCHEME_GC_TEST_H
#pragma once

#include "base-scheme/object.h"
#include "base-scheme/heap.h"
#include "base-scheme/context.h"
#include "base-scheme/gc.h"
#include <base-scheme/vm.h>


/**
 * 测试 context, heap, gc
 * 对托管堆结构进行测试
 */

// test case here
UTEST(gc_test, hello_utest) {
    ASSERT_TRUE(1);
}


UTEST(gc_test, make_context_test) {
    context_t context = context_make(0x100, 2, 0x10000);
    heap_grow(context->heap);
    ASSERT_EQ(context->heap->total_size, context->heap->init_size + context->heap->init_size * 2);
    printf("heap total size: %zd\n", context->heap->total_size);
    context_destroy(context);
}

UTEST(gc_test, alloc_test) {
    size_t init_size = 16u;
    size_t scale = 2u;
    size_t max_size = 500u;
    size_t n = 8;
    context_t context = context_make(init_size, scale, max_size);
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
        objs[i] = i64_make_real_object(context, 200);
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

UTEST(vm_test, heap_make_free_test) {
    context_t context = context_make(16, 2, 0x10000);
    object i64 = i64_make_real_object(context, 20);
    object i64max = i64_make(context, INT64_MAX);
    object i64min = i64_make_real_object(context, INT64_MIN);
    object doublenum = doublenum_make(context, 200.0);
    object pair_carImmI64_cdrDouble = pair_make(context, i64, doublenum);
    object string_obj = string_make_from_cstr(context, "this is a string object");
    object string_null_object = string_make_from_cstr(context, NULL);
    object string_empty = string_make_from_cstr(context, "");
    object symbol_obj = symbol_make_from_cstr(context, "this is a symbol object");
    object vector10 = vector_make(context, 10);
    vector_ref(vector10, 1) = doublenum;
    vector_ref(vector10, 9) = IMM_TRUE;

    object objs[] = {
            i64, i64max, i64min, doublenum, pair_carImmI64_cdrDouble,
            string_obj, string_null_object, string_empty, symbol_obj, vector10,
    };

    // 开始拆解 heap_t 结构
    heap_t heap = context->heap;

    // 遍历所有块
    UTEST_PRINTF("heap dump:");
    size_t count = 1;
    size_t objc = 1;
    for (heap_node_t node = heap->first_node; node != NULL; node = node->next, count++) {
        // 第一个块
        UTEST_PRINTF(" [\n    heap node %zd: [\n", count);
        for (char *heap_ptr = node->data;
             heap_ptr < node->free_ptr; heap_ptr += object_size_runtime((object) heap_ptr)) {
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
            UTEST_PRINTF("        object %zd: {\n", objc);
            objc++;

#define P(...) UTEST_PRINTF("            "__VA_ARGS__)
#define MORE_SPACE(...) UTEST_PRINTF("                "__VA_ARGS__)
#define address() MORE_SPACE("address:    0x%p\n", obj);
#define obj_size() MORE_SPACE("size:       %zd\n", object_size_runtime(obj))
#define magic() MORE_SPACE("magic:      0x%"PRIX8"\n", obj->magic)
#define type() MORE_SPACE("type:       0x%"PRIX8"\n", obj->type)
#define marked() MORE_SPACE("marked:     0x%"PRIX8"\n", obj->marked)
#define padding_size() MORE_SPACE("pad:        0x%"PRIX8"\n", obj->padding_size)
#define forwarding() MORE_SPACE("forward:    0x%p\n", obj->forwarding)
#define header() address(); obj_size(); magic(); type(); marked(); padding_size(); forwarding()
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

            UTEST_PRINTF("        }\n");

        }
        if (node->data == node->free_ptr) {
            UTEST_PRINTF("        empty_heap_node;\n");
        }
        UTEST_PRINTF("    ]\n]");
    }
    UTEST_PRINTF("\n");

    context_destroy(context);
}

#endif // BASE_SCHEME_GC_TEST_H