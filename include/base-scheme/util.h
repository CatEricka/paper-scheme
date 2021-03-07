#ifndef _BASE_SCHEME_ASSERT_HELPER_HEADER_
#define _BASE_SCHEME_ASSERT_HELPER_HEADER_
#pragma once

#include <assert.h>

//¾²Ì¬¶ÏÑÔ
#define ___STATIC_ASSERT(COND,MSG) typedef char ___static_assertion_##MSG[(!!(COND))*2-1]
#define COMPILE_TIME_ASSERT3(X,L) ___STATIC_ASSERT(X,static_assertion_at_line_##L)
#define COMPILE_TIME_ASSERT2(X,L) COMPILE_TIME_ASSERT3(X,L)
#define COMPILE_TIME_ASSERT(X)    COMPILE_TIME_ASSERT2(X,__LINE__)

// API±ê¼Ç
#define EXPORT_API

#endif