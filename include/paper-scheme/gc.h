#ifndef BASE_SCHEME_GC_H
#define BASE_SCHEME_GC_H
#pragma once


/**
 * gc.h gc.c
 * �����ѽṹ����, �Լ��ڶѽṹ�з���ͻ��ն���� API
 * �������ݼ� test/test_cases/gc_test.h
 */


#include <paper-scheme/feature.h>
#include <paper-scheme/object.h>
#include <paper-scheme/heap.h>
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
 * ������ʱ����
 * @param var ��ʱ����
 * @param illusory_dream_name
 */
#define gc_var(__ctx, __var, __illusory_dream_name) \
    struct gc_illusory_dream __illusory_dream_name = { &(__var), (__ctx)->saves }; \
    do { \
        (__ctx)->saves = &(__illusory_dream_name); \
    } while(0)

#define gc_release(__ctx, this_root_illusory_dream_name)   ((__ctx)->saves = (this_root_illusory_dream_name).next)

// �������뱣����, ����������뱣��������ᵼ�� gc �쳣����
// ���������ʼ��
#define gc_var1(ctx, a)                         gc_var(ctx, a, __gc_dream1__)
// ���������ʼ��
#define gc_var2(ctx, a, b)                      gc_var1(ctx, a);                        gc_var(ctx, b, __gc_dream2__)
// ���������ʼ��
#define gc_var3(ctx, a, b, c)                   gc_var2(ctx, a, b);                     gc_var(ctx, c, __gc_dream3__)
// ���������ʼ��
#define gc_var4(ctx, a, b, c, d)                gc_var3(ctx, a, b, c);                  gc_var(ctx, d, __gc_dream4__)
// ���������ʼ��
#define gc_var5(ctx, a, b, c, d, e)             gc_var4(ctx, a, b, c, d);               gc_var(ctx, e, __gc_dream5__)
// ���������ʼ��
#define gc_var6(ctx, a, b, c, d, e, f)          gc_var5(ctx, a, b, c, d, e);            gc_var(ctx, f, __gc_dream6__)
// ���������ʼ��
#define gc_var7(ctx, a, b, c, d, e, f, g)       gc_var6(ctx, a, b, c, d, e, f);         gc_var(ctx, g, __gc_dream7__)
// ���������ʼ��
#define gc_var8(ctx, a, b, c, d, e, f, g, h)    gc_var7(ctx, a, b, c, d, e, f, g);      gc_var(ctx, h, __gc_dream8__)
// ���������ʼ��
#define gc_var9(ctx, a, b, c, d, e, f, g, h, i) gc_var8(ctx, a, b, c, d, e, f, g, h);   gc_var(ctx, i, __gc_dream9__)

// �ͷŵ�ǰջ�ı�����
#define gc_release_all(ctx) gc_release((ctx), __gc_dream1__)


#endif // BASE_SCHEME_GC_H