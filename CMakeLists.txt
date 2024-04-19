cmake_minimum_required(VERSION 3.26)
project(Spark)

set(CMAKE_CXX_STANDARD 17)

# add_compile_options(-Wall -Wextra -Werror)

add_executable(Spark
        main.cpp
        Spark/Types/Int.hpp
        Spark/Types/Float.hpp
        Spark/Types/Bool.hpp
        Spark/Types.hpp
        Spark/API.hpp
        Spark/API.cpp
        Spark/Env.hpp
        Spark/Thread.hpp
        Spark/GC/GC.hpp
        Spark/GC/GCNode.hpp
        Spark/Types/Value.hpp
        spark/GC/GC.cpp
        spark/GC/GCOperation.hpp
        spark/GC/CollectOperation.hpp
        spark/GC/ReferenceOperation.hpp
        spark/GC/UnreferenceOperation.hpp
        spark/GC/AllocateOperation.hpp
        spark/Types/Type.hpp
        Spark/GC/GCPtr.hpp
        spark/Types/Value.cpp
        Spark/GC/CollectOperation.cpp
        Spark/Env.cpp
        Spark/Opcode.hpp
        Spark/Thread.cpp
        Spark/Config.hpp
        Spark/Types/Type.cpp
        Spark/Compilation/BytecodeBuffer.hpp
        Spark/Types/Int64.hpp
        Spark/Types/Float64.hpp
        Spark/Types/CFunction.hpp
        Spark/Types/Closure.hpp
        Spark/Types/Object.hpp
        Spark/Types/Function.hpp
        Spark/Types/Array.hpp
        Spark/StackBuffer.hpp
        Spark/StackBuffer.cpp
        Spark/Types/Namespace.hpp
        Spark/Types/Object.cpp
        Spark/Utilities/ConcurrentQueue.hpp
)