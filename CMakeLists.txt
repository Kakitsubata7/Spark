cmake_minimum_required(VERSION 3.26)
project(Spark)

set(CMAKE_CXX_STANDARD 17)

add_executable(Spark
        main.cpp
        Spark.hpp
        Spark/SparkContext.cpp
        Spark/SparkContext.hpp
        Spark/Types/TypeTrait.hpp
        Spark/Types.hpp
        Spark/Types/Integer.hpp
        Spark/Types/Float.hpp
        Spark/Types/String.hpp
        Spark/Types/Type.hpp
        Spark/GC/GC.hpp
        Spark/GC/GCNode.hpp
        Spark/GC/GCHandle.hpp
        Spark/Types/Value.hpp
        Spark/Types/Boolean.hpp
        Spark/Types/Boolean.hpp
        Spark/Types/Value.cpp
        Spark/Types/Pointer.hpp
        Spark/Types/Float.inl
        Spark/Types/Integer.inl
)
