cmake_minimum_required(VERSION 3.26)
project(Spark)

set(CMAKE_CXX_STANDARD 17)

add_executable(Spark
        main.cpp
        Spark.hpp
        Spark/Types/Integer.hpp
        Spark/Types/Float.hpp
        Spark/Types/Type.hpp
        Spark/Types/Boolean.hpp
        Spark/Types/Boolean.hpp
        Spark/Env.hpp
)
