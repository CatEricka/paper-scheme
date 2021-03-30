#ifndef BASE_SCHEME_CONTEXT_H
#define BASE_SCHEME_CONTEXT_H
#pragma once


/**
 * context.h context.c
 * �����Ľṹ, ���� scheme ������������Ҫ����ṹΪ����
 * �������ݼ� test/test_cases/gc_test.h
 */


#include <paper-scheme/heap.h>


/**
                                �����Ľṹ����
******************************************************************************/

/**
 * ������ gc ǰ������ʱ����, �ƶ���������ȷ��������
 */
typedef struct gc_illusory_dream {
    object *illusory_object;
    struct gc_illusory_dream *next;
} *gc_saves_list_t;

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
    // debug ����
    int debug;

    // �Ƿ��� repl ģʽ
    int repl_mode;

    // ��
    heap_t heap;

    // TODO �������Ĵ���

    // ���� gc ʱ gc_collect() ��������, �����ڲ���
    int gc_collect_on;
    // GC! gc ʱ����ʱ����������
    GC gc_saves_list_t saves;
    // gc ���ջ
    struct gc_mark_stack_node_t mark_stack[MAX_MARK_STACK_DEEP];
    // ���ջ��
    gc_mark_stack_ptr mark_stack_top;

    // �������������
    FILE *context_stdin;
    FILE *context_stdout;
    FILE *context_stderr;

    // ȫ��������Ϣ����󳤶�
    size_t type_info_table_size;
    // GC! ȫ��������Ϣ��
    GC struct object_runtime_type_info_t *global_type_table;
    // ȫ��������Ϣ��ǰ����
    size_t global_type_table_len;

    // TODO ȫ�ַ��ű�, �˴�ӦΪ������, gc ����ʱӦ�����⴦��
    // ��Ϊ����֮��û���õ������õĵط�, ��˲�����ʵ�������ù���

    // ����������Ϣ������, �����ڹ�����, ���� object ���;�������; .name �ֶ�Ϊ�ַ���ָ��
    struct object_runtime_type_info_t const *scheme_type_specs;
} *context_t;


/**
                                ����ʱ������Ϣ
******************************************************************************/

typedef object (*proc_1)(context_t context, object arg1);

typedef object (*proc_2)(context_t context, object arg1, object arg2);

typedef object (*proc_3)(context_t context, object arg1, object arg2, object arg3);

typedef object (*proc_4)(context_t context, object arg1, object arg2, object arg3, object arg4);

typedef object (*proc_n)(context_t context, size_t argument_length, object args[]);

/**
 * ����ʱ������Ϣ
 */
struct object_runtime_type_info_t {
    object name, getter, setter, to_string;
    object_type_tag tag;

    /** ���ڱ�Ƕ������õ��Ӷ���, ���ͱ�����object, ��������һ�� */
    size_t member_base;                  // �����Ա��ʼƫ����
    size_t member_eq_len_base;           // �Ƚ϶���ʱ��Ҫ�Ƚϵ� ��Ա��������
    size_t member_len_base;              // ��Ա��������
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
                              ������������
******************************************************************************/
/**
 * stdio_port finalizer
 * @param context
 * @param port
 * @return
 */
object stdio_finalizer(context_t context, object port);


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

// finalize
#define type_info_finalizer(t)                   type_info_field((t), finalizer)


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

#define context_get_object_finalize(context, obj) \
    type_info_finalizer(context_get_object_type((context), (obj)))

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
 * ��������ע��������Ϣ, ���ں�����չ
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
