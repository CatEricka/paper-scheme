#ifndef BASE_SCHEME_CONTEXT_H
#define BASE_SCHEME_CONTEXT_H
#pragma once


/**
 * context.h context.c
 * �����Ľṹ, ���� scheme ������������Ҫ����ṹΪ����
 * �������ݼ� test/test_cases/gc_test.h
 */


#include <paper-scheme/feature.h>
#include <paper-scheme/object.h>
#include <paper-scheme/heap.h>


/**
                                �����Ľṹ����
******************************************************************************/

/**
 * ������ gc ǰ������ʱ����
 *
 */
typedef struct gc_illusory_dream {
    object *illusory_object;
    struct gc_illusory_dream *next;
} *gc_saves_list_t;

/**
 * ������ʱ����
 * @param var_name ��ʱ������
 * @param illusory_dream_name
 */
#define gc_var(var_name, illusory_dream_name) \
    object var_name = IMM_UNIT; \
    struct gc_illusory_dream illusory_dream_name = { NULL, NULL }

#define gc_preserve(ctx, var_name, illusory_dream_name) \
    do { \
        (illusory_dream_name).illusory_object = &(var_name); \
        (illusory_dream_name).next = (ctx)->saves; \
        (ctx)->saves = &(illusory_dream_name); \
    } while(0)

#define gc_release(ctx, this_root_illusory_dream_name)   ((ctx)->saves = (this_root_illusory_dream_name).next)

#define gc_var1(a) gc_var(a, gc_dream1)
#define gc_var2(a, b) gc_var1(a); gc_var(b, gc_dream2)
#define gc_var3(a, b, c) gc_var2(a, b); gc_var(c, gc_dream3)
#define gc_var4(a, b, c, d) gc_var3(a, b, c); gc_var(d, gc_dream4)
#define gc_var5(a, b, c, d, e) gc_var4(a, b, c, d); gc_var(e, gc_dream5)
#define gc_var6(a, b, c, d, e, f) gc_var5(a, b, c, d, e); gc_var(f, gc_dream6)
#define gc_var7(a, b, c, d, e, f, g) gc_var6(a, b, c, d, e, f); gc_var(g, gc_dream7)

#define gc_preserve1(ctx, a) gc_preserve(ctx, a, gc_dream1)
#define gc_preserve2(ctx, a, b) gc_preserve1(ctx, a); gc_preserve(ctx, b, gc_dream2)
#define gc_preserve3(ctx, a, b, c) gc_preserve2(ctx, a, b); gc_preserve(ctx, c, gc_dream3)
#define gc_preserve4(ctx, a, b, c, d) gc_preserve3(ctx, a, b, c); gc_preserve(ctx, d, gc_dream4)
#define gc_preserve5(ctx, a, b, c, d, e)  gc_preserve4(ctx, a, b, c, d); gc_preserve(ctx, e, gc_dream5)
#define gc_preserve6(ctx, a, b, c, d, e, f) gc_preserve5(ctx, a, b, c, d, e); gc_preserve(ctx, f, gc_dream6)
#define gc_preserve7(ctx, a, b, c, d, e, f, g) gc_preserve6(ctx, a, b, c, d, e, f); gc_preserve(ctx, g, gc_dream7)

#define gc_release_all(ctx) gc_release((ctx), gc_dream1)

/**
 * gc ʱ����ͼ�ṹ�õı��ջ
 */
typedef struct gc_mark_stack_node_t {
    object *start, *end;
    struct gc_mark_stack_node_t *prev;
} *gc_mark_stack_ptr;

/**
 * ������Ϣ
 */
struct object_runtime_type_info_t;
typedef struct object_runtime_type_info_t *object_type_info_ptr;

/**
 * vm ������
 */
