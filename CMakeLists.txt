cmake_minimum_required(VERSION 3.26)
project(Spark)

set(CMAKE_CXX_STANDARD 17)

add_executable(Spark
        main.cpp
        Spark.hpp
        Spark/Value.hpp
        Spark/Env.hpp
        Spark/Type.hpp
        Spark/GC/GCPtr.hpp
        Spark/Bool8.hpp
        Spark/Int64.hpp
        Spark/Float64.hpp
        Spark/Type.cpp
        Spark/GC/GCNode.hpp
        Spark/GC/GC.hpp
)
