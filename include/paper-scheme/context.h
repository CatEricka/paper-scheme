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
 * ������
 */
struct scheme_context_t;
typedef struct scheme_context_t *context_t;

/**
 * op_exec_ ����ָ��
 */
typedef object (*op_exec_dispatch_func)(context_t r, enum opcode_e e);

/**
 * opcodes info
 */
typedef struct op_code_info_t {
    enum opcode_e op;
    op_exec_dispatch_func func;
    object name;
    size_t min_args_length;
    size_t max_args_length;
    char *args_type_table;
} op_code_info;

/**
 * �����Ľṹ����
 */
struct scheme_context_t {
    /**
     *                      ����ѡ��
     ****************************************************/
    // ������Ϣ ����
    int debug;
    // �Ƿ��� repl ģʽ
    int repl_mode;
    // �����������Ƿ��ʼ������
    int init_done;


    /**
     *                  op_exec �ַ���
     ****************************************************/
    op_code_info *dispatch_table;


    /**
     *                    �������Ĵ���
     ****************************************************/
    // TODO �������Ĵ���
    /**
     * �����Ĵ���, pair / IMM_UNIT
     */
    GC object args;
    /**
     * s-exp �Ĵ���, pair / IMM_UNIT
     */
    GC object code;
    /**
     * ��ǰ environment ָ��
     * <p>ֻ����˵�ȫ�� env �� hashmap, </p>
     * <p>env: (list ... env_frame2 env_frame1 global_environment '())</p>
     * <p>env_frame: (list ... slot3 slot2 slot1)</p>
     * <p>��Ϊ����ĳ����ܶ�, ���� hashmap �Ĵ��۹���</p>
     * <p>�μ� env_stack ����</p>
     */
    GC object current_env;
    /**
     * ջ��¼, ��ջ, Ϊ��ʵ��`����`
     */
    GC object scheme_stack;

    /**
     * ��ǰ������
     */
    int op_code;
    /**
     * ����ֵ, any type
     */
    GC object value;

    /**
     * (load "") ջ, (make-stack MAX_LOAD_FILE_DEEP)
     */
    GC object load_stack;


    /**
     *                   ȫ����Ϣ��
     ****************************************************/
    /**
     * ȫ�ַ��ű� ������ hashset
     * <p>(make-weak-hashset GLOBAL_SYMBOL_TABLE_INIT_SIZE)</p>
     * <p>symbol</p>
     */
    GC object global_symbol_table;
    /**
     * ȫ�� environment, (make-hashmap)
     * symbol->any
     */
    GC object global_environment;
    /**
     * ���� gensym ����Ψһ symbol
     */
    uint64_t gensym_count;

    /**
     * GC ȫ��������Ϣ��
     */
    GC struct object_runtime_type_info_t *global_type_table;
    // ȫ��������Ϣ����󳤶�
    size_t type_info_table_size;
    // ȫ��������Ϣ��ǰ����
    size_t global_type_table_len;


    /**
     *                      �ѽṹ
     ****************************************************/
    // ��
    heap_t heap;
    // ���� gc ʱ gc_collect() ��������, �����ڲ���
    int gc_collect_on;
    // GC! gc ʱ����ʱ����������
    GC gc_saves_list_t saves;
    // �������� GC ʱ, �ñ�־������Ϊ 1, ���ڵ����ڶ���
    int gc_happened;
    // ��������, ���� GC ʱɨ��������������
    GC WEAK_REF object weak_ref_chain;
    // gc ���ջ
    struct gc_mark_stack_node_t mark_stack[GC_MAX_MARK_STACK_DEEP];
    // ���ջ��
    gc_mark_stack_ptr mark_stack_top;

    // TODO �޸�Ϊ�ڲ� port ʵ��, ��������ʱ��; �������������
    GC object in_port;
    GC object out_port;
    GC object err_out_port;


    /**
     *                   ������Ϣ����
     ****************************************************/
    // ����������Ϣ������, �����ڹ�����, ���� object ���;�������; .name �ֶ�Ϊ�ַ���ָ��
    struct object_runtime_type_info_t const *_internal_scheme_type_specs;


    /**
     *                      C ����
     ****************************************************/
    int ret;
};


/**
                                ����ʱ������Ϣ
******************************************************************************/

