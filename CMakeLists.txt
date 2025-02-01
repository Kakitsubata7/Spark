cmake_minimum_required(VERSION 3.30)
project(Spark)

set(CMAKE_CXX_STANDARD 17)

add_executable(Spark
        src/compiler/ast.hpp
        src/compiler/expr.hpp
        src/compiler/lexer.cpp
        src/compiler/lexer.hpp
        src/compiler/result.hpp
        src/compiler/token.cpp
        src/compiler/token.hpp
        src/utils/trie.cpp
        src/utils/trie.hpp
        src/main.cpp
)
