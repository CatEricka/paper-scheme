#ifndef BASE_SCHEME_ASSERT_HELPER_H
#define BASE_SCHEME_ASSERT_HELPER_H
#pragma once

#include <assert.h>

#define ___STATIC_ASSERT(COND, MSG) typedef char ___static_assertion_##MSG[(!!(COND))*2-1]
#define COMPILE_TIME_ASSERT3(X, L) ___STATIC_ASSERT(X,static_assertion_at_line_##L)
#define COMPILE_TIME_ASSERT2(X, L) COMPILE_TIME_ASSERT3(X,L)
/**
 * ��̬����
 */
#define COMPILE_TIME_ASSERT(X)    COMPILE_TIME_ASSERT2(X,__LINE__)

/**
 * ������API���
 */
#define EXPORT_API

#endif