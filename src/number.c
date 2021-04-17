#include <paper-scheme/number.h>

EXPORT_API GC object number_add(context_t context, object a, object b) {
    assert(context != NULL);
    assert(is_i64(a) || is_doublenum(a));
    assert(is_i64(b) || is_doublenum(b));
    gc_param2(context, a, b);
    gc_var1(context, ret);

    if (is_i64(a) && is_i64(b)) {
        int64_t v = i64_getvalue(a) + i64_getvalue(b);
        ret = i64_make_op(context, v);
    } else {
        double v;
        if (is_i64(a)) {
            v = (double) i64_getvalue(a);
        } else {
            v = doublenum_getvalue(a);
        }

        if (is_i64(b)) {
            v += (double) i64_getvalue(b);
        } else {
            v += doublenum_getvalue(b);
        }
        ret = doublenum_make_op(context, v);
    }

    gc_release_param(context);
    return ret;
}

EXPORT_API GC object number_mul(context_t context, object a, object b) {
    assert(context != NULL);
    assert(is_i64(a) || is_doublenum(a));
    assert(is_i64(b) || is_doublenum(b));
    gc_param2(context, a, b);
    gc_var1(context, ret);

    if (is_i64(a) && is_i64(b)) {
        int64_t v = i64_getvalue(a) * i64_getvalue(b);
        ret = i64_make_op(context, v);
    } else {
        double v;
        if (is_i64(a)) {
            v = (double) i64_getvalue(a);
        } else {
            v = doublenum_getvalue(a);
        }

        if (is_i64(b)) {
            v *= (double) i64_getvalue(b);
        } else {
            v *= doublenum_getvalue(b);
        }
        ret = doublenum_make_op(context, v);
    }

    gc_release_param(context);
    return ret;
}

EXPORT_API GC object number_div(context_t context, object a, object b) {
    assert(context != NULL);
    assert(is_i64(a) || is_doublenum(a));
    assert(is_i64(b) || is_doublenum(b));
    gc_param2(context, a, b);
    gc_var1(context, ret);

    if (is_i64(a) && is_i64(b)) {
        int64_t L = i64_getvalue(a);
        int64_t R = i64_getvalue(b);
        if (L % R == 0) {
            int64_t v = L / R;
            ret = i64_make_op(context, v);
            gc_release_param(context);
            return ret;
        }
    }

    double v;
    if (is_i64(a)) {
        v = (double) i64_getvalue(a);
    } else {
        v = doublenum_getvalue(a);
    }

    if (is_i64(b)) {
        v /= (double) i64_getvalue(b);
    } else {
        v /= doublenum_getvalue(b);
    }
    ret = doublenum_make_op(context, v);
    gc_release_param(context);
    return ret;
}

EXPORT_API GC object number_int_div(context_t context, object a, object b) {
    assert(context != NULL);
    assert(is_i64(a) || is_doublenum(a));
    assert(is_i64(b) || is_doublenum(b));
    gc_param2(context, a, b);
    gc_var1(context, ret);

    if (is_i64(a) && is_i64(b)) {
        int64_t v = i64_getvalue(a) / i64_getvalue(b);
        ret = i64_make_op(context, v);
    } else {
        double v;
        if (is_i64(a)) {
            v = (double) i64_getvalue(a);
        } else {
            v = doublenum_getvalue(a);
        }

        if (is_i64(b)) {
            v /= (double) i64_getvalue(b);
        } else {
            v /= doublenum_getvalue(b);
        }
        ret = doublenum_make_op(context, v);
    }

    gc_release_param(context);
    return ret;
}

EXPORT_API GC object number_sub(context_t context, object a, object b) {
    assert(context != NULL);
    assert(is_i64(a) || is_doublenum(a));
    assert(is_i64(b) || is_doublenum(b));
    gc_param2(context, a, b);
    gc_var1(context, ret);

    if (is_i64(a) && is_i64(b)) {
        int64_t v = i64_getvalue(a) - i64_getvalue(b);
        ret = i64_make_op(context, v);
    } else {
        double v;
        if (is_i64(a)) {
            v = (double) i64_getvalue(a);
        } else {
            v = doublenum_getvalue(a);
        }

        if (is_i64(b)) {
            v -= (double) i64_getvalue(b);
        } else {
            v -= doublenum_getvalue(b);
        }
        ret = doublenum_make_op(context, v);
    }

    gc_release_param(context);
    return ret;
}

