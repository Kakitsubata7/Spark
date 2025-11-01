cmake_minimum_required(VERSION 3.20)
project(spark LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Dependencies
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
# For Windows: prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

find_package(FLEX REQUIRED)
find_package(BISON 3.5 REQUIRED)

# Flex & Bison
set(FRONTEND_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/frontend)
set(FRONTEND_GENERATED_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated/frontend)
file(MAKE_DIRECTORY ${FRONTEND_GENERATED_DIR})

FLEX_TARGET(
        sparklexer
        ${FRONTEND_DIR}/lexer.l
        ${FRONTEND_GENERATED_DIR}/lex.yy.cpp
)

BISON_TARGET(
        sparkparser
        ${FRONTEND_DIR}/parser.y
        ${FRONTEND_GENERATED_DIR}/parser.tab.cpp
        COMPILE_FLAGS "--defines=${FRONTEND_GENERATED_DIR}/parser.tab.hpp"
)

add_flex_bison_dependency(sparklexer sparkparser)

# Library
add_library(sparklib
        ${FLEX_sparklexer_OUTPUTS}
        ${BISON_sparkparser_OUTPUTS}

        src/frontend/lexer_utils.cpp
        src/frontend/lexer_utils.hpp
        src/frontend/token_value.hpp

        src/runtime/allocator.hpp
        src/runtime/drc.cpp
        src/runtime/drc.hpp
        src/runtime/rc.hpp
        src/runtime/spark.hpp
        src/runtime/thread.hpp
        src/runtime/type.hpp

        src/result.hpp
        src/error.hpp
)

target_include_directories(sparklib
    PRIVATE
        src
        ${FRONTEND_GENERATED_DIR}
)

# Executable
add_executable(spark
        src/main.cpp)
target_link_libraries(spark PRIVATE sparklib)

# Testing
enable_testing()

add_executable(sparktest
        tests/frontend/lexer_test.cpp
        tests/runtime/drc_test.cpp
)
target_include_directories(sparktest
    PRIVATE
        src
        ${FRONTEND_GENERATED_DIR}
)
target_link_libraries(sparktest
    PRIVATE
        sparklib
        GTest::gtest_main
        GTest::gmock
)
target_compile_definitions(sparktest PRIVATE SPARK_TESTS_ENABLED)

include(GoogleTest)
gtest_discover_tests(sparktest)