struct scheme_context_t;
typedef struct scheme_context_t {
    // ��
    heap_t heap;

    // ���� gc ʱ gc_collect() ��������, �����ڲ���
    int gc_collect_on;
    // GC! gc ʱ����ʱ����������
    GC gc_saves_list_t saves;
    // gc ���ջ
    struct gc_mark_stack_node_t mark_stack[MAX_MARK_STACK_DEEP];
    // ���ջ��
    gc_mark_stack_ptr mark_stack_top;

    FILE *context_stdin;
    FILE *context_stdout;
    FILE *context_stderr;

    // ȫ��������Ϣ����󳤶�
    size_t type_info_table_size;
    // GC! ȫ��������Ϣ��
    GC struct object_runtime_type_info_t *global_type_table;
    // ȫ��������Ϣ����ǰ����
    size_t global_type_table_len;

    // ����������Ϣ������, �����ڹ�����, ���� object ���;�������; .name �ֶ�Ϊ�ַ���ָ��
    struct object_runtime_type_info_t const *scheme_type_specs;
} *context_t;


/**
                                ����ʱ������Ϣ
******************************************************************************/

typedef void (*proc_1)(context_t context, object arg1);

typedef void (*proc_2)(context_t context, object arg1, object arg2);

typedef void (*proc_3)(context_t context, object arg1, object arg2, object arg3);

typedef void (*proc_4)(context_t context, object arg1, object arg2, object arg3, object arg4);

typedef void (*proc_n)(context_t context, size_t argument_length, object args[]);

/**
 * ����ʱ������Ϣ
 */
struct object_runtime_type_info_t {
    object name, getter, setter, to_string;
    object_type_tag tag;

    /** ���ڱ�Ƕ������õ��Ӷ���, ���ͱ�����object, ��������һ�� */
    size_t member_base;                  // �����Ա��ʼƫ����
    size_t member_eq_len_base;           // �Ƚ϶���ʱ��Ҫ�Ƚϵ� ��Ա������С
    size_t member_len_base;              // ��Ա������С
    // �ɱ䳤�����ݽṹ���õ��������, �� vector, stack
    size_t member_meta_len_offset;       // ��Ա����ǿɱ䳤��, �� ������Ա���������� �ڽṹ���ƫ����, ������ 0
    size_t member_meta_len_scale;        // ��Ա����������ϵ��, һ��Ϊ 1 �� 0, ȡ���ڳ�Ա�����Ƿ�ɱ�
    /** ���ڼ�������С, �����ɱ䳤�ȶ������Ϣ */
    size_t size_base;                   // ���������С
    size_t size_meta_size_offset;       // �������鳤������ƫ����
    size_t size_meta_size_scale;        // ��������Ԫ�ش�С

    proc_1 finalizer;                   // finalizer
};


/**
                           ����ʱ������Ϣ���㸨����
******************************************************************************/
// object_type_info_ptr �ṹ���ֶη��ʺ�
#define type_info_field(_t, _f) ((_t)->_f)

// �����Ա��ʼƫ����
#define type_info_member_base(_t)               type_info_field((_t), member_base)

// �Ƚ϶���ʱ��Ҫ�Ƚϵ� ��Ա������С
#define type_info_member_eq_len_base(_t)        type_info_field((_t), member_eq_len_base)
// ��Ա������С
#define type_info_member_len_base(_t)           type_info_field((_t), member_len_base)
// ��Ա����ǿɱ䳤��, �� ������Ա���������� �ڽṹ���ƫ����, ������ 0
#define type_info_member_meta_len_offset(_t)    type_info_field((_t), member_meta_len_offset)
// ��Ա����������ϵ��, һ��Ϊ 1 �� 0, ȡ���ڳ�Ա�����Ƿ�ɱ�
#define type_info_member_meta_len_scale(_t)     type_info_field((_t), member_meta_len_scale)

// ���������С
#define type_info_size_base(_t)                 type_info_field((_t), size_base)
// �������鳤������ƫ����
#define type_info_size_meta_size_offset(_t)     type_info_field((_t), size_meta_size_offset)
// ��������Ԫ�ش�С
#define type_info_size_meta_size_scale(_t)      type_info_field((_t), size_meta_size_scale)


