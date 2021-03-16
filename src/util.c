#include "paper-scheme/util.h"


/**
 * 保证对 int64_t 类型实现算数右移
 * @param x 被算数右移值
 * @param n 右移位数
 * @return
 */
int64_t i64_arithmetic_right_shift(int64_t x, size_t n) {
    static const int compile_use_arithmetic_right_shift = (-1 >> 1u) < 0;
    // 希望编译时会优化掉这个分支...
    if (compile_use_arithmetic_right_shift) {
        return x >> n;
    } else {
        if (x < 0) return (x >> n) | ~(~((size_t) 0u) >> n);
        else return x >> n;
    }
}