cmake_minimum_required(VERSION 3.30)
project(spark)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Library
add_library(spark_lib
        src/compiler/ast.hpp
        src/compiler/expr.hpp
        src/compiler/lexer.cpp
        src/compiler/lexer.hpp
        src/compiler/result.hpp
        src/compiler/token.cpp
        src/compiler/token.hpp
        src/utils/trie.cpp
        src/utils/trie.hpp
)
target_include_directories(spark_lib PUBLIC src)

# Executable
add_executable(spark src/main.cpp)
target_link_libraries(spark PRIVATE spark_lib)

# Testing
enable_testing()

add_executable(spark_test
        tests/token_test.cpp
        tests/trie_test.cpp
)
target_link_libraries(spark_test
        PRIVATE spark_lib
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(spark_test)