/**
 * ���ݶ���� size_t �����ֶ�ƫ������ȡ size_t ֵ
 * @param _obj object ����
 * @param _offset size_t �ֶ�ƫ����
 */
#define type_info_get_size_by_size_field_offset(_obj, _offset) \
    (((size_t*)((char*)(_obj) + (_offset)))[0])

/**
 * ���������л�ȡ�����������Ϣ
 * @param _context context_t
 * @param _obj object
 * @return object_type_info_ptr
 */
#define context_get_object_type(_context, _obj) \
    (&((_context)->global_type_table[(_obj)->type]))

/**
 * ���ݶ����������Ϣ������������ʱ��С, ��Ҫ������ context_t �ṹ, ��Ҫ��ȷ��ʼ��object->padding_size
 * @param _type object_type_info_ptr
 * @param _obj object
 */
#define object_type_info_sizeof(_type, _obj) \
  (type_info_get_size_by_size_field_offset((_obj), type_info_size_meta_size_offset((_type))) \
   * type_info_size_meta_size_scale((_type)) \
   + type_info_size_base((_type)) \
   + (_obj)->padding_size)

/**
 * ��������, ʹ�� context_t->global_type_table ��Ϣ��ö����С
 * @param _context context_t
 * @param _obj object
*/
#define context_object_sizeof(_context, _obj) \
    object_type_info_sizeof(context_get_object_type((_context), (_obj)), (_obj))

/**
 * ���ݶ����������Ϣȡ�ö���ĵ�һ����Ա�ֶ�����, ���� gc ͼ������ջ
 * <p>ע��, ����ֵ�� object *, ������ object</p>
 * @param _type object_type_info_ptr
 * @param _obj object
 * @param object *
 */
#define type_info_get_object_ptr_of_first_member(_type, _obj) \
    ((object *)((char*)(_obj) + type_info_member_base((_type))))

/**
 * ���ݶ����������Ϣȡ�� ȫ�� �����Ա�ֶθ���
 * @param _type object_type_info_ptr
 * @param _obj object
 * @return size_t ��Ա�ֶθ���
*/
#define object_type_info_member_slots_of(_type, _obj)                             \
  (type_info_get_size_by_size_field_offset((_obj), type_info_member_meta_len_offset((_type))) \
   * type_info_member_meta_len_scale((_type)) \
   + type_info_member_len_base((_type)))

/**
 * ���ݶ����������Ϣȡ�� compare ʱ��Ҫ�ȽϵĶ����Ա�ֶθ���, һ�� <= object_type_info_member_slots_of
 * @param _type object_type_info_ptr
 * @param _obj object
 * @return size_t ��Ա�ֶθ���
*/
#define object_type_info_member_eq_slots_of(_type, _obj)                          \
  (type_info_get_size_by_size_field_offset((_obj), type_info_member_meta_len_offset((_type))) \
   * type_info_member_meta_len_scale((_type)) \
   + type_info_member_eq_len_base((_type)))

/**
                               �����������Ľṹ
******************************************************************************/

/**
 * ����������, �������������������Ľṹ���е�
 * @return NULL: �ڴ����ʧ��
 */
EXPORT_API OUT NULLABLE context_t
context_make(IN size_t heap_init_size, IN size_t heap_growth_scale, IN size_t heap_max_size);

/**
 * ��������ע��������Ϣ TODO context_register_type ��Ҫ����
 * @param context ������
 * @param type_tag enum object_type_enum, �� object �ṹ����ƥ��, ��� 255
 * @param type_info ������Ϣ, ��Ҫ�ֶ������ڴ�
 * @return 0: ע��ʧ��
 */
EXPORT_API int
context_register_type(REF NOTNULL context_t context,
                      IN object_type_tag type_tag,
                      IN NOTNULL object_type_info_ptr type_info);

/**
 * �ͷ������Ľṹ
 * @param context
 */
EXPORT_API void context_destroy(IN NOTNULL context_t context);


#endif //BASE_SCHEME_CONTEXT_H