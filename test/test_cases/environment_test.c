#include "lib/utest.h"
#include <paper-scheme/interpreter.h>


UTEST(environment_test, test) {
    context_t context = interpreter_create(0x8000000, 2, 0x32000000);
    gc_var5(context, var, val, slot, env, obj);

    var = symbol_make_from_cstr_op(context, "variable1");
    val = string_make_from_cstr_op(context, "value1");
    new_slot_in_current_env(context, var, val);
    slot = find_slot_in_spec_env(context, context->current_env, var, 0);
    ASSERT_EQ(var, env_slot_var(slot));
    ASSERT_EQ(val, env_slot_value(slot));

    gc_collect(context);
    var = symbol_make_from_cstr_op(context, "variable2");
    val = string_make_from_cstr_op(context, "value2");
    new_slot_in_current_env(context, var, val);
    slot = find_slot_in_current_env(context, var, 0);
    ASSERT_EQ(var, env_slot_var(slot));
    ASSERT_EQ(val, env_slot_value(slot));

    gc_collect(context);
    var = symbol_make_from_cstr_op(context, "variable3");
    val = string_make_from_cstr_op(context, "value3");
    new_slot_in_current_env(context, var, val);
    slot = find_slot_in_current_env(context, var, 0);
    ASSERT_EQ(var, env_slot_var(slot));
    ASSERT_EQ(val, env_slot_value(slot));

    gc_collect(context);
    new_frame_push_current_env(context);

    gc_collect(context);
    var = symbol_make_from_cstr_op(context, "variable1");
    val = string_make_from_cstr_op(context, "value1");
    new_slot_in_current_env(context, var, val);
    slot = find_slot_in_spec_env(context, context->current_env, var, 0);
    ASSERT_EQ(var, env_slot_var(slot));
    ASSERT_EQ(val, env_slot_value(slot));
    slot = find_slot_in_spec_env(context, context->current_env, var, 1);
    ASSERT_EQ(var, env_slot_var(slot));
    ASSERT_EQ(val, env_slot_value(slot));

    gc_collect(context);
    var = symbol_make_from_cstr_op(context, "variable2");
    val = string_make_from_cstr_op(context, "value2");
    new_slot_in_current_env(context, var, val);
    slot = find_slot_in_current_env(context, var, 0);
    ASSERT_EQ(var, env_slot_var(slot));
    ASSERT_EQ(val, env_slot_value(slot));
    slot = find_slot_in_current_env(context, var, 1);
    ASSERT_EQ(var, env_slot_var(slot));
    ASSERT_EQ(val, env_slot_value(slot));

    gc_collect(context);
    var = symbol_make_from_cstr_op(context, "variable3");
    val = string_make_from_cstr_op(context, "value3");
    slot = find_slot_in_current_env(context, var, 0);
    ASSERT_EQ(slot, IMM_UNIT);
    slot = find_slot_in_current_env(context, var, 1);
    ASSERT_NE(var, env_slot_var(slot));
    ASSERT_NE(val, env_slot_value(slot));
    ASSERT_TRUE(symbol_equals(context, var, env_slot_var(slot)));
    ASSERT_TRUE(string_equals(context, val, env_slot_value(slot)));

    gc_collect(context);
    new_frame_push_current_env(context);

    gc_collect(context);
    var = symbol_make_from_cstr_op(context, "variable4");
    val = string_make_from_cstr_op(context, "value4");
    new_slot_in_spec_env(context, var, val, context->global_environment);

    gc_collect(context);
    var = symbol_make_from_cstr_op(context, "variable4");
    val = string_make_from_cstr_op(context, "value4");
    slot = find_slot_in_current_env(context, var, 0);
    ASSERT_EQ(slot, IMM_UNIT);
    slot = find_slot_in_current_env(context, var, 1);
    ASSERT_NE(var, env_slot_var(slot));
    ASSERT_NE(val, env_slot_value(slot));
    ASSERT_TRUE(symbol_equals(context, var, env_slot_var(slot)));
    ASSERT_TRUE(string_equals(context, val, env_slot_value(slot)));
    slot = find_slot_in_spec_env(context, context->global_environment, var, 1);
    ASSERT_NE(var, env_slot_var(slot));
    ASSERT_NE(val, env_slot_value(slot));
    ASSERT_TRUE(symbol_equals(context, var, env_slot_var(slot)));
    ASSERT_TRUE(string_equals(context, val, env_slot_value(slot)));

    assert(context->saves == &__gc_var_dream5__);
    gc_release_var(context);
    interpreter_destroy(context);
}