#include "lib/utest.h"
#include <paper-scheme/context.h>


/**
 * 对 context 结构进行静态断言测试
 */

UTEST(context_test, object_type_info_cal_test) {
    struct object_runtime_type_info_t info = {
            .size_base = 1,
            .size_meta_size_offset = 2,
            .size_meta_size_scale = 3,
            .member_base = 0,
            .member_eq_len_base = 1,
            .member_len_base = 2,
            .member_meta_len_offset = 3,
            .member_meta_len_scale = 4,
    };
    struct object_struct_t obj = {
            .type = 1,
    };

    object_type_info_sizeof(&info, &obj);
    object_type_info_member_slots_of(&info, &obj);
    object_type_info_member_eq_slots_of(&info, &obj);
    type_info_get_object_ptr_of_first_member(&info, &obj);
}