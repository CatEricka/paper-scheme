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
    context_t context = context_make();
    heap_grow(context->heap);
    printf("heap total size: %zd\n", context->heap->total_size);
    object obj1 = i64_make(context, 200);
    object obj2 = i64_make(context, 200);
    object obj3 = i64_make(context, 200);
    object obj4 = i64_make(context, 200);
    ASSERT_TRUE(obj1);
    ASSERT_TRUE(obj2);
    ASSERT_TRUE(obj3);
    ASSERT_TRUE(obj4);
    printf("heap data: %p, free_ptr: %p, obj1: %p\n", context->heap->first_node->data,
           context->heap->first_node->free_ptr, obj1);
    printf("allocated: %d\n", (context->heap->first_node->free_ptr - context->heap->first_node->data));
    ASSERT_EQ(obj1->value.i64, 200);
    context_destroy(context);
}

UTEST(vm_test, make_context_test) {
    context_t context = context_make();
    heap_grow(context->heap);
    ASSERT_EQ(context->heap->total_size, context->heap->init_size + context->heap->init_size * 2);
    printf("heap total size: %zd\n", context->heap->total_size);
    context_destroy(context);
}


#endif //BASE_SCHEME_VM_TEST_H
