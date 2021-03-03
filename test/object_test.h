#include "base-scheme/object.h"

#define UT(test_name) UTEST(object_test, test_name)

// test case here
UT(hello_utest) {
    ASSERT_TRUE(1);
}

UT(struct_offset) {
    printf("i32 object size: %zd, header size: %zd\n", object_size(doublenum), object_sizeof_header());
    ASSERT_TRUE(1);
}