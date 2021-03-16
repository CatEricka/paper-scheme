#ifndef BASE_SCHEME_CONTEXT_H
#define BASE_SCHEME_CONTEXT_H
#pragma once


/**
 * context.h context.c
 * �����Ľṹ, ���� scheme ������������Ҫ����ṹΪ����
 * �������ݼ� test/test_cases/gc_test.h
 */


#include <paper-scheme/util.h>
#include <paper-scheme/object.h>
#include <paper-scheme/heap.h>


/**
                                �����Ľṹ����
******************************************************************************/
/**
 * ������Ϣ
 */
struct object_runtime_type_info_t;
typedef struct object_runtime_type_info_t *object_type_info;
/**
 * vm ������
 */
struct scheme_context_t;
typedef struct scheme_context_t {
    // ��
    heap_t heap;

    FILE *port_stdin;
    FILE *port_stdout;
    FILE *port_stderr;

    // ȫ��������Ϣ��
    // TODO ���ȫ�ֱ���
    size_t type_info_len;
    size_t type_info_table_size;
    object_type_info *global_type_table;
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
    // TODO ��д����
    object name, getter, setter, to_string;
    object_type_tag tag;

    size_t member_base;                  // �����Ա��ʼƫ����
    size_t member_eq_len_base;           // �Ƚ϶���ʱ��Ҫ�Ƚϵ� ��Ա������С
    size_t member_len_base;              // ��Ա������С

    // �ɱ䳤�����ݽṹ���õ��������
    size_t member_meta_len_offset;       // ��Ա����ǿɱ䳤��, �� ������Ա���������� �ڽṹ���ƫ����;
    size_t member_meta_len_scale;        // �ɱ䳤�ȳ�Ա ��ÿ�����ԵĴ�С

    size_t size_base;                   // ���������С
    size_t size_meta_size_offset;       // �������鳤������ƫ����
    size_t size_meta_size_scale;        // ��������Ԫ�ش�С
    proc_1 finalizer;                   // finalizer
};


/**
                           ����ʱ������Ϣ���㸨����
******************************************************************************/
// TODO ����ʱ������Ϣ���㸨���� ��д����
#define type_info_field(_t, _f) ((_t)->_f)
#define type_info_member_base(_t)               type_info_field((_t), member_base)

#define type_info_member_eq_len_base(_t)        type_info_field((_t), member_eq_len_base)
#define type_info_member_len_base(_t)           type_info_field((_t), member_len_base)
#define type_info_member_meta_len_offset(_t)    type_info_field((_t), member_meta_len_offset)
#define type_info_member_meta_len_scale(_t)     type_info_field((_t), member_meta_len_scale)

#define type_info_size_base(_t)                 type_info_field((_t), size_base)
#define type_info_size_meta_size_offset(_t)     type_info_field((_t), size_meta_size_offset)
#define type_info_size_meta_size_scale(_t)      type_info_field((_t), size_meta_size_scale)

#define type_info_get_object_of_first_member(_type, _obj) \
    (((object)((char*)(_obj) + type_info_member_base((_type))))[0])
#define type_info_get_size_by_size_field_offset(_obj, _offset) \
    (((size_t*)((char*)(_obj) + (_offset)))[0])

#define object_type_info_sizeof(_type, _obj) \
  (type_info_get_size_by_size_field_offset((_obj), type_info_size_meta_size_offset((_type))) \
   * type_info_size_meta_size_scale((_type)) \
   + type_info_size_base((_type)))

#define object_type_info_member_slots_of(_type, _obj)                             \
  (type_info_get_size_by_size_field_offset((_obj), type_info_member_meta_len_offset((_type))) \
   * type_info_member_meta_len_scale((_type)) \
   + type_info_member_len_base((_type)))

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
 * ��������ע��������Ϣ
 * @param context ������
 * @param type_tag enum object_type_enum, �� object �ṹ����ƥ��, ��� 255
 * @param type_info ������Ϣ, ��Ҫ�ֶ������ڴ�
 * @return 0: ע��ʧ��
 */
EXPORT_API int
context_register_type(REF NOTNULL context_t context,
                      IN object_type_tag type_tag,
                      IN NOTNULL object_type_info type_info);

/**
 * �ͷ������Ľṹ
 * @param context
 */
EXPORT_API void context_destroy(IN NOTNULL context_t context);


#endif //BASE_SCHEME_CONTEXT_H
