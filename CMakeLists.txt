cmake_minimum_required(VERSION 3.26)
project(Spark)

set(CMAKE_CXX_STANDARD 17)

add_executable(Spark
        main.cpp
        Spark.hpp
        Spark/Opcode.hpp
        Spark/Types.hpp
        Spark/Types/Array.hpp
        Spark/Types/Boolean.hpp
        Spark/Types/Dynamic.hpp
        Spark/Types/Float.hpp
        Spark/Types/Integer.hpp
        Spark/Types/Object.hpp
        Spark/Types/String.hpp
        Spark/Types/Table.hpp
        Spark/Types/Type.hpp
        Spark/Executor.cpp
        Spark/Executor.hpp
        Spark/VM.cpp
        Spark/VM.hpp
        Spark/Thread.cpp
        Spark/Thread.hpp
        Spark/VirtualThread.cpp
        Spark/VirtualThread.hpp)
