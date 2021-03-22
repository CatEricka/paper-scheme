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


#endif // BASE_SCHEME_GC_H