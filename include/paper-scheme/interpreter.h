#ifndef PAPER_SCHEME_INTERPRETER_H
#define PAPER_SCHEME_INTERPRETER_H
#pragma once


/**
 * interpreter.h interpreter.c
 * ����������Ԫѭ��ʵ�ֺ� API
 *
 * �ý�����ʵ��Ϊ AST Walker, ͨ�� opcodes dispatch ѭ���� sexp(pair) �ṹ���� EVAL-APPLY Ԫѭ��
 *
 * �����ϴ����Ż������, ���������ȵĲ������ܶ��Ǵ���Ŀɶ�����ʵ��������ӵı�����
 */


#include <paper-scheme/runtime.h>
#include <paper-scheme/token.h>



/******************************************************************************
                                  ���ó���
******************************************************************************/
#define ZERO    i64_imm_make(0)
#define ONE     i64_imm_make(1)
#define TWO     i64_imm_make(2)
#define THREE   i64_imm_make(3)


/******************************************************************************
                                ��������ʼ��
******************************************************************************/

/**
 * ����������
 * @param heap_init_size
 * @param heap_growth_scale
 * @param heap_max_size
 * @return
 */
EXPORT_API context_t interpreter_create(size_t heap_init_size, size_t heap_growth_scale, size_t heap_max_size);

/**
 * ����������
 * @param context
 */
EXPORT_API void interpreter_destroy(context_t context);

/******************************************************************************
                            �ؼ��� (syntax) ���
******************************************************************************/
/**
 * �������� symbol �Ƿ��� syntax
 * @param context
 * @param symbol
 * @return IMM_TRUE/IMM_FALSE
 */
EXPORT_API object symbol_is_syntax(context_t context, object symbol);

/******************************************************************************
                         global_symbol_table ����
******************************************************************************/
/**
 * ���ɵ�ǰ������Ψһ symbol
 * @param context
 * @return symbol
 */
EXPORT_API OUT NOTNULL GC object gensym(REF NOTNULL context_t context);
/**
 * ��ȫ�ַ��ű���� symbol ��������� symbol
 * @param context
 * @param symbol symbol ����
 * @param ��Ӻ�� symbol ����, ��������򷵻�ԭʼ����
 */
EXPORT_API OUT NOTNULL GC object
global_symbol_add_from_symbol_obj(REF NOTNULL context_t context, REF NOTNULL object symbol);

/**
 * ��ȫ�ַ��ű��в����Ƿ���ڴ˷���
 * @param context
 * @param symbol
 * @return ���ڷ��� IMM_TRUE, ���򷵻� IMM_FALSE
 */
EXPORT_API OUT NOTNULL GC object
global_symbol_found(REF NOTNULL context_t context, REF NOTNULL object symbol);

/**
 * ����ȫ�ַ��ű�ȫ������
 * @param context
 * @return vector, vector �п��ܴ��� IMM_UNIT
 */
EXPORT_API OUT NOTNULL GC object
global_symbol_all_symbol(REF NOTNULL context_t context);

/**
 * ���� symbol ���Ͷ���
 * <p>symbol_len() ������������ '\0', ���� object->symbol.len ���� '\0', ����Ϊ�˷�������ʱ��������С</p>
 * <p>�÷������ symbol ��ӽ�ȫ�ַ��ű�</p>
 * @param context
 * @param cstr C�ַ���, '\0'��β
 * @return
 */
EXPORT_API OUT NOTNULL GC object
symbol_make_from_cstr_op(REF NOTNULL context_t context, COPY char *cstr);

/**
 * string ת symbol
 * <p>�÷������ symbol ��ӽ�ȫ�ַ��ű�</p>
 * @param context
 * @param string
 * @return
 */
EXPORT_API OUT NOTNULL GC object
string_to_symbol_op(REF NOTNULL context_t context, NOTNULL COPY object str);

/**
 * string_buffer ת��Ϊ symbol, ���
 * <p>�÷������ symbol ��ӽ�ȫ�ַ��ű�</p>
 * @param context
 * @param str_buffer string_buffer
 * @return symbol
 */
EXPORT_API OUT NOTNULL GC object
string_buffer_to_symbol_op(REF NOTNULL context_t context, NOTNULL COPY object str_buffer);


/******************************************************************************
                             scheme_stack ����
******************************************************************************/
/**
 * ���� scheme stack
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
                              environment ����
******************************************************************************/
/**
 * �� context->current_env ���ϲ��� env_slot
 * <p>���ᴥ�� GC</p>
 * @param context
 * @param symbol
 * @param all 1: �������ҵ�ȫ�� env; 0: ֻ���ҵ�ǰ env
 * @return env_slot / IMM_UNIT (δ�ҵ�)
 */
EXPORT_API object find_slot_in_current_env(REF NOTNULL context_t context, object symbol, int all);
/**
 * �� context->current_env ���ϲ��� env_slot
 * <p>���ᴥ�� GC</p>
 * @param context
 * @param symbol
 * @param all 1: �������ҵ�ȫ�� env; 0: ֻ���ҵ�ǰ environment
 * @return pair(symbol, value) / IMM_UNIT (δ�ҵ�)
 */
EXPORT_API object find_slot_in_spec_env(REF NOTNULL context_t context, object env, object symbol, int all);
/**
 * �ڵ�ǰ context->current_env ������ env_slot
 * @param context
 * @param symbol
 * @param value
 */
EXPORT_API GC void new_slot_in_current_env(context_t context, object symbol, object value);
/**
 * ���ض� env frame ������ env_slot
 * @param context
 * @param symbol
 * @param value
 * @param env
 */
EXPORT_API GC void new_slot_in_spec_env(context_t context, object symbol, object value, object env);
/**
 * �� context->current_env ��Ϊ�ϲ�, ������ frame, ��ֵ�� context->current_env
 * @param context
 * @param old_env һ���� context->current_env
 */
EXPORT_API GC void new_frame_push_current_env(context_t context);
/**
 * �� old_env ��Ϊ�ϲ�, ������ frame, ��ֵ�� context->current_env
 * @param context
 * @param old_env
 */
EXPORT_API GC void new_frame_push_spec_env(context_t context, object old_env);


/******************************************************************************
                              load_stack ����
******************************************************************************/



/******************************************************************************
                                TODO Ԫѭ��
******************************************************************************/
EXPORT_API uint32_t eval_apply_loop(context_t context, enum opcode_e opcode);



/******************************************************************************
                                  �ļ�����
******************************************************************************/
EXPORT_API GC void interpreter_load_cstr(context_t context, const char *cstr);

EXPORT_API GC void interpreter_load_file(context_t context, FILE *file);

EXPORT_API GC void interpreter_load_file_with_name(context_t context, FILE *file, const char *file_name);

#endif //PAPER_SCHEME_INTERPRETER_H