EXPORT_API GC object number_rem(context_t context, object a, object b) {
    assert(context != NULL);
    assert(is_i64(a) || is_doublenum(a));
    assert(is_i64(b) || is_doublenum(b));
    gc_param2(context, a, b);
    gc_var1(context, ret);

    int64_t e1, e2, res;

    if (is_i64(a)) {
        e1 = i64_getvalue(a);
    } else {
        e1 = (int64_t) doublenum_getvalue(a);
    }

    if (is_i64(b)) {
        e2 = i64_getvalue(b);
    } else {
        e2 = (int64_t) doublenum_getvalue(b);
    }

    res = e1 % e2;
    // 模必须和第二个操作数符号相同
    if (res > 0) {
        if (e1 < 0) {
            res -= llabs(e2);
        }
    } else if (res < 0) {
        if (e1 > 0) {
            res += llabs(e2);
        }
    }

    ret = i64_make_op(context, res);
    gc_release_param(context);
    return ret;
}

EXPORT_API GC object number_mod(context_t context, object a, object b) {
    assert(context != NULL);
    assert(is_i64(a) || is_doublenum(a));
    assert(is_i64(b) || is_doublenum(b));
    gc_param2(context, a, b);
    gc_var1(context, ret);

    int64_t e1, e2, res;

    if (is_i64(a)) {
        e1 = i64_getvalue(a);
    } else {
        e1 = (int64_t) doublenum_getvalue(a);
    }

    if (is_i64(b)) {
        e2 = i64_getvalue(b);
    } else {
        e2 = (int64_t) doublenum_getvalue(b);
    }

    res = e1 % e2;
    // 模必须和第二个操作数符号相同
    if (res * e2 < 0) {
        res += e2;
    }

    ret = i64_make_op(context, res);
    gc_release_param(context);
    return ret;
}

EXPORT_API int number_eq(object a, object b) {
    if (is_i64(a) && is_i64(b)) {
        return i64_getvalue(a) == i64_getvalue(b);
    } else {
        double e1, e2;
        if (is_i64(a)) {
            e1 = (double) i64_getvalue(a);
        } else {
            e1 = doublenum_getvalue(a);
        }

        if (is_i64(b)) {
            e2 = (double) i64_getvalue(b);
        } else {
            e2 = doublenum_getvalue(b);
        }
        return e1 == e2;
    }
}

EXPORT_API int number_gt(object a, object b) {
    if (is_i64(a) && is_i64(b)) {
        return i64_getvalue(a) > i64_getvalue(b);
    } else {
        double e1, e2;
        if (is_i64(a)) {
            e1 = (double) i64_getvalue(a);
        } else {
            e1 = doublenum_getvalue(a);
        }

        if (is_i64(b)) {
            e2 = (double) i64_getvalue(b);
        } else {
            e2 = doublenum_getvalue(b);
        }
        return e1 > e2;
    }
}

EXPORT_API int number_ge(object a, object b) {
    if (is_i64(a) && is_i64(b)) {
        return i64_getvalue(a) >= i64_getvalue(b);
    } else {
        double e1, e2;
        if (is_i64(a)) {
            e1 = (double) i64_getvalue(a);
        } else {
            e1 = doublenum_getvalue(a);
        }

        if (is_i64(b)) {
            e2 = (double) i64_getvalue(b);
        } else {
            e2 = doublenum_getvalue(b);
        }
        return e1 >= e2;
    }
}

EXPORT_API int number_lt(object a, object b) {
    if (is_i64(a) && is_i64(b)) {
        return i64_getvalue(a) < i64_getvalue(b);
    } else {
        double e1, e2;
        if (is_i64(a)) {
            e1 = (double) i64_getvalue(a);
        } else {
            e1 = doublenum_getvalue(a);
        }

        if (is_i64(b)) {
            e2 = (double) i64_getvalue(b);
        } else {
            e2 = doublenum_getvalue(b);
        }
        return e1 < e2;
    }
}

EXPORT_API int number_le(object a, object b) {
    if (is_i64(a) && is_i64(b)) {
        return i64_getvalue(a) <= i64_getvalue(b);
    } else {
        double e1, e2;
        if (is_i64(a)) {
            e1 = (double) i64_getvalue(a);
        } else {
            e1 = doublenum_getvalue(a);
        }

        if (is_i64(b)) {
            e2 = (double) i64_getvalue(b);
        } else {
            e2 = doublenum_getvalue(b);
        }
        return e1 <= e2;
    }
}

EXPORT_API double round_per_R5RS(double x) {
    double fl = floor(x);
    double ce = ceil(x);
    double dfl = x - fl;
    double dce = ce - x;
    if (dfl > dce) {
        return ce;
    } else if (dfl < dce) {
        return fl;
    } else {
        if (fmod(fl, 2.0) == 0.0) {       /* I imagine this holds */
            return fl;
        } else {
            return ce;
        }
    }
}