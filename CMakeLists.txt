cmake_minimum_required(VERSION 3.26)
project(Spark)

set(CMAKE_CXX_STANDARD 17)

# add_compile_options(-Wall -Wextra -Werror)

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
        Spark/GC/GC.hpp
        Spark/GC/GCNode.hpp
        Spark/Types/Value.hpp
        spark/GC/GC.cpp
        spark/GC/GCOperation.hpp
        spark/GC/CollectOperation.hpp
        spark/GC/ReferenceOperation.hpp
        spark/GC/DereferenceOperation.hpp
        spark/GC/AllocateOperation.hpp
        spark/Types/Type.hpp
        Spark/GC/GCPtr.hpp
)