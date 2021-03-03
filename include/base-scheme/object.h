#include <stdint.h>
#include <stddef.h>

struct object_struct_t
{
    uint32_t object_tag;
    uint32_t gc_tag;
    union {
        double doublenum;
        int32_t i32;
    } value;
};

typedef struct object_struct_t *object;

#define object_size(value_field)\
    (offsetof(struct object_struct_t, value) + sizeof(((object)0)->value.value_field))
#define object_sizeof_header() (object_size(doublenum) - sizeof(double))