#include "lib/utest.h"
#include <paper-scheme/runtime.h>

UTEST(hashmap_test, test) {
    context_t context = context_make(0x10000, 2, 0x100000);
    gc_var7(context, obj, map, map2, vector, vector2, k, v);
    uint64_t start, time;

    map = hashmap_make_op(context, 2, DEFAULT_HASH_SET_MAP_LOAD_FACTOR);
    map2 = hashmap_make_op(context, 2, DEFAULT_HASH_SET_MAP_LOAD_FACTOR);
    ASSERT_TRUE(is_object(map));
    ASSERT_FALSE(is_hashset(map));
    ASSERT_TRUE(is_hashmap(map));
    ASSERT_TRUE(is_object(map2));
    ASSERT_FALSE(is_hashset(map2));
    ASSERT_TRUE(is_hashmap(map2));

    ASSERT_EQ(hashmap_contains_key_op(context, map, IMM_UNIT), IMM_FALSE);
    ASSERT_EQ(hashmap_contains_key_op(context, map, IMM_UNIT), IMM_FALSE);

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
    char *values[] = {
            "value0",
            "value1",
            "value2",
            "value3",
            "value4",
            "value5",
            "value6",
            "value7",
            "value8",
            "value9",
    };
    char *other_symbols[] = {
            "other_symbol0",
            "other_symbol1",
            "other_symbol2",
            "other_symbol3",
            "other_symbol4",
            "other_symbol5",
            "other_symbol6",
            "other_symbol7",
            "other_symbol8",
            "other_symbol9",
    };

    char *other_values[] = {
            "other_value0",
            "other_value1",
            "other_value2",
            "other_value3",
            "other_value4",
            "other_value5",
            "other_value6",
            "other_value7",
            "other_value8",
            "other_value9",
    };
    size_t symbols_len = sizeof(symbols) / sizeof(char *);
    size_t other_symbol_len = sizeof(other_symbols) / sizeof(char *);

    // make test data
    vector = vector_make_op(context, symbols_len);
    for (size_t i = 0; i < symbols_len; i++) {
        k = symbol_make_from_cstr_op(context, symbols[i]);
        v = string_make_from_cstr_op(context, values[i]);
        obj = pair_make_op(context, k, v);
        vector_ref(vector, i) = obj;
    }
    vector2 = vector_make_op(context, other_symbol_len);
    for (size_t i = 0; i < other_symbol_len; i++) {
        k = symbol_make_from_cstr_op(context, other_symbols[i]);
        v = string_make_from_cstr_op(context, other_values[i]);
        obj = pair_make_op(context, k, v);
        vector_ref(vector2, i) = obj;
    }

    // put symbol
    for (size_t i = 0; i < vector_len(vector); i++) {
        k = pair_car(vector_ref(vector, i));
        v = pair_cdr(vector_ref(vector, i));
        obj = hashmap_put_op(context, map, k, v);
        ASSERT_EQ(obj, IMM_UNIT);
        ASSERT_EQ(hashmap_contains_key_op(context, map, k), IMM_TRUE);
        ASSERT_EQ(hashmap_contains_key_op(context, map, v), IMM_FALSE);
    }
    ASSERT_EQ(hashmap_size(map), symbols_len);
    for (size_t i = 0; i < vector_len(vector2); i++) {
        k = pair_car(vector_ref(vector2, i));
        v = pair_cdr(vector_ref(vector2, i));
        obj = hashmap_put_op(context, map2, k, v);
        ASSERT_EQ(obj, IMM_UNIT);
        ASSERT_EQ(hashmap_contains_key_op(context, map2, k), IMM_TRUE);
        ASSERT_EQ(hashmap_contains_key_op(context, map2, v), IMM_FALSE);
    }
    ASSERT_EQ(hashmap_size(map2), other_symbol_len);

    // test contains
    for (size_t i = 0; i < vector_len(vector); i++) {
        k = pair_car(vector_ref(vector, i));
        ASSERT_EQ(hashmap_contains_key_op(context, map, k), IMM_TRUE);
    }
    for (size_t i = 0; i < vector_len(vector2); i++) {
        k = pair_car(vector_ref(vector2, i));
        ASSERT_EQ(hashmap_contains_key_op(context, map2, k), IMM_TRUE);
    }

    // test get
    for (size_t i = 0; i < vector_len(vector); i++) {
        k = pair_car(vector_ref(vector, i));
        v = pair_cdr(vector_ref(vector, i));
        obj = hashmap_get_op(context, map, k);
        ASSERT_EQ(v, obj);
    }
    for (size_t i = 0; i < vector_len(vector2); i++) {
        k = pair_car(vector_ref(vector2, i));
        v = pair_cdr(vector_ref(vector2, i));
        obj = hashmap_get_op(context, map2, k);
        ASSERT_EQ(v, obj);
    }

    // put all test
    hashmap_put_all_op(context, map, map2);
    for (size_t i = 0; i < vector_len(vector); i++) {
        k = pair_car(vector_ref(vector, i));
        ASSERT_EQ(hashmap_contains_key_op(context, map, k), IMM_TRUE);
    }
    for (size_t i = 0; i < vector_len(vector2); i++) {
        k = pair_car(vector_ref(vector2, i));
        v = pair_cdr(vector_ref(vector2, i));
        ASSERT_EQ(hashmap_contains_key_op(context, map, k), IMM_TRUE);
    }
    ASSERT_EQ(hashmap_size(map), symbols_len + other_symbol_len);
    ASSERT_EQ(hashmap_size(map2), other_symbol_len);

    // put all -> get test
    for (size_t i = 0; i < vector_len(vector); i++) {
        k = pair_car(vector_ref(vector, i));
        v = pair_cdr(vector_ref(vector, i));
        obj = hashmap_get_op(context, map, k);
        ASSERT_EQ(v, obj);
    }
    for (size_t i = 0; i < vector_len(vector2); i++) {
        k = pair_car(vector_ref(vector2, i));
        v = pair_cdr(vector_ref(vector2, i));
        obj = hashmap_get_op(context, map, k);
        ASSERT_EQ(v, obj);
    }

    // dup put test
    k = pair_car(vector_ref(vector, 0));
    v = char_imm_make('x');
    obj = hashmap_put_op(context, map, k, v);
    ASSERT_EQ(obj, pair_cdr(vector_ref(vector, 0)));
    ASSERT_NE(obj, v);
    ASSERT_EQ(hashmap_size(map), symbols_len + other_symbol_len);

    // clear test
    hashmap_clear_op(context, map);
    for (size_t i = 0; i < vector_len(vector); i++) {
        k = pair_car(vector_ref(vector, i));
        ASSERT_EQ(hashmap_contains_key_op(context, map, k), IMM_FALSE);
        ASSERT_EQ(hashmap_get_op(context, map, k), IMM_UNIT);
    }
    for (size_t i = 0; i < vector_len(vector2); i++) {
        k = pair_car(vector_ref(vector2, i));
        ASSERT_EQ(hashmap_contains_key_op(context, map, k), IMM_FALSE);
        ASSERT_EQ(hashmap_get_op(context, map, k), IMM_UNIT);
    }
    ASSERT_EQ(hashmap_size(map), 0);

    // re put test
    hashmap_put_all_op(context, map, map2);
    for (size_t i = 0; i < vector_len(vector); i++) {
        k = pair_car(vector_ref(vector, i));
        ASSERT_EQ(hashmap_contains_key_op(context, map, k), IMM_FALSE);
    }
    for (size_t i = 0; i < vector_len(vector2); i++) {
        k = pair_car(vector_ref(vector2, i));
        ASSERT_EQ(hashmap_contains_key_op(context, map, k), IMM_TRUE);
    }
    ASSERT_EQ(hashmap_size(map), hashmap_size(map2));
    ASSERT_EQ(hashmap_size(map), other_symbol_len);

    // hashmap->vector test
    obj = hashmap_to_vector_op(context, map2);
    ASSERT_EQ(vector_len(obj), hashmap_size(map2));
    UTEST_PRINTF("hashmap->vector:\n");
    for (size_t i = 0; i < vector_len(obj); i++) {
        k = pair_car(vector_ref(obj, i));
        v = pair_cdr(vector_ref(obj, i));
        ASSERT_TRUE(is_pair(vector_ref(obj, i)));
        ASSERT_TRUE(is_symbol(k));
        ASSERT_TRUE(is_string(v));
        UTEST_PRINTF("key: %s, value: %s\n", symbol_get_cstr(k), string_get_cstr(v));
        ASSERT_EQ(hashmap_contains_key_op(context, map2, k), IMM_TRUE);
    }

    // remove all test
    hashmap_clear_op(context, map);
    hashmap_put_all_op(context, map, map2);
    for (size_t i = 0; i < vector_len(vector); i++) {
        k = pair_car(vector_ref(vector, i));
        v = pair_cdr(vector_ref(vector, i));
        ASSERT_EQ(hashmap_remove_op(context, map, k), IMM_UNIT);
    }
    for (size_t i = 0; i < vector_len(vector2); i++) {
        k = pair_car(vector_ref(vector2, i));
        v = pair_cdr(vector_ref(vector2, i));
        ASSERT_EQ(hashmap_remove_op(context, map, k), v);
    }
    ASSERT_NE(hashmap_size(map), hashmap_size(map2));
    ASSERT_EQ(hashmap_size(map), 0);
    obj = hashmap_to_vector_op(context, map);
    ASSERT_EQ(vector_len(obj), 0);

    obj = IMM_UNIT;
    map = IMM_UNIT;
    map2 = IMM_UNIT;
    vector = IMM_UNIT;
    ASSERT_EQ(context->saves, &__gc_var_dream7__);
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