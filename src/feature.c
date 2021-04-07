#include <paper-scheme/feature.h>


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


/**
 * 随机数生成器的包装
 * 为 hash 值生成提供有限的随机性, [0, RAND_MAX]
 * @return uint32_t
 */
uint32_t rand_helper() {
    // todo 也许需要更好的随机数生成
    // 但是考虑到实际上字符串已经有良好的 hash 算法,
    // 其他类型的 hash 值并不是很常用, 因此此处的随机数
    // 质量也并不是很重要
    return (uint32_t) rand();
}