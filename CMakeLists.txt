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
find_package(Boost REQUIRED)

# Flex & Bison
set(FRONTEND_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/frontend)
set(FRONTEND_GENERATED_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated/frontend)
file(MAKE_DIRECTORY ${FRONTEND_GENERATED_DIR})

FLEX_TARGET(
        sparklexer
        ${FRONTEND_DIR}/lexer.l
        ${FRONTEND_GENERATED_DIR}/lex.yy.cpp
        COMPILE_FLAGS "--header-file=${FRONTEND_GENERATED_DIR}/lex.yy.hpp"
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

        src/core/type.hpp

        src/frontend/ast.hpp

        src/frontend/ast/expr.hpp
        src/frontend/ast/node.hpp
        src/frontend/ast/node_visitor.hpp
        src/frontend/ast/pattern.hpp
        src/frontend/ast/stmt.hpp
        src/frontend/ast/type_nodes.hpp
        src/frontend/ast/utils.hpp

        src/frontend/lexer.cpp
        src/frontend/lexer.hpp
        src/frontend/lexer/lexer_state.hpp
        src/frontend/lexer/lexer_utils.cpp
        src/frontend/lexer/lexer_utils.hpp
        src/frontend/lexer/token.hpp
        src/frontend/lexer/token_buffer.hpp
        src/frontend/lexer/token_type.cpp
        src/frontend/lexer/token_type.hpp
        src/frontend/lexer/token_value.hpp

        src/frontend/parser.cpp
        src/frontend/parser.hpp
        src/frontend/parser/parser_context.hpp

        src/frontend/semantic_type.hpp
        src/frontend/source_buffer.cpp
        src/frontend/source_buffer.hpp

        src/runtime/allocator.hpp
        src/runtime/drc.cpp
        src/runtime/drc.hpp
        src/runtime/rc.hpp
        src/runtime/spark.hpp
        src/runtime/thread.hpp

        src/utils/bigint.hpp
        src/utils/bigreal.hpp
        src/utils/error.hpp
        src/utils/location.hpp
        src/utils/result.hpp
)

target_include_directories(sparklib
    PUBLIC
        src
        ${FRONTEND_GENERATED_DIR}
        ${Boost_INCLUDE_DIRS}
)

# Executable
add_executable(spark
        src/main.cpp)
target_link_libraries(spark PRIVATE sparklib)

# Testing
enable_testing()

add_executable(sparktest
        tests/frontend/lex_test.cpp
        tests/frontend/lexer_test.cpp
        tests/frontend/parser_test.cpp
        tests/frontend/source_buffer_test.cpp
        tests/frontend/token_buffer_test.cpp
        tests/frontend/token_value_test.cpp
        tests/frontend/utils.cpp
        tests/frontend/utils.hpp
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
