cmake_minimum_required(VERSION 3.26)
project(Spark)

set(CMAKE_CXX_STANDARD 17)

add_compile_options(-Wall -Wextra -Werror)

add_executable(Spark
        main.cpp
        Spark/Types/Integer.hpp
        Spark/Types/Float.hpp
        Spark/Types/Bool.hpp
        Spark/Types.hpp
        Spark/API.hpp
        Spark/API.cpp
        Spark/Env.hpp
        Spark/Thread.hpp
)