#include <paper-scheme/feature.h>


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


/**
 * ������������İ�װ
 * Ϊ hash ֵ�����ṩ���޵������, [0, RAND_MAX]
 * @return uint32_t
 */
uint32_t rand_helper() {
    // todo Ҳ����Ҫ���õ����������
    // ���ǿ��ǵ�ʵ�����ַ����Ѿ������õ� hash �㷨,
    // �������͵� hash ֵ�����Ǻܳ���, ��˴˴��������
    // ����Ҳ�����Ǻ���Ҫ
    return (uint32_t) rand();
}