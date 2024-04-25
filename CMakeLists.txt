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
        Spark/GC/GCNode.hpp
        Spark/Types/Value.hpp
        spark/GC/GCOperation.hpp
        Spark/GC/Operations/CollectOperation.hpp
        Spark/GC/Operations/ReferenceOperation.hpp
        Spark/GC/Operations/UnreferenceOperation.hpp
        Spark/GC/Operations/AllocateOperation.hpp
        spark/Types/Type.hpp
        Spark/GC/GCPtr.hpp
        spark/Types/Value.cpp
        Spark/GC/Operations/CollectOperation.cpp
        Spark/Opcode.hpp
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
        Spark/Types/Namespace.hpp
        Spark/Types/Object.cpp
        Spark/Utilities/ConcurrentQueue.hpp
        Spark/NewThread.hpp
        Spark/NewThread.cpp
        Spark/GC/GCBase.hpp
        Spark/GC/ConcurrentGC.hpp
        Spark/GC/SingleThreadedGC.hpp
        Spark/GC/ConcurrentGC.cpp
        Spark/GC/SingleThreadedGC.cpp
        Spark/InterpretedVM.hpp
        Spark/InterpretedVM.cpp
        Spark/GC/GCType.hpp
)