typedef object (*proc_1)(context_t context, object arg1);

typedef object (*proc_2)(context_t context, object arg1, object arg2);

typedef object (*proc_3)(context_t context, object arg1, object arg2, object arg3);

typedef object (*proc_4)(context_t context, object arg1, object arg2, object arg3, object arg4);

typedef object (*proc_n)(context_t context, size_t argument_length, object args[]);

typedef uint32_t (*hash_code_fn)(context_t context, object arg);

typedef int (*equals_fn)(context_t context, object arg1, object arg2);

/**
 * ����ʱ������Ϣ
 */
struct object_runtime_type_info_t {
    object name, getter, setter, to_string;
    object_type_tag tag;

    /** ���ڱ�Ƕ������õ��Ӷ���, ���ͱ�����object, ��������һ�� */
    size_t member_base;                 // �����Ա��ʼƫ����
    size_t member_eq_len_base;          // �Ƚ϶���ʱ��Ҫ�Ƚϵ� ��Ա��������
    size_t member_len_base;             // ��Ա��������
    // �ɱ䳤�����ݽṹ���õ��������, �� vector, stack
    size_t member_meta_len_offset;      // ��Ա����ǿɱ䳤��, �� ������Ա���������� �ڽṹ���ƫ����, ������ 0
    size_t member_meta_len_scale;       // ��Ա����������ϵ��, һ��Ϊ 1 �� 0, ȡ���ڳ�Ա�����Ƿ�ɱ�
    /** ���ڼ�������С, �����ɱ䳤�ȶ������Ϣ */
    size_t size_base;                   // ���������С
    size_t size_meta_size_offset;       // �������鳤������ƫ����
    size_t size_meta_size_scale;        // ��������Ԫ�ش�С

    proc_1 finalizer;                   // finalizer
    hash_code_fn hash_code;             // hashֵ����      (context, object) -> uint32_t
    equals_fn equals;                   // �Ƚ��Ƿ����     (context, object) -> int 0/1
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
object stdio_finalizer(context_t _context, object port);


/**
                               hash ֵ�㷨
******************************************************************************/
// ��������
EXPORT_API uint32_t uint32_pair_hash(uint32_t x, uint32_t y);

EXPORT_API uint32_t uint64_hash(uint64_t value);

EXPORT_API uint32_t double_number_hash(double num);
// https://www.partow.net/programming/hashfunctions/#BKDRHashFunction
EXPORT_API uint32_t symbol_hash_helper(object symbol);

EXPORT_API uint32_t string_hash_helper(object str_obj);
/**
 * ���ݶ����ַ�Ͷ��������ö������ hash ֵ, ��֤ hash_code_fn �� equals ƥ��
 * @param ptr
 * @param enum
 * @return
 */
EXPORT_API uint32_t pointer_with_type_to_hash(object ptr, enum object_type_enum type_enum);

/**
                              ��ȡ hash ֵ
                       todo ������������д hash �㷨
******************************************************************************/
EXPORT_API uint32_t i64_hash_code(context_t context, object i64);
EXPORT_API uint32_t d64_hash_code(context_t context, object d64);
EXPORT_API uint32_t char_hash_code(context_t context, object imm_char);
EXPORT_API uint32_t boolean_hash_code(context_t context, object imm_bool);
EXPORT_API uint32_t unit_hash_code(context_t context, object unit_obj);
EXPORT_API uint32_t eof_hash_code(context_t context, object eof_obj);
EXPORT_API uint32_t pair_hash_code(context_t context, object pair);

EXPORT_API uint32_t bytes_hash_code(context_t context, object bytes);

EXPORT_API uint32_t string_buffer_hash_code(context_t context, object string_buffer);

EXPORT_API uint32_t vector_hash_code(context_t context, object vector);

EXPORT_API uint32_t stack_hash_code(context_t context, object stack);

EXPORT_API uint32_t string_port_hash_code(context_t context, object string_port);

EXPORT_API uint32_t stdio_port_hash_code(context_t context, object stdio_port);

EXPORT_API uint32_t hash_set_hash_code(context_t context, object hashset);

EXPORT_API uint32_t hash_map_hash_code(context_t context, object hashmap);

EXPORT_API uint32_t weak_ref_hash_code(context_t context, object weak_ref);

EXPORT_API uint32_t weak_hashset_hash_code(context_t context, object weak_hashset);

EXPORT_API uint32_t stack_frame_hash_code(context_t context, object frame);

EXPORT_API uint32_t env_slot_hash_code(context_t context, object slot);


/**
 * symbol hash code ����
 * <p>https://www.partow.net/programming/hashfunctions/#BKDRHashFunction</p>
 * @param context
 * @param symbol
 * @return imm_i64, �Ǹ���
 */
EXPORT_API uint32_t symbol_hash_code(context_t _context, object symbol);

/**
 * string hash code ����
 * <p>https://www.partow.net/programming/hashfunctions/#BKDRHashFunction</p>
 * @param context
 * @param str
 * @return imm_i64, �Ǹ���
 */
EXPORT_API uint32_t string_hash_code(context_t _context, object str);

/**
                          equals ���� (������ hash ����)
                          todo ������������д equals �㷨
******************************************************************************/
EXPORT_API int i64_equals(context_t context, object i64_a, object i64_b);
EXPORT_API int d64_equals(context_t context, object d64_a, object d64_b);
EXPORT_API int char_equals(context_t context, object char_a, object char_b);
EXPORT_API int boolean_equals(context_t context, object boolean_a, object boolean_b);
EXPORT_API int unit_equals(context_t context, object unit_a, object unit_b);
EXPORT_API int eof_equals(context_t context, object eof_a, object eof_b);
// ע��, ���������ýṹ���������, �ݹ�ȽϿ����޷���ֹ
EXPORT_API int pair_equals(context_t context, object pair_a, object pair_b);
EXPORT_API int bytes_equals(context_t context, object bytes_a, object bytes_b);
EXPORT_API int string_buffer_equals(context_t context, object string_buffer_a, object string_buffer_b);
// ע��, ���������ýṹ���������, �ݹ�ȽϿ����޷���ֹ
EXPORT_API int vector_equals(context_t context, object vector_a, object vector_b);
// ע��, ���������ýṹ���������, �ݹ�ȽϿ����޷���ֹ
EXPORT_API int stack_equals(context_t context, object stack_a, object stack_b);

EXPORT_API int string_port_equals(context_t context, object string_port_a, object string_port_b);

EXPORT_API int stdio_port_equals(context_t context, object stdio_a, object stdio_b);

EXPORT_API int hash_set_equals(context_t context, object hashset_a, object hashset_b);

EXPORT_API int hash_map_equals(context_t context, object hashmap_a, object hashmap_b);

EXPORT_API int weak_ref_equals(context_t context, object weak_ref_a, object weak_ref_b);

EXPORT_API int weak_hashset_equals(context_t context, object weak_hashset_a, object weak_hashset_b);

EXPORT_API int stack_frame_equals(context_t context, object stack_a, object stack_b);

EXPORT_API int env_slot_equals(context_t context, object slot_a, object slot_b);

/**
 * symbol �Ƚ�
 * @param context
 * @param symbol_a
 * @param symbol_b
 * @return 1: ���; 0: �����
 */
EXPORT_API int symbol_equals(context_t _context, object symbol_a, object symbol_b);
/**
 * string �Ƚ�
 * @param context
 * @param str_a
 * @param str_b
 * @return 1: ���; 0: �����
 */
EXPORT_API int string_equals(context_t _context, object str_a, object str_b);

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
#define type_info_finalizer(t)                  type_info_field((t), finalizer)
// hash_code
#define type_info_hash_code(t)                  type_info_field((t), hash_code)
// equals
#define type_info_equals(t)                     type_info_field((t), equals)


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
 * @param _obj object ����Ϊ NULL
 * @return object_type_info_ptr
 */
object_type_info_ptr context_get_object_type(NOTNULL context_t context, NOTNULL object obj);

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
                                    ��������
******************************************************************************/

/**
 * �Ӷ��󷵻� hash_code ����
 * @param context
 * @param obj
 * @return ���������, ���� NULL
 */
EXPORT_API hash_code_fn object_hash_helper(context_t context, object obj);

/**
 * �Ӷ��󷵻� equals ����
 * @param context
 * @param obj
 * @return ���������, ���� NULL
 */
EXPORT_API equals_fn object_equals_helper(context_t context, object obj);


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
