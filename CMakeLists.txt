cmake_minimum_required(VERSION 3.26)
project(Spark)

set(CMAKE_CXX_STANDARD 17)

add_executable(Spark
        main.cpp
        Spark.hpp
        Spark/GC/GCHandle.tpp
        Spark/GC/GCHandle.hpp
        Spark/SparkContext.cpp
        Spark/SparkContext.hpp
        Spark/Types/TypeTrait.hpp
        Spark/Types.hpp
        Spark/Types/Integer.hpp
        Spark/GC/GC.cpp
        Spark/GC/GC.hpp
        Spark/SparkException.hpp
        Spark/Types/Float.hpp
        Spark/Types/String.cpp
        Spark/Types/String.hpp
)
