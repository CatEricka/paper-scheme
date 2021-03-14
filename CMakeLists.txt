# 项目定义
cmake_minimum_required (VERSION 3.8)

project(scheme-base LANGUAGES C)

# 需要 C99
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_STANDARD_REQUIRED ON)

# 头文件目录
include_directories(include)
# 公共库
aux_source_directory(src SRCS)
list(REMOVE_ITEM SRCS "src/scheme-base.c")
add_library(scheme-base-lib STATIC ${SRCS})
# 可执行文件
add_executable(scheme-base-exec src/scheme-base.c)
target_link_libraries(scheme-base-exec scheme-base-lib)

# 架构探测与宏定义
if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    # IS_64_BIT_ARCH
    target_compile_definitions(scheme-base-exec PUBLIC "IS_64_BIT_ARCH")
    target_compile_definitions(scheme-base-lib PUBLIC "IS_64_BIT_ARCH")
    message(STATUS "Target is 64 bits")
else ()
    # IS_32_BIT_ARCH
    target_compile_definitions(scheme-base-exec PUBLIC "IS_32_BIT_ARCH")
    target_compile_definitions(scheme-base-lib PUBLIC "IS_32_BIT_ARCH")
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
    add_executable(scheme-base-test "")
    target_sources(scheme-base-test
            PRIVATE
            "test/test_run.c"
            )
    target_link_libraries(scheme-base-test scheme-base-lib)
    if (MSVC)
        target_compile_definitions(scheme-base-test PUBLIC "IS_USE_MSVC")
    endif (MSVC)
    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        # IS_64_BIT_ARCH
        target_compile_definitions(scheme-base-test PUBLIC "IS_64_BIT_ARCH")
        message(STATUS "Target is 64 bits")
    else ()
        # IS_32_BIT_ARCH
        target_compile_definitions(scheme-base-test PUBLIC "IS_32_BIT_ARCH")
        message(STATUS "Target is 32 bits")
    endif ()
    # 添加测试
    add_test(
            NAME scheme-base-test
            COMMAND $<TARGET_FILE:scheme-base-test>
    )
endif()