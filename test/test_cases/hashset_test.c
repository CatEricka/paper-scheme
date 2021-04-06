#include "lib/utest.h"
#include <paper-scheme/runtime.h>

UTEST(hashset_test, hashset_test) {
    context_t context = context_make(0x10000, 2, 0x100000);
    gc_var4(context, obj, set, k1, k2);
    uint64_t start, time;

    set = hashset_make_op(context, DEFAULT_HASH_SET_MAP_INIT_CAPACITY, DEFAULT_HASH_SET_MAP_LOAD_FACTOR);
    ASSERT_TRUE(is_object(set));
    ASSERT_TRUE(is_hashset(set));
    ASSERT_FALSE(is_hashmap(set));

    ASSERT_EQ(hashset_contains_op(context, set, IMM_UNIT), IMM_FALSE);

    obj = symbol_make_from_cstr_op(context, "symbol");
    hashset_put_op(context, set, obj);
    ASSERT_EQ(hashset_contains_op(context, set, obj), IMM_TRUE);

    obj = IMM_UNIT;
    set = IMM_UNIT;
    k1 = IMM_UNIT;
    k2 = IMM_UNIT;
    gc_release_var(context);

    start = utest_ns();
    gc_collect(context);
    time = utest_ns() - start;
    UTEST_PRINTF("gc time: %"
                         PRId64
                         " ns\n", time);
    ASSERT_TRUE(context->heap->first_node->data == context->heap->first_node->free_ptr);
    ASSERT_EQ(context->saves, NULL);
    context_destroy(context);
}