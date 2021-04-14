# 项目定义
cmake_minimum_required(VERSION 3.8)

project(paper-scheme LANGUAGES C)

# 需要 C99
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_STANDARD_REQUIRED ON)

# 头文件目录
include_directories(include)
# 公共库
aux_source_directory(src SRCS)
list(REMOVE_ITEM SRCS "src/paper-scheme.c")
add_library(paper-scheme-lib STATIC ${SRCS})
# 可执行文件
add_executable(paper-scheme-exec src/paper-scheme.c)
target_link_libraries(paper-scheme-exec paper-scheme-lib)

# DEBUG 构建宏
# 当 Debug 构建模式时会开启一些调试信息和断言
if (CMAKE_BUILD_TYPE STREQUAL Debug)
    target_compile_definitions(paper-scheme-lib PUBLIC "DEBUG_ONLY")
    target_compile_definitions(paper-scheme-exec PUBLIC "DEBUG_ONLY")
    message(STATUS "paper-scheme-lib Debug build")
    message(STATUS "paper-scheme-exec Debug build")
endif ()

# 架构探测与宏定义
if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    # IS_64_BIT_ARCH
    target_compile_definitions(paper-scheme-exec PUBLIC "IS_64_BIT_ARCH")
    target_compile_definitions(paper-scheme-lib PUBLIC "IS_64_BIT_ARCH")
    message(STATUS "Target is 64 bits")
else ()
    # IS_32_BIT_ARCH
    target_compile_definitions(paper-scheme-exec PUBLIC "IS_32_BIT_ARCH")
    target_compile_definitions(paper-scheme-lib PUBLIC "IS_32_BIT_ARCH")
    message(STATUS "Target is 32 bits")
endif ()

if (MSVC)
    # fopen()
    target_compile_definitions(paper-scheme-exec PUBLIC "_CRT_SECURE_NO_WARNINGS")
    target_compile_definitions(paper-scheme-lib PUBLIC "_CRT_SECURE_NO_WARNINGS")
endif (MSVC)

# 测试
# 是否需要开启测试, 默认开启
option(ENABLE_UNIT_TESTS "Enable unit tests" ON)
message(STATUS "Enable testing: ${ENABLE_UNIT_TESTS}")

if (ENABLE_UNIT_TESTS)
    # 配置测试
    INCLUDE_DIRECTORIES(test/)
    add_executable(paper-scheme-test "")
    target_sources(paper-scheme-test
            PRIVATE
            "test/test_cases/object_test.c"
            "test/test_cases/context_test.c"
            "test/test_cases/gc_test.c"
            "test/test_cases/runtime_test.c"
            "test/test_cases/value_test.c"
            "test/test_cases/hash_test.c"
            "test/test_cases/equals_test.c"
            "test/test_cases/hashmap_test.c"
            "test/test_cases/hashset_test.c"
            "test/test_cases/weak_hashset_test.c"
            "test/test_cases/global_symbol_table_test.c"
            "test/test_cases/port_test.c"
            "test/test_cases/environment_test.c"
            "test/test_cases/scheme_stack_test.c"
            "test/test_cases/syntax_test.c"
            "test/test_cases/opcode_test.c"
            "test/test_run.c"
            )
    target_link_libraries(paper-scheme-test paper-scheme-lib)
    if (CMAKE_BUILD_TYPE STREQUAL Debug)
        target_compile_definitions(paper-scheme-test PUBLIC "DEBUG_ONLY")
        message(STATUS "paper-scheme-test Debug build")
    endif ()
    if (MSVC)
        target_compile_definitions(paper-scheme-test PUBLIC "IS_USE_MSVC")
        target_compile_definitions(paper-scheme-test PUBLIC "_CRT_SECURE_NO_WARNINGS")
    endif (MSVC)
    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        # IS_64_BIT_ARCH
        target_compile_definitions(paper-scheme-test PUBLIC "IS_64_BIT_ARCH")
        message(STATUS "Target is 64 bits")
    else ()
        # IS_32_BIT_ARCH
        target_compile_definitions(paper-scheme-test PUBLIC "IS_32_BIT_ARCH")
        message(STATUS "Target is 32 bits")
    endif ()
    # 添加测试
    add_test(
            NAME paper-scheme-test
            COMMAND $<TARGET_FILE:paper-scheme-test>
    )
endif ()