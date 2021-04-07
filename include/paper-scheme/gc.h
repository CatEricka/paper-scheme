#ifndef BASE_SCHEME_GC_H
#define BASE_SCHEME_GC_H
#pragma once


/**
 * gc.h gc.c
 * �����ѽṹ����, �Լ��ڶѽṹ�з���ͻ��ն���� API
 * �������ݼ� test/test_cases/gc_test.h
 */


#include <paper-scheme/context.h>


/**
                               �������� API
******************************************************************************/

#define gc_collect_disable(context) ((context)->gc_collect_on = 0)
#define gc_collect_enable(context) ((context)->gc_collect_on = 1)

/**
 * ����������������
 * <p>todo ע��! ���� c ����ջ ��ʹ�õı�����Ķ��󶼱��뱣�浽������, �������Ϊ�����ƶ��ƻ�����</p>
 * <p>��� context.h: macro gc_var()</p>
 * <p>���˵���, ������������������ c ����ջ�����������</p>
 * <p>ֻҪ��ȷʹ�� gc_var �� gc_preserve ���ܱ�֤���õ���ȷ����</p>
 * <p>ע�����лᴥ�� gc �ĺ��� (���� GC ��ǵĺ���) �ڵ� object ������Ӧ�ü��뱣����</p>
 * <p>ע����Щ������Ҫ�ر� gc ���ܱ�֤������������, �μ� macro gc_collect_disable()</p>
 * @param context
 * @return <li>IMM_TRUE: ���гɹ�</li><li>IMM_FALSE: ����ʧ��</li>
 */
EXPORT_API CHECKED GC object gc_collect(REF NOTNULL context_t context);

/**
 * ���ԴӶ��з����ڴ�, ���ᴥ�� gc, ʧ�ܷ��� NULL
 * <p>��Ҫֱ��ʹ��</p>
 * @param heap �ѽṹ
 * @param size Ҫ����Ķ����С
 * @return <li>NULL: �Ҳ����㹻��Ŀ��пؼ�</li>
 */
EXPORT_API OUT CHECKED GC object gc_try_alloc(REF NOTNULL context_t context, IN size_t size);

/**
 * �Ӷ��з����ڴ�
 * <p>��Ҫֱ��ʹ��</p>
 * <p>ע��, ���˵ײ����й��ڴ�ķֽ�����</p>
 * <p>!�˺���ʧ�ܻ�ֱ�ӽ�������</p>
 * @param heap �ѽṹ
 * @param size Ҫ����Ķ����С
 * @return
 * <li>exit(EXIT_FAILURE_OUT_OF_MEMORY): �ﵽ���Ѵ�С</li>
 * <li>exit(EXIT_FAILURE_MALLOC_FAILED): δ�ﵽ���Ѵ�С, ����ϵͳ�ڴ治��</li>
 */
EXPORT_API OUT GC object gc_alloc(REF NOTNULL context_t context, IN size_t size);


/**
                               ջ�� object ����
******************************************************************************/

/**
 * һ��ʹ�÷���:
 *      gc_param()
 *      gc_var()
 *          ���ܴ��� gc �Ĵ���
 *      gc_release_var_param()
 * ��
 *      gc_param()
 *          ���ܴ��� gc �Ĵ���
 *      gc_release_param()
 * ��
 *      gc_var()
 *          ���ܴ��� gc �Ĵ���
 *      gc_release_var()
 */

/**
 * ջ�ϱ���
 * <p>�������Զ�������������ʼ��Ϊ IMM_UNIT</p>
 * <p>��� gc.h: ջ�� object ����, һ��ʹ�÷���</p>
 * @param __var ����������
 * @param illusory_dream_name
 */
#define gc_var(__ctx, __var, __illusory_dream_name) \
    object __var = IMM_UNIT;\
    struct gc_illusory_dream __illusory_dream_name = { &(__var), (__ctx)->saves }; \
    do { \
        (__ctx)->saves = &(__illusory_dream_name); \
    } while(0)

/**
 * ջ�ϲ���
 * <p>��� gc.h: ջ�� object ����, һ��ʹ�÷���</p>
 * @param __param ����
 * @param illusory_dream_name
*/
#define gc_param(__ctx, __param, __illusory_dream_name) \
    struct gc_illusory_dream __illusory_dream_name = { &(__param), (__ctx)->saves }; \
    do { \
        (__ctx)->saves = &(__illusory_dream_name); \
    } while(0)

/**
 * �ͷ�ջ�� object ����
 * <p>��� gc.h: ջ�� object ����, һ��ʹ�÷���</p>
 * @param __ctx context_t
 */
#define gc_release_var(__ctx) \
    do { \
        ((__ctx)->saves = (__gc_var_dream1__).next); \
    } while(0)

/**
 * �ͷŲ��� object ����
 * <p>��� gc.h: ջ�� object ����, һ��ʹ�÷���</p>
 * @param __ctx context_t
 */
#define gc_release_param(__ctx) \
    do { \
        ((__ctx)->saves = (__gc_param_dream1__).next); \
    } while(0)

#define gc_release_var_param(ctx) gc_release_var(ctx); gc_release_param(ctx)

// �������뱣����, ����������뱣��������ᵼ�� gc �쳣����
// ע��, gc_var_n() �� gc_param_n() �� FILO ��ϵ, Ӧ���ϸ񱣳ֵ��ù�ϵ˳��:
//      gc_var_n(); gc_param_n();  / code /  gc_release_param(); gc_release_var();
// ��
//      gc_param_n(); gc_var_n();  / code /  gc_release_var(); gc_release_param();
#define gc_var1(ctx, a)                         gc_var(ctx, a, __gc_var_dream1__)
#define gc_var2(ctx, a, b)                      gc_var1(ctx, a);                        gc_var(ctx, b, __gc_var_dream2__)
#define gc_var3(ctx, a, b, c)                   gc_var2(ctx, a, b);                     gc_var(ctx, c, __gc_var_dream3__)
#define gc_var4(ctx, a, b, c, d)                gc_var3(ctx, a, b, c);                  gc_var(ctx, d, __gc_var_dream4__)
#define gc_var5(ctx, a, b, c, d, e)             gc_var4(ctx, a, b, c, d);               gc_var(ctx, e, __gc_var_dream5__)
#define gc_var6(ctx, a, b, c, d, e, f)          gc_var5(ctx, a, b, c, d, e);            gc_var(ctx, f, __gc_var_dream6__)
#define gc_var7(ctx, a, b, c, d, e, f, g)       gc_var6(ctx, a, b, c, d, e, f);         gc_var(ctx, g, __gc_var_dream7__)
#define gc_var8(ctx, a, b, c, d, e, f, g, h)    gc_var7(ctx, a, b, c, d, e, f, g);      gc_var(ctx, h, __gc_var_dream8__)
#define gc_var9(ctx, a, b, c, d, e, f, g, h, i) gc_var8(ctx, a, b, c, d, e, f, g, h);   gc_var(ctx, i, __gc_var_dream9__)

// �������뱣����, ����������뱣��������ᵼ�� gc �쳣����
#define gc_param1(ctx, a)                           gc_param(ctx, a, __gc_param_dream1__)
#define gc_param2(ctx, a, b)                        gc_param1(ctx, a);                      gc_param(ctx, b, __gc_param_dream2__)
#define gc_param3(ctx, a, b, c)                     gc_param2(ctx, a, b);                   gc_param(ctx, c, __gc_param_dream3__)
#define gc_param4(ctx, a, b, c, d)                  gc_param3(ctx, a, b, c);                gc_param(ctx, d, __gc_param_dream4__)
#define gc_param5(ctx, a, b, c, d, e)               gc_param4(ctx, a, b, c, d);             gc_param(ctx, e, __gc_param_dream5__)
#define gc_param6(ctx, a, b, c, d, e, f)            gc_param5(ctx, a, b, c, d, e);          gc_param(ctx, f, __gc_param_dream6__)
#define gc_param7(ctx, a, b, c, d, e, f, g)         gc_param6(ctx, a, b, c, d, e, f);       gc_param(ctx, g, __gc_param_dream7__)
#define gc_param8(ctx, a, b, c, d, e, f, g, h)      gc_param7(ctx, a, b, c, d, e, f, g);    gc_param(ctx, h, __gc_param_dream8__)
#define gc_param9(ctx, a, b, c, d, e, f, g, h, i)   gc_param8(ctx, a, b, c, d, e, f, g, h); gc_param(ctx, i, __gc_param_dream9__)


/**
                               gc ���Զ���
******************************************************************************/
/**
 * ���� context->gc_happened λ
 */
#define gc_set_no_gc_assert_flag(context) ((context)->gc_happened = 0)
/**
 * ����: gc ��δ����
 */
#define gc_assert_no_gc(context) (assert((context)->gc_happened == 0))

#endif // BASE_SCHEME_GC_H