#include "lib/utest.h"
#include <paper-scheme/interpreter.h>


UTEST(scheme_stack_test, test) {
    context_t context = interpreter_create(0x8000000, 2, 0x32000000);
    gc_var5(context, var, val, slot, env, obj);

    scheme_stack_reset(context);
    ASSERT_EQ(IMM_UNIT, scheme_stack_return(context, IMM_UNIT));
    gc_collect(context);

    // new env 1
    new_frame_push_current_env(context);
    context->op_code = 1;
    context->args = pair_make_op(context, IMM_TRUE, IMM_TRUE);
    context->code = pair_make_op(context, IMM_FALSE, IMM_FALSE);
    gc_collect(context);
    var = symbol_make_from_cstr_op(context, "variable1");
    val = string_make_from_cstr_op(context, "value1");
    gc_collect(context);
    new_slot_in_current_env(context, var, val);
    // stack push 1
    gc_collect(context);
    scheme_stack_save(context, context->op_code, context->args, context->code);
    gc_collect(context);

    // new env 2
    new_frame_push_current_env(context);
    context->op_code = 2;
    context->args = pair_make_op(context, char_imm_make('x'), char_imm_make('x'));
    context->code = pair_make_op(context, char_imm_make('d'), char_imm_make('d'));
    gc_collect(context);
    var = symbol_make_from_cstr_op(context, "variable2");
    val = string_make_from_cstr_op(context, "value2");
    gc_collect(context);
    new_slot_in_current_env(context, var, val);
    // stack push 2
    gc_collect(context);
    scheme_stack_save(context, context->op_code, context->args, context->code);
    gc_collect(context);

    // new env 3
    new_frame_push_current_env(context);
    // push 3
    scheme_stack_save(context, context->op_code, context->args, context->code);
    // pop 3
    scheme_stack_return(context, char_imm_make('r'));
    gc_collect(context);

    // pop 2
    // env 2
    scheme_stack_return(context, char_imm_make('r'));
    gc_collect(context);
    ASSERT_EQ(context->value, char_imm_make('r'));
    ASSERT_EQ(context->op_code, 2);
    ASSERT_EQ(pair_car(context->args), char_imm_make('x'));
    ASSERT_EQ(pair_cdr(context->args), char_imm_make('x'));
    ASSERT_EQ(pair_cdr(context->code), char_imm_make('d'));
    ASSERT_EQ(pair_cdr(context->code), char_imm_make('d'));
    slot = find_slot_in_current_env(context, symbol_make_from_cstr_op(context, "variable2"), 0);
    slot = find_slot_in_current_env(context, symbol_make_from_cstr_op(context, "variable2"), 0);
    gc_collect(context);
    ASSERT_NE(slot, IMM_UNIT);
    var = env_slot_var(slot);
    val = env_slot_value(slot);
    gc_collect(context);
    ASSERT_TRUE(symbol_equals(context, var, symbol_make_from_cstr_op(context, "variable2")));
    ASSERT_TRUE(string_equals(context, val, string_make_from_cstr_op(context, "value2")));
    gc_collect(context);

    // pop 1
    // env 1
    scheme_stack_return(context, char_imm_make('q'));
    ASSERT_EQ(context->value, char_imm_make('q'));
    ASSERT_EQ(context->op_code, 1);
    ASSERT_EQ(pair_car(context->args), IMM_TRUE);
    ASSERT_EQ(pair_cdr(context->args), IMM_TRUE);
    ASSERT_EQ(pair_cdr(context->code), IMM_FALSE);
    ASSERT_EQ(pair_cdr(context->code), IMM_FALSE);
    gc_collect(context);
    slot = find_slot_in_current_env(context, symbol_make_from_cstr_op(context, "variable2"), 0);
    ASSERT_EQ(slot, IMM_UNIT);
    gc_collect(context);
    slot = find_slot_in_current_env(context, symbol_make_from_cstr_op(context, "variable1"), 0);
    ASSERT_NE(slot, IMM_UNIT);
    var = env_slot_var(slot);
    val = env_slot_value(slot);
    gc_collect(context);
    ASSERT_TRUE(symbol_equals(context, var, symbol_make_from_cstr_op(context, "variable1")));
    ASSERT_TRUE(string_equals(context, val, string_make_from_cstr_op(context, "value1")));
    gc_collect(context);

    // empty
    scheme_stack_reset(context);
    ASSERT_EQ(IMM_UNIT, scheme_stack_return(context, IMM_UNIT));
    gc_collect(context);

    assert(context->saves == &__gc_var_dream5__);
    gc_release_var(context);
    interpreter_destroy(context);
}