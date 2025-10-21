cmake_minimum_required(VERSION 3.20)
project(spark)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Library
add_library(spark_lib
        src/frontend/lexer.cpp
        src/frontend/lexer.hpp
        src/frontend/syntax.cpp
        src/frontend/syntax.hpp
        src/frontend/token.hpp
        src/runtime/allocator.hpp
        src/runtime/drc.cpp
        src/runtime/drc.hpp
        src/runtime/rc.hpp
        src/runtime/spark.hpp
        src/runtime/thread.hpp
        src/runtime/type.hpp
        src/result.hpp
        src/error.hpp)

target_include_directories(spark_lib PUBLIC src)

# Executable
add_executable(spark
        src/main.cpp)
target_link_libraries(spark PRIVATE spark_lib)

# Testing
enable_testing()

add_executable(spark_test
        tests/frontend/lexer_test.cpp
        tests/runtime/drc_test.cpp)
target_link_libraries(spark_test
        PRIVATE spark_lib
        GTest::gtest_main
        GTest::gmock
)
target_compile_definitions(spark_test PRIVATE SPARK_TESTS_ENABLED)

include(GoogleTest)
gtest_discover_tests(spark_test)
