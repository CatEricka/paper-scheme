#ifndef BASE_SCHEME_VM_TEST_H
#define BASE_SCHEME_VM_TEST_H
#pragma once

#include "base-scheme/vm.h"


/**
 * ²âÊÔ vm
 */

UTEST(vm_test, hello) {
    ASSERT_TRUE(1);
}


UTEST(gc_test, alloc_test) {
    context_t context = context_make(16u, 2u, 112u);
    printf("heap init total size: %zd\n", context->heap->total_size);
    heap_grow(context->heap);
    object obj1 = i64_make(context, 200);
    object obj2 = i64_make(context, 200);
    object obj3 = i64_make(context, 200);
    object obj4 = i64_make(context, 200);
    ASSERT_TRUE(obj1);
    ASSERT_TRUE(obj2);
    ASSERT_TRUE(obj3);
    ASSERT_TRUE(obj4);
    printf("heap data: 0x%p, free_ptr: 0x%p\n", context->heap->first_node->data,
           context->heap->first_node->free_ptr);
    printf("obj1: %p, obj2: %p, obj3: %p, obj4: %p\n", obj1, obj2, obj3, obj4);
    size_t allocated = 0u;
    for (heap_node_t node = context->heap->first_node; node != NULL; node = node->next) {
        allocated += node->free_ptr - node->data;
    }
    printf("allocated: %td\n", allocated);
    printf("heap total size: %zd\n", context->heap->total_size);
    ASSERT_EQ(obj1->value.i64, 200);
    context_destroy(context);
}

UTEST(vm_test, make_context_test) {
    context_t context = context_make(0x100, 2, 0x10000);
    heap_grow(context->heap);
    ASSERT_EQ(context->heap->total_size, context->heap->init_size + context->heap->init_size * 2);
    printf("heap total size: %zd\n", context->heap->total_size);
    context_destroy(context);
}


#endif //BASE_SCHEME_VM_TEST_H
