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


/******************************************************************************
                         global_symbol_table ����
******************************************************************************/



/******************************************************************************
                          global_environment ����
******************************************************************************/



/******************************************************************************
                               current_env ����
******************************************************************************/



#endif //PAPER_SCHEME_INTERPRETER_H
