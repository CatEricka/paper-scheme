#ifndef PAPER_SCHEME_NUMBER_H
#define PAPER_SCHEME_NUMBER_H
#pragma once


/**
 * numberber.h numberber.c
 * 数学运算包
 */


#include <paper-scheme/runtime.h>

EXPORT_API GC object number_add(context_t context, object a, object b);

EXPORT_API GC object number_mul(context_t context, object a, object b);

EXPORT_API GC object number_div(context_t context, object a, object b);

EXPORT_API GC object number_int_div(context_t context, object a, object b);

EXPORT_API GC object number_sub(context_t context, object a, object b);

EXPORT_API GC object number_rem(context_t context, object a, object b);

EXPORT_API GC object number_mod(context_t context, object a, object b);

typedef int (*number_compare_func)(object, object);

EXPORT_API int number_eq(object a, object b);

EXPORT_API int number_gt(object a, object b);

EXPORT_API int number_ge(object a, object b);

EXPORT_API int number_lt(object a, object b);

EXPORT_API int number_le(object a, object b);

EXPORT_API double round_per_R5RS(double x);

#endif //PAPER_SCHEME_NUMBER_H
