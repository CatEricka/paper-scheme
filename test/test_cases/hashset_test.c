#include "lib/utest.h"
#include <paper-scheme/runtime.h>

UTEST(hashset_test, test) {
    context_t context = context_make(0x10000, 2, 0x100000);
    gc_var5(context, obj, set, set2, vector, vector2);
    uint64_t start, time;

    set = hashset_make_op(context, 2, DEFAULT_HASH_SET_MAP_LOAD_FACTOR);
    set2 = hashset_make_op(context, 2, DEFAULT_HASH_SET_MAP_LOAD_FACTOR);
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
    char *other_symbol_table[] = {
            "values0",
            "values1",
            "values2",
            "values3",
            "values4",
            "values5",
            "values6",
            "values7",
            "values8",
            "values9",
    };
    size_t symbols_len = sizeof(symbols) / sizeof(char *);
    size_t other_symbol_len = sizeof(other_symbol_table) / sizeof(char *);

    vector = vector_make_op(context, symbols_len);
    for (size_t i = 0; i < symbols_len; i++) {
        vector_ref(vector, i) = symbol_make_from_cstr_op(context, symbols[i]);
    }
    vector2 = vector_make_op(context, other_symbol_len);
    for (size_t i = 0; i < other_symbol_len; i++) {
        vector_ref(vector2, i) = symbol_make_from_cstr_op(context, other_symbol_table[i]);
    }

    // put symbol
    for (size_t i = 0; i < vector_len(vector); i++) {
        hashset_put_op(context, set, vector_ref(vector, i));
    }
    ASSERT_EQ(hashset_size(set), symbols_len);

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
    for (size_t i = 0; i < vector_len(vector); i++) {
        ASSERT_EQ(hashset_contains_op(context, set, vector_ref(vector, i)), IMM_FALSE);
    }
    ASSERT_EQ(hashset_size(set), 0);

    // put symbol 2
    for (size_t i = 0; i < vector_len(vector); i++) {
        hashset_put_op(context, set, vector_ref(vector, i));
        ASSERT_EQ(hashset_contains_op(context, set, vector_ref(vector, i)), IMM_TRUE);
    }
    for (size_t i = 0; i < vector_len(vector2); i++) {
        hashset_put_op(context, set2, vector_ref(vector2, i));
        ASSERT_EQ(hashset_contains_op(context, set2, vector_ref(vector2, i)), IMM_TRUE);
    }

    // test contains 2
    for (size_t i = 0; i < vector_len(vector); i++) {
        ASSERT_EQ(hashset_contains_op(context, set, vector_ref(vector, i)), IMM_TRUE);
    }
    for (size_t i = 0; i < vector_len(vector2); i++) {
        ASSERT_EQ(hashset_contains_op(context, set2, vector_ref(vector2, i)), IMM_TRUE);
    }
    ASSERT_EQ(hashset_size(set), symbols_len);
    ASSERT_EQ(hashset_size(set2), other_symbol_len);

    // put all test 2
    hashset_put_all_op(context, set, set2);
    for (size_t i = 0; i < vector_len(vector); i++) {
        ASSERT_EQ(hashset_contains_op(context, set, vector_ref(vector, i)), IMM_TRUE);
    }
    for (size_t i = 0; i < vector_len(vector2); i++) {
        ASSERT_EQ(hashset_contains_op(context, set, vector_ref(vector2, i)), IMM_TRUE);
    }
    ASSERT_EQ(hashset_size(set), symbols_len + other_symbol_len);
    ASSERT_EQ(hashset_size(set2), other_symbol_len);

    // dup put test 2
    hashset_put_op(context, set, vector_ref(vector, 0));
    ASSERT_EQ(hashset_size(set), symbols_len + other_symbol_len);

    // clear test 2
    hashset_clear_op(context, set);
    for (size_t i = 0; i < vector_len(vector); i++) {
        ASSERT_EQ(hashset_contains_op(context, set, vector_ref(vector, i)), IMM_FALSE);
    }
    for (size_t i = 0; i < vector_len(vector2); i++) {
        ASSERT_EQ(hashset_contains_op(context, set, vector_ref(vector2, i)), IMM_FALSE);
    }
    ASSERT_EQ(hashset_size(set), 0);

    // re put test 2
    hashset_put_all_op(context, set, set2);
    for (size_t i = 0; i < vector_len(vector); i++) {
        ASSERT_EQ(hashset_contains_op(context, set, vector_ref(vector, i)), IMM_FALSE);
    }
    for (size_t i = 0; i < vector_len(vector2); i++) {
        ASSERT_EQ(hashset_contains_op(context, set, vector_ref(vector2, i)), IMM_TRUE);
    }
    ASSERT_EQ(hashset_size(set), hashset_size(set2));
    ASSERT_EQ(hashset_size(set), other_symbol_len);

    // hashset->vector test
    obj = hashset_to_vector_op(context, set2);
    ASSERT_EQ(vector_len(obj), hashset_size(set2));
    UTEST_PRINTF("hashset->vector:\n");
    for (size_t i = 0; i < vector_len(obj); i++) {
        ASSERT_TRUE(is_symbol(vector_ref(obj, i)));
        UTEST_PRINTF("key: %s\n", symbol_get_cstr(vector_ref(obj, i)));
        ASSERT_EQ(hashset_contains_op(context, set2, vector_ref(obj, i)), IMM_TRUE);
    }

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