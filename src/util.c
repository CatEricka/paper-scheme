#include "paper-scheme/util.h"


/**
 * ��֤�� int64_t ����ʵ����������
 * @param x ����������ֵ
 * @param n ����λ��
 * @return
 */
int64_t i64_arithmetic_right_shift(int64_t x, size_t n) {
    static const int compile_use_arithmetic_right_shift = (-1 >> 1u) < 0;
    // ϣ������ʱ���Ż��������֧...
    if (compile_use_arithmetic_right_shift) {
        return x >> n;
    } else {
        if (x < 0) return (x >> n) | ~(~((size_t) 0u) >> n);
        else return x >> n;
    }
}