#include <paper-scheme/number.h>

EXPORT_API GC object number_add(context_t context, object a, object b) {
    assert(context != NULL);
    assert(is_i64(a) || is_doublenum(a));
    assert(is_i64(b) || is_doublenum(b));
    gc_param2(context, a, b);
    gc_var1(context, ret);

    gc_release_param(context);
    return ret;
}

EXPORT_API GC object number_mul(context_t context, object a, object b) {
    assert(context != NULL);
    assert(is_i64(a) || is_doublenum(a));
    assert(is_i64(b) || is_doublenum(b));
    gc_param2(context, a, b);
    gc_var1(context, ret);

    gc_release_param(context);
    return ret;
}

EXPORT_API GC object number_div(context_t context, object a, object b) {
    assert(context != NULL);
    assert(is_i64(a) || is_doublenum(a));
    assert(is_i64(b) || is_doublenum(b));
    gc_param2(context, a, b);
    gc_var1(context, ret);

    gc_release_param(context);
    return ret;
}

EXPORT_API GC object number_int_div(context_t context, object a, object b) {
    assert(context != NULL);
    assert(is_i64(a) || is_doublenum(a));
    assert(is_i64(b) || is_doublenum(b));
    gc_param2(context, a, b);
    gc_var1(context, ret);

    gc_release_param(context);
    return ret;
}

EXPORT_API GC object number_sub(context_t context, object a, object b) {
    assert(context != NULL);
    assert(is_i64(a) || is_doublenum(a));
    assert(is_i64(b) || is_doublenum(b));
    gc_param2(context, a, b);
    gc_var1(context, ret);

    gc_release_param(context);
    return ret;
}

EXPORT_API GC object number_rem(context_t context, object a, object b) {
    assert(context != NULL);
    assert(is_i64(a) || is_doublenum(a));
    assert(is_i64(b) || is_doublenum(b));
    gc_param2(context, a, b);
    gc_var1(context, ret);

    gc_release_param(context);
    return ret;
}

EXPORT_API GC object number_mod(context_t context, object a, object b) {
    assert(context != NULL);
    assert(is_i64(a) || is_doublenum(a));
    assert(is_i64(b) || is_doublenum(b));
    gc_param2(context, a, b);
    gc_var1(context, ret);

    gc_release_param(context);
    return ret;
}

EXPORT_API int number_eq(object a, object b) {

}

EXPORT_API int number_gt(object a, object b) {

}

EXPORT_API int number_ge(object a, object b) {

}

EXPORT_API int number_lt(object a, object b) {

}

EXPORT_API int number_le(object a, object b) {

}

EXPORT_API double round_per_R5RS(double x) {

}