cmake_minimum_required(VERSION 3.26)
project(Spark)

set(CMAKE_CXX_STANDARD 17)

# add_compile_options(-Wall -Wextra -Werror)

add_executable(Spark
        main.cpp
        Spark/GC/GCNode.hpp
        Spark/GC/Operations/GCOperation.hpp
        Spark/GC/Operations/CollectOperation.hpp
        Spark/GC/Operations/ReferenceOperation.hpp
        Spark/GC/Operations/UnreferenceOperation.hpp
        Spark/GC/Operations/AllocateOperation.hpp
        Spark/GC/GCPtr.hpp
        Spark/GC/Operations/CollectOperation.cpp
        Spark/Compilation/BytecodeBuffer.hpp
        Spark/Utilities/ConcurrentQueue.hpp
        Spark/GC/GCBase.hpp
        Spark/GC/ConcurrentGC.hpp
        Spark/GC/SingleThreadedGC.hpp
        Spark/GC/ConcurrentGC.cpp
        Spark/GC/SingleThreadedGC.cpp
        Spark/GC/GCType.hpp
        Spark/GC/Operations/AddEntryOperation.hpp
        Spark/GC/Operations/RemoveEntryOperation.hpp
        Spark/Compilation/Compiler.hpp
        Spark/Compilation/Lex/Lexer.hpp
        Spark/Compilation/Parse/AST/ASTNode.hpp
        Spark/Compilation/Parse/AST/Expressions/BinaryExpression.hpp
        Spark/Compilation/Parse/AST/Statements/StatementNode.hpp
        Spark/Compilation/Parse/AST/Expressions/ExpressionNode.hpp
        Spark/Compilation/Lex/Token.hpp
        Spark/Compilation/Lex/TokenType.hpp
        Spark/Compilation/Lex/Lexer.cpp
        Spark/Compilation/Lex/Token.cpp
        Spark/Compilation/Lex/TokenType.cpp
        Spark/Compilation/Lex/LexException.hpp
        Spark/Compilation/Parse/AST/Expressions/AddExpression.hpp
        Spark/Compilation/Parse/AST/Expressions/ConstExpression.hpp
        Spark/Compilation/Parse/AST/Statements/SequenceStatement.hpp
        Spark/Compilation/Parse/AST/Statements/IfStatement.hpp
        Spark/Compilation/Parse/AST/Statements/ExpressionStatement.hpp
        Spark/Compilation/Parse/AST/EvaluateVisitor.hpp
        Spark/Compilation/Parse/Types/Value.hpp
        Spark/Compilation/Parse/AST/EvaluateVisitor.cpp
        Spark/Compilation/Parse/Types/Int.hpp
        Spark/Compilation/Parse/Types/Float.hpp
        Spark/Compilation/Parse/Types/Type.hpp
        Spark/Compilation/Parse/Types/Value.cpp
        Spark/Compilation/Parse/Types/Type.cpp
        Spark/Compilation/Parse/Types/Bool.hpp
        Spark/Compilation/Parse/AST/Expressions/SubtractExpression.hpp
)