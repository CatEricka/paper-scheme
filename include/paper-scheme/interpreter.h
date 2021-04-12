#ifndef PAPER_SCHEME_INTERPRETER_H
#define PAPER_SCHEME_INTERPRETER_H
#pragma once


/**
 * interpreter.h interpreter.c
 * 包含解释器元循环实现和 API
 *
 * 该解释器实际为 AST Walker, 通过 opcodes dispatch 循环对 sexp(pair) 结构进行 EVAL-APPLY 元循环
 *
 * 性能上存在优化的余地, 但是最优先的不是性能而是代码的可读性与实验特性添加的便利性
 */


#include <paper-scheme/runtime.h>
#include <paper-scheme/token.h>



/******************************************************************************
                                  常用常量
******************************************************************************/
#define ZERO    i64_imm_make(0)
#define ONE     i64_imm_make(1)
#define TWO     i64_imm_make(2)
#define THREE   i64_imm_make(3)


/******************************************************************************
                                解释器初始化
******************************************************************************/

/**
 * 创建解释器
 * @param heap_init_size
 * @param heap_growth_scale
 * @param heap_max_size
 * @return
 */
EXPORT_API context_t interpreter_create(size_t heap_init_size, size_t heap_growth_scale, size_t heap_max_size);

/**
 * 析构解释器
 * @param context
 */
EXPORT_API void interpreter_destroy(context_t context);

/******************************************************************************
                            关键字 (syntax) 相关
******************************************************************************/
/**
 * 检查给定的 symbol 是否是 syntax
 * @param context
 * @param symbol
 * @return IMM_TRUE/IMM_FALSE
 */
EXPORT_API object symbol_is_syntax(context_t context, object symbol);

/******************************************************************************
                         global_symbol_table 操作
******************************************************************************/
/**
 * 生成当前环境下唯一 symbol
 * @param context
 * @return symbol
 */
EXPORT_API OUT NOTNULL GC object gensym(REF NOTNULL context_t context);
/**
 * 向全局符号表添加 symbol 并返回这个 symbol
 * @param context
 * @param symbol symbol 对象
 * @param 添加后的 symbol 对象, 如果存在则返回原始对象
 */
EXPORT_API OUT NOTNULL GC object
global_symbol_add_from_symbol_obj(REF NOTNULL context_t context, REF NOTNULL object symbol);

/**
 * 从全局符号表中查找是否存在此符号
 * @param context
 * @param symbol
 * @return 存在返回 IMM_TRUE, 否则返回 IMM_FALSE
 */
EXPORT_API OUT NOTNULL GC object
global_symbol_found(REF NOTNULL context_t context, REF NOTNULL object symbol);

/**
 * 返回全局符号表全部内容
 * @param context
 * @return vector, vector 中可能存在 IMM_UNIT
 */
EXPORT_API OUT NOTNULL GC object
global_symbol_all_symbol(REF NOTNULL context_t context);

/**
 * 构造 symbol 类型对象
 * <p>symbol_len() 运算结果不包括 '\0', 但是 object->symbol.len 包括 '\0', 这是为了方便运行时计算对象大小</p>
 * <p>该方法会把 symbol 添加进全局符号表</p>
 * @param context
 * @param cstr C字符串, '\0'结尾
 * @return
 */
EXPORT_API OUT NOTNULL GC object
symbol_make_from_cstr_op(REF NOTNULL context_t context, COPY char *cstr);

/**
 * string 转 symbol
 * <p>该方法会把 symbol 添加进全局符号表</p>
 * @param context
 * @param string
 * @return
 */
EXPORT_API OUT NOTNULL GC object
string_to_symbol_op(REF NOTNULL context_t context, NOTNULL COPY object str);

/**
 * string_buffer 转换为 symbol, 深拷贝
 * <p>该方法会把 symbol 添加进全局符号表</p>
 * @param context
 * @param str_buffer string_buffer
 * @return symbol
 */
EXPORT_API OUT NOTNULL GC object
string_buffer_to_symbol_op(REF NOTNULL context_t context, NOTNULL COPY object str_buffer);


/******************************************************************************
                             scheme_stack 操作
******************************************************************************/
/**
 * 重置 scheme stack
 * @param context
 */
EXPORT_API void scheme_stack_reset(context_t context);
/**
 * push scheme stack
 * @param context
 * @param op
 * @param args
 * @param code
 */
EXPORT_API GC void scheme_stack_save(context_t context, enum opcode_e op, object args, object code);
/**
 * pop scheme stack
 * @param context
 * @param value
 * @return
 */
EXPORT_API object scheme_stack_return(context_t context, object value);



/******************************************************************************
                              environment 操作
******************************************************************************/
/**
 * 从 context->current_env 向上查找 env_slot
 * <p>不会触发 GC</p>
 * @param context
 * @param symbol
 * @param all 1: 持续查找到全局 env; 0: 只查找当前 env
 * @return env_slot / IMM_UNIT (未找到)
 */
EXPORT_API object find_slot_in_current_env(REF NOTNULL context_t context, object symbol, int all);
/**
 * 从 context->current_env 向上查找 env_slot
 * <p>不会触发 GC</p>
 * @param context
 * @param symbol
 * @param all 1: 持续查找到全局 env; 0: 只查找当前 environment
 * @return pair(symbol, value) / IMM_UNIT (未找到)
 */
EXPORT_API object find_slot_in_spec_env(REF NOTNULL context_t context, object env, object symbol, int all);
/**
 * 在当前 context->current_env 插入新 env_slot
 * @param context
 * @param symbol
 * @param value
 */
EXPORT_API GC void new_slot_in_current_env(context_t context, object symbol, object value);
/**
 * 从特定 env frame 插入新 env_slot
 * @param context
 * @param symbol
 * @param value
 * @param env
 */
EXPORT_API GC void new_slot_in_spec_env(context_t context, object symbol, object value, object env);
/**
 * 以 context->current_env 作为上层, 创建新 frame, 赋值给 context->current_env
 * @param context
 * @param old_env 一般是 context->current_env
 */
EXPORT_API GC void new_frame_push_current_env(context_t context);
/**
 * 以 old_env 作为上层, 创建新 frame, 赋值给 context->current_env
 * @param context
 * @param old_env
 */
EXPORT_API GC void new_frame_push_spec_env(context_t context, object old_env);


/******************************************************************************
                              load_stack 操作
******************************************************************************/



/******************************************************************************
                                TODO 元循环
******************************************************************************/
EXPORT_API uint32_t eval_apply_loop(context_t context, enum opcode_e opcode);



/******************************************************************************
                                  文件读入
******************************************************************************/
EXPORT_API GC void interpreter_load_cstr(context_t context, const char *cstr);

EXPORT_API GC void interpreter_load_file(context_t context, FILE *file);

EXPORT_API GC void interpreter_load_file_with_name(context_t context, FILE *file, const char *file_name);

#endif //PAPER_SCHEME_INTERPRETER_H
