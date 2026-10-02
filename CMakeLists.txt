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

add_custom_target(flex
    DEPENDS
        ${FLEX_sparklexer_OUTPUTS}
)

#BISON_TARGET(
#        sparkparser
#        ${FRONTEND_DIR}/parser.y
#        ${FRONTEND_GENERATED_DIR}/parser.tab.cpp
#        COMPILE_FLAGS "--defines=${FRONTEND_GENERATED_DIR}/parser.tab.hpp"
#)

BISON_TARGET(
        sparkparser
        ${FRONTEND_DIR}/parser.y
        ${FRONTEND_GENERATED_DIR}/parser.tab.cpp
        COMPILE_FLAGS "--defines=${FRONTEND_GENERATED_DIR}/parser.tab.hpp -Wcounterexamples -Wconflicts-sr"
)

add_custom_target(bison
    DEPENDS
        ${BISON_sparkparser_OUTPUTS}
)

add_flex_bison_dependency(sparklexer sparkparser)

# Library
add_library(sparklib
        ${FLEX_sparklexer_OUTPUTS}
        ${BISON_sparkparser_OUTPUTS}

        
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
