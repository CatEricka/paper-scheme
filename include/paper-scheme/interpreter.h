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
#include <paper-scheme/opcodes.h>
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
#define assert_init_done(context) assert((context)->init_done)

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
EXPORT_API void interpreter_destory(context_t context);

/******************************************************************************
                         global_symbol_table ����
******************************************************************************/
/**
 * ��ȫ�ַ��ű���� symbol ��������� symbol
 * @param context
 * @param symbol symbol ����
 */
EXPORT_API OUT NOTNULL GC void
global_symbol_add_from_symbol_obj(REF NOTNULL context_t context, REF NOTNULL object symbol);

/**
 * ��ȫ�ַ��ű��в����Ƿ���ڴ˷���
 * @param context
 * @param symbol
 * @return ���ڷ��� IMM_TRUE, ���򷵻� IMM_UNIT
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
                          global_environment ����
******************************************************************************/



/******************************************************************************
                               current_env ����
******************************************************************************/



/******************************************************************************
                                    TODO Ԫѭ��
******************************************************************************/
EXPORT_API uint32_t eval_apply_loop(context_t context, enum opcode_e opcode);


#endif //PAPER_SCHEME_INTERPRETER_H
