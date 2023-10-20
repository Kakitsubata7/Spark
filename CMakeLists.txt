cmake_minimum_required(VERSION 3.26)
project(Spark)

set(CMAKE_CXX_STANDARD 17)

add_executable(Spark
        Spark/Types/Array.cpp
        Spark/Types/Array.hpp
        Spark/Types/Boolean.hpp
        Spark/Types/Coroutine.hpp
        Spark/Types/Dynamic.hpp
        Spark/Types/Float.hpp
        Spark/Types/Function.hpp
        Spark/Types/Integer.hpp
        Spark/Types/Object.hpp
        Spark/Types/String.hpp
        Spark/Types/Table.hpp
        Spark/Types/Thread.hpp
        Spark/Types/Type.hpp
        Spark/Executor.cpp
        Spark/Executor.hpp
        Spark/NativeThread.cpp
        Spark/NativeThread.hpp
        Spark/Opcode.hpp
        Spark/Types.hpp
        Spark/VirtualThread.cpp
        Spark/VirtualThread.hpp
        Spark/VM.cpp
        Spark/VM.hpp
        main.cpp
        Spark.hpp
        Spark/Types/Object.cpp)
