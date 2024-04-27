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
        Spark/GC/Operations/GCOperation.hpp
        Spark/GC/Operations/CollectOperation.hpp
        Spark/GC/Operations/ReferenceOperation.hpp
        Spark/GC/Operations/UnreferenceOperation.hpp
        Spark/GC/Operations/AllocateOperation.hpp
        Spark/Types/TypeID.hpp
        Spark/GC/GCPtr.hpp
        spark/Types/Value.cpp
        Spark/GC/Operations/CollectOperation.cpp
        Spark/Opcode.hpp
        Spark/Config.hpp
        Spark/Types/TypeID.cpp
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
        Spark/GC/Operations/AddEntryOperation.hpp
        Spark/GC/Operations/RemoveEntryOperation.hpp
        Spark/Compilation/Compiler.hpp
        Spark/Compilation/Lex/Lexer.hpp
        Spark/Compilation/Parse/ASTNode.hpp
        Spark/Compilation/Parse/BinaryExpression.hpp
        Spark/Compilation/Parse/StatementNode.hpp
        Spark/Compilation/Parse/ExpressionNode.hpp
        Spark/Compilation/Lex/Token.hpp
        Spark/Compilation/Lex/TokenType.hpp
        Spark/Compilation/Lex/Lexer.cpp
        Spark/Compilation/Lex/Token.cpp
        Spark/Compilation/Lex/TokenType.cpp
        Spark/Compilation/Lex/LexException.hpp
)