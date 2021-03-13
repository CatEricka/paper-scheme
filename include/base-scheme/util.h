#ifndef BASE_SCHEME_ASSERT_HELPER_H
#define BASE_SCHEME_ASSERT_HELPER_H
#pragma once


/**
 * util.h
 * ���ù�����ϵͳͷ�ļ�����
 */


#define __STDC_FORMAT_MACROS

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
#define COMPILE_TIME_ASSERT3(X, L) ___STATIC_ASSERT(X,static_assertion_at_line_##L)
#define COMPILE_TIME_ASSERT2(X, L) COMPILE_TIME_ASSERT3(X,L)
// ��̬����
#define COMPILE_TIME_ASSERT(X)    COMPILE_TIME_ASSERT2(X,__LINE__)

/**
    ������ API ���
******************************************************************************/
#define EXPORT_API



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
        if (ptr == NULL) { \
            return ret; \
        } \
    } while(0)
#endif // DEBUG_ONLY



/**
    exit() ����ֵ����
******************************************************************************/
// δ֪����
#define EXIT_FAILURE_UNKNOWN (-2)
// �ڴ治��: ���������� max_size
#define EXIT_FAILURE_OUT_OF_MEMORY (-3)
// malloc() ����ʧ��
#define EXIT_FAILURE_MALLOC_FAILED (-4)

#endif // BASE_SCHEME_ASSERT_HELPER_H