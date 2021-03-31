#ifndef BASE_SCHEME_FEATURE_H
#define BASE_SCHEME_FEATURE_H
#pragma once


/**
 * feature.h
 * ���ù�����ϵͳͷ�ļ�����
 */


#ifndef __STDC_FORMAT_MACROS
# define __STDC_FORMAT_MACROS
#endif // __STDC_FORMAT_MACROS

#include <stdint.h>
#include <inttypes.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


/**
                                ��̬����
******************************************************************************/
#define ___STATIC_ASSERT(COND, MSG) typedef char ___static_assertion_##MSG[(!!(COND))*2-1]
#define ___COMPILE_TIME_ASSERT3(X, L) ___STATIC_ASSERT(X,static_assertion_at_line_##L)
#define ___COMPILE_TIME_ASSERT2(X, L) ___COMPILE_TIME_ASSERT3(X,L)
// ����ʱ����
#define compile_time_assert(X)    ___COMPILE_TIME_ASSERT2(X,__LINE__)

/**
                                �������
******************************************************************************/
// ������ API
#define EXPORT_API
// �������ڱ��, ��������Ȩ
#define IN
// �������ڱ��, ��������Ȩ (gc ����Ķ���Ҳ���� context->heap)
#define OUT
// �������ڱ��, �����������������
#define REF
// �������ڱ��, ������ִ�п�������
#define COPY
// ����ֵ���, ��Ҫ��麯��ִ�н��
#define CHECKED
// �������, �ǿ�
#define NOTNULL
// �������, ����Ϊ��
#define NULLABLE
// ���صĲ���һ��Ϊ������
#define IMM

// �ú������ܴ��� GC ���¶����ƶ�, ���øú����ĺ�����Ҫ���������뱣����
// ���д�Ⱦ��
#define GC
// �����ñ��
#define WEAK_REF


/**
                                 ������
******************************************************************************/
#ifdef DEBUG_ONLY
/**
 * <p>���ptr�Ƿ�Ϊ��ָ��<p>
 * <p>���Ϊ��, ��� message ������ ret</p>
 * <p>!���޸Ŀ�����<p>
 */
# define notnull_or_return(ptr, message, ret) \
    do { \
        if ((ptr) == NULL) { \
            fprintf(stderr, "null pointer: %s\n", message); \
            return ret; \
        } \
    } while(0)
#else
/**
 * ���ptr�Ƿ�Ϊ��ָ��
 * ���Ϊ��, ���� ret
 */
# define notnull_or_return(ptr, message, ret) \
    do { \
        if ((ptr) == NULL) { \
            return ret; \
        } \
    } while(0)
#endif // DEBUG_ONLY


#define ptr_to_uintptr(x) ((uintptr_t)(x))
#define ptr_to_intptr(x) ((intptr_t)(x))

#define is_null(x) ((x) == NULL)


/**
                                ��������
******************************************************************************/

/**
 * ��֤�� int64_t ����ʵ����������
 * @param x ����������ֵ
 * @param n ����λ��
 * @return
 */
int64_t i64_arithmetic_right_shift(int64_t x, size_t n);

/**
                             exit() ����ֵ����
******************************************************************************/
// δ֪����
#define EXIT_FAILURE_UNKNOWN (-2)
// �ڴ治��: ���������� max_size
#define EXIT_FAILURE_OUT_OF_MEMORY (-3)
// malloc() ����ʧ��
#define EXIT_FAILURE_MALLOC_FAILED (-4)


/**
                                gc ����
******************************************************************************/
// ��ʼ���ջ���
#define GC_MAX_MARK_STACK_DEEP 1024u
// ʹ�ô��� debug ��Ϣ�� gc
#define USE_DEBUG_GC 1

/**
                               ����������
******************************************************************************/
// ��������ջ��������, ����������Ȼ�����µ� stack ��
#define MAX_STACK_BLOCK_DEEP 1024
// ȫ�ַ��ű��ʼ��С
#define GLOBAL_SYMBOL_TABLE_INIT_SIZE 512
// ȫ�� environment ��ʼ��С
#define GLOBAL_ENVIRONMENT_INIT_SIZE 512
// (load "") �������Ƕ�����
#define MAX_LOAD_FILE_DEEP 64

/**
                             ������������
******************************************************************************/
#define DEFAULT_HASH_SET_MAP_LOAD_FACTOR 0.75
#define DEFAULT_HASH_SET_MAP_INIT_init_CAPACITY 16

/**
                                ��������
******************************************************************************/
// string_buffer Ĭ�ϳ�ʼ��С
#define STRING_BUFFER_DEFAULT_INIT_SIZE 16
// string_buffer Ĭ��������С
#define STRING_BUFFER_DEFAULT_GROWTH_SIZE 16

// stack push �Զ����� Ĭ��������С
#define STACK_DEFAULT_GROWTH_SIZE 16

#endif // BASE_SCHEME_FEATURE_H