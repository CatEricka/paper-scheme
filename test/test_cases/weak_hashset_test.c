#include "lib/utest.h"
#include <paper-scheme/runtime.h>

UTEST(weak_hashset_test, strong_ref_test) {
    context_t context = context_make(0x10000, 2, 0x100000);
    gc_var5(context, obj, set, set2, vector, vector2);
    uint64_t start, time;

    // 弱引用
    set = weak_hashset_make_op(context, 2, DEFAULT_HASH_SET_MAP_LOAD_FACTOR);
    set2 = weak_hashset_make_op(context, 2, DEFAULT_HASH_SET_MAP_LOAD_FACTOR);
    ASSERT_TRUE(is_object(set));
    ASSERT_TRUE(is_weak_hashset(set));
    ASSERT_TRUE(is_weak_hashset(set2));

    ASSERT_EQ(weak_hashset_contains_op(context, set, IMM_UNIT), IMM_FALSE);

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

    // 强引用
    vector = vector_make_op(context, symbols_len);
    for (size_t i = 0; i < symbols_len; i++) {
        vector_ref(vector, i) = symbol_make_from_cstr_untracked_op(context, symbols[i]);
    }
    vector2 = vector_make_op(context, other_symbol_len);
    for (size_t i = 0; i < other_symbol_len; i++) {
        vector_ref(vector2, i) = symbol_make_from_cstr_untracked_op(context, other_symbol_table[i]);
    }

    gc_collect(context);
    // put symbol
    for (size_t i = 0; i < vector_len(vector); i++) {
        weak_hashset_put_op(context, set, vector_ref(vector, i));
    }
    ASSERT_EQ(weak_hashset_size_op(context, set), symbols_len);

    gc_collect(context);
    // test contains
    for (size_t i = 0; i < vector_len(vector); i++) {
        ASSERT_EQ(weak_hashset_contains_op(context, set, vector_ref(vector, i)), IMM_TRUE);
    }
    size_t size = weak_hashset_size_op(context, set);
    ASSERT_EQ(size, symbols_len);

    gc_collect(context);
    // test remove
    for (size_t i = 0; i < vector_len(vector); i++) {
        weak_hashset_remove_op(context, set, vector_ref(vector, i));
    }
    for (size_t i = 0; i < vector_len(vector); i++) {
        ASSERT_EQ(weak_hashset_contains_op(context, set, vector_ref(vector, i)), IMM_FALSE);
    }
    ASSERT_EQ(weak_hashset_size_op(context, set), 0);

    gc_collect(context);
    // put symbol 2
    for (size_t i = 0; i < vector_len(vector); i++) {
        weak_hashset_put_op(context, set, vector_ref(vector, i));
        ASSERT_EQ(weak_hashset_contains_op(context, set, vector_ref(vector, i)), IMM_TRUE);
    }
    for (size_t i = 0; i < vector_len(vector2); i++) {
        weak_hashset_put_op(context, set2, vector_ref(vector2, i));
        ASSERT_EQ(weak_hashset_contains_op(context, set2, vector_ref(vector2, i)), IMM_TRUE);
    }

    gc_collect(context);
    // test contains 2
    for (size_t i = 0; i < vector_len(vector); i++) {
        ASSERT_EQ(weak_hashset_contains_op(context, set, vector_ref(vector, i)), IMM_TRUE);
    }
    for (size_t i = 0; i < vector_len(vector2); i++) {
        ASSERT_EQ(weak_hashset_contains_op(context, set2, vector_ref(vector2, i)), IMM_TRUE);
    }
    ASSERT_EQ(weak_hashset_size_op(context, set), symbols_len);
    ASSERT_EQ(weak_hashset_size_op(context, set2), other_symbol_len);

    gc_collect(context);
    // put set2 all to set
    for (size_t i = 0; i < vector_len(vector2); i++) {
        weak_hashset_put_op(context, set, vector_ref(vector2, i));
    }
    ASSERT_EQ(weak_hashset_size_op(context, set), symbols_len + other_symbol_len);
    ASSERT_EQ(weak_hashset_size_op(context, set2), other_symbol_len);

    gc_collect(context);
    // put all test, set
    for (size_t i = 0; i < vector_len(vector); i++) {
        ASSERT_EQ(weak_hashset_contains_op(context, set, vector_ref(vector, i)), IMM_TRUE);
    }
    for (size_t i = 0; i < vector_len(vector2); i++) {
        ASSERT_EQ(weak_hashset_contains_op(context, set, vector_ref(vector2, i)), IMM_TRUE);
    }
    ASSERT_EQ(weak_hashset_size_op(context, set), symbols_len + other_symbol_len);
    ASSERT_EQ(weak_hashset_size_op(context, set2), other_symbol_len);

    gc_collect(context);
    // dup put test 2
    weak_hashset_put_op(context, set, vector_ref(vector, 0));
    ASSERT_EQ(weak_hashset_size_op(context, set), symbols_len + other_symbol_len);

    gc_collect(context);
    // clear test 2
    weak_hashset_clear_op(context, set);
    for (size_t i = 0; i < vector_len(vector); i++) {
        ASSERT_EQ(weak_hashset_contains_op(context, set, vector_ref(vector, i)), IMM_FALSE);
    }
    for (size_t i = 0; i < vector_len(vector2); i++) {
        ASSERT_EQ(weak_hashset_contains_op(context, set, vector_ref(vector2, i)), IMM_FALSE);
    }
    ASSERT_EQ(weak_hashset_size_op(context, set), 0);

    gc_collect(context);
    // re put test 2
    for (size_t i = 0; i < vector_len(vector2); i++) {
        weak_hashset_put_op(context, set, vector_ref(vector2, i));
    }
    for (size_t i = 0; i < vector_len(vector); i++) {
        ASSERT_EQ(weak_hashset_contains_op(context, set, vector_ref(vector, i)), IMM_FALSE);
    }
    for (size_t i = 0; i < vector_len(vector2); i++) {
        ASSERT_EQ(weak_hashset_contains_op(context, set, vector_ref(vector2, i)), IMM_TRUE);
    }
    ASSERT_EQ(weak_hashset_size_op(context, set), weak_hashset_size_op(context, set2));
    ASSERT_EQ(weak_hashset_size_op(context, set), other_symbol_len);

    gc_collect(context);
    // hashset->vector test
    obj = weak_hashset_to_vector_op(context, set2);
    ASSERT_EQ(vector_len(obj), weak_hashset_size_op(context, set2));
    UTEST_PRINTF("weak_hashset->vector:\n");
    for (size_t i = 0; i < vector_len(obj); i++) {
        ASSERT_TRUE(is_symbol(vector_ref(obj, i)));
        UTEST_PRINTF("key: %s\n", symbol_get_cstr(vector_ref(obj, i)));
        ASSERT_EQ(weak_hashset_contains_op(context, set2, vector_ref(obj, i)), IMM_TRUE);
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

UTEST(weak_hashset_test, strong_ref_remove_test) {
    context_t context = context_make(0x10000, 2, 0x100000);
    gc_var5(context, obj, set, set2, vector, vector2);
    uint64_t start, time;

    // 弱引用
    set = weak_hashset_make_op(context, 2, DEFAULT_HASH_SET_MAP_LOAD_FACTOR);
    set2 = weak_hashset_make_op(context, 2, DEFAULT_HASH_SET_MAP_LOAD_FACTOR);
    ASSERT_TRUE(is_object(set));
    ASSERT_TRUE(is_weak_hashset(set));
    ASSERT_TRUE(is_weak_hashset(set2));

    ASSERT_EQ(weak_hashset_contains_op(context, set, IMM_UNIT), IMM_FALSE);

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

    // 强引用
    vector = vector_make_op(context, symbols_len);
    for (size_t i = 0; i < symbols_len; i++) {
        vector_ref(vector, i) = symbol_make_from_cstr_untracked_op(context, symbols[i]);
    }
    vector2 = vector_make_op(context, other_symbol_len);
    for (size_t i = 0; i < other_symbol_len; i++) {
        vector_ref(vector2, i) = symbol_make_from_cstr_untracked_op(context, other_symbol_table[i]);
    }

    gc_collect(context);
    // put symbol
    for (size_t i = 0; i < vector_len(vector); i++) {
        weak_hashset_put_op(context, set, vector_ref(vector, i));
    }
    ASSERT_EQ(weak_hashset_size_op(context, set), symbols_len);
    for (size_t i = 0; i < vector_len(vector2); i++) {
        weak_hashset_put_op(context, set2, vector_ref(vector2, i));
    }
    ASSERT_EQ(weak_hashset_size_op(context, set2), other_symbol_len);

    // remove strong_ref
    for (size_t i = 0; i < vector_len(vector); i++) {
        vector_set(vector, i, IMM_UNIT);
    }
    gc_collect(context);
    ASSERT_EQ(weak_hashset_size_op(context, set), 0);

    vector_set(vector2, 0, IMM_UNIT);
    vector_set(vector2, 1, IMM_UNIT);
    vector_set(vector2, 2, IMM_UNIT);
    vector_set(vector2, 3, IMM_UNIT);
    vector_set(vector2, 4, IMM_UNIT);
    gc_collect(context);
    ASSERT_EQ(weak_hashset_size_op(context, set2), other_symbol_len - 5);

    obj = weak_hashset_to_vector_op(context, set2);
    UTEST_PRINTF("weak_hashset->vector:\n");
    for (size_t i = 0; i < vector_len(obj); i++) {
        if (is_symbol(vector_ref(obj, i))) {
            UTEST_PRINTF("key: %s\n", symbol_get_cstr(vector_ref(obj, i)));
            ASSERT_EQ(weak_hashset_contains_op(context, set2, vector_ref(obj, i)), IMM_TRUE);
        } else {
            UTEST_PRINTF("key: '()\n");
            ASSERT_TRUE(is_imm_unit(vector_ref(obj, i)));
        }
    }


    obj = IMM_UNIT;
    set = IMM_UNIT;
    set2 = IMM_UNIT;
    vector = IMM_UNIT;
    ASSERT_EQ(context->saves, &__gc_var_dream5__);
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