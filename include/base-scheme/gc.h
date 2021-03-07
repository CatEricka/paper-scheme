#ifndef _BASE_SCHEME_GC_HEADER_
#define _BASE_SCHEME_GC_HEADER_
#pragma once

#include <stdlib.h>
#include <string.h>


#include "base-scheme/object.h"
#include "base-scheme/util.h"


/******************************************************************************
    垃圾回收 API
******************************************************************************/
void* base_alloc(size_t size) {
    return malloc(size);
}

void base_free(void* obj) {
    free(obj);
}

object gc_alloc(size_t size) {
    return (object)base_alloc(size);
}

EXPORT_API object gc_collect() {
    return 0;
}

EXPORT_API object gc_mark() {
    return 0;
}



/******************************************************************************
    对象构造 API
******************************************************************************/
EXPORT_API object alloc_i64() {
    object ret = base_alloc(object_size(i64));
    memset(ret, 0, object_size(i64));
    return ret;
}

EXPORT_API object mk_i64(int64_t v) {
    object ret = alloc_i64();
    ret->type = OBJ_I64;
    ret->value.i64 = v;
    return ret;
}




#endif // _BASE_SCHEME_GC_HEADER_