#include "lib/utest.h"
#include <paper-scheme/runtime.h>

UTEST(hashset_test, hashset_test) {
    context_t context = context_make(0x10000, 2, 0x100000);
    gc_var4(context, obj, set, set2, vector);
    uint64_t start, time;

    set = hashset_make_op(context, 2, DEFAULT_HASH_SET_MAP_LOAD_FACTOR);
    ASSERT_TRUE(is_object(set));
    ASSERT_TRUE(is_hashset(set));
    ASSERT_FALSE(is_hashmap(set));

    ASSERT_EQ(hashset_contains_op(context, set, IMM_UNIT), IMM_FALSE);

    // vector = { "symbol1", "symbol2", ...}
    char *symbols[] = {
            "symbol0",
            "symbol1",
            "symbol2",
            "symbol3",
            "symbol4",
            "symbol5",
            "symbol6",
            "symbol7",
            "symbol8",
            "symbol9",
    };
    size_t symbols_len = sizeof(symbols) / sizeof(char *);

    vector = vector_make_op(context, 10);
    for (size_t i = 0; i < symbols_len; i++) {
        vector_ref(vector, i) = symbol_make_from_cstr_op(context, symbols[i]);
    }

    // put all symbol
    for (size_t i = 0; i < vector_len(vector); i++) {
        hashset_put_op(context, set, vector_ref(vector, i));
    }

    // test contains
    for (size_t i = 0; i < vector_len(vector); i++) {
        ASSERT_EQ(hashset_contains_op(context, set, vector_ref(vector, i)), IMM_TRUE);
    }
    size_t size = hashset_size(set);
    ASSERT_EQ(size, symbols_len);

    // test remove
    for (size_t i = 0; i < vector_len(vector); i++) {
        hashset_remove_op(context, set, vector_ref(vector, i));
    }
    ASSERT_EQ(hashset_size(set), 0);

    obj = IMM_UNIT;
    set = IMM_UNIT;
    set2 = IMM_UNIT;
    vector = IMM_UNIT;
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