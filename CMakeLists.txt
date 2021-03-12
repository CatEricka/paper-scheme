# 项目定义
cmake_minimum_required (VERSION 3.8)

project(base-scheme LANGUAGES C)

# 需要 C99
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_STANDARD_REQUIRED ON)

# 源码目录
aux_source_directory(src SRCS)
# 头文件目录
include_directories(include)
# 可执行文件
add_executable(base-scheme-exec ${SRCS})

# 架构探测与宏定义
if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    # IS_64_BIT_ARCH
    target_compile_definitions(base-scheme-exec PUBLIC "IS_64_BIT_ARCH")
    message(STATUS "Target is 64 bits")
else ()
    # IS_32_BIT_ARCH
    target_compile_definitions(base-scheme-exec PUBLIC "IS_32_BIT_ARCH")
    message(STATUS "Target is 32 bits")
endif ()

# DEBUG 构建宏
if (CMAKE_BUILD_TYPE STREQUAL Debug)
    add_definitions(
            -DDEBUG_ONLY
    )
endif ()

# 测试
# 是否需要开启测试, 默认开启
option(ENABLE_UNIT_TESTS "Enable unit tests" ON)
message(STATUS "Enable testing: ${ENABLE_UNIT_TESTS}")

if (ENABLE_UNIT_TESTS)
    # 配置测试
    INCLUDE_DIRECTORIES(test/test_cases)
    add_executable(base-scheme-test "")
    target_sources(base-scheme-test
            PRIVATE
            "test/test_run.c"
            )
    if (MSVC)
        target_compile_definitions(base-scheme-test PUBLIC "IS_USE_MSVC")
    endif (MSVC)
    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        # IS_64_BIT_ARCH
        target_compile_definitions(base-scheme-test PUBLIC "IS_64_BIT_ARCH")
        message(STATUS "Target is 64 bits")
    else ()
        # IS_32_BIT_ARCH
        target_compile_definitions(base-scheme-test PUBLIC "IS_32_BIT_ARCH")
        message(STATUS "Target is 32 bits")
    endif ()
    # 添加测试
    add_test(
            NAME base-scheme-test
            COMMAND $<TARGET_FILE:base-scheme-test>
    )
endif()