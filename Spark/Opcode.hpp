#pragma once

#include <cstdint>

namespace Spark {

enum class Opcode : uint8_t {

    Halt,               // Stop the execution of the current thread.

    PushNil,            // Push a 'nil' value onto the stack.
    PushInteger,        // Fetch 8 bytes value and push it as a signed integer.
    PushFloat,          // Fetch 8 bytes and push it as a floating-point number.
    PushBoolean,        // Fetch 1 byte and push it as a boolean.
    PushString,         // Fetch an 8-byte integer, and use it to obtain a string, then push the string onto the stack.
    PushEmptyArray,     // Push an empty array onto the stack.
    PushEmptySet,       // Push an empty set onto the stack.
    PushEmptyMap,       // Push an empty map onto the stack.

    Push,               // Fetch an 8-byte index, and use it to push a value to the operation stack.
    PushStorage,        // Fetch an 8-byte index, and use it to push a value to the storage stack.

    Pop,                // Pop a value from the operation stack.
    PopStorage,         // Pop a value from the storage stack.

    Add,                // Pop two values and perform the '+' operation. Push the result onto the stack.
    Subtract,           // Pop two values and perform the '-' operation. Push the result onto the stack.
    Multiply,           // Pop two values and perform the '*' operation. Push the result onto the stack.
    Divide,             // Pop two values and perform the '/' operation. Push the result onto the stack.
    Modulus,            // Pop two values and perform the '%' operation. Push the result onto the stack.

    Equal,              // Pop two values and perform the '==' operation. Push the result onto the stack.
    NotEqual,           // Pop two values and perform the '!=' operation. Push the result onto the stack.
    Less,               // Pop two values and perform the '<' operation. Push the result onto the stack.
    LessOrEqual,        // Pop two values and perform the '<=' operation. Push the result onto the stack.
    Greater,            // Pop two values and perform the '>' operation. Push the result onto the stack.
    GreaterOrEqual,     // Pop two values and perform the '>=' operation. Push the result onto the stack.

    LogicalAnd,         // Pop two values and perform the '&&' operation. Push the result onto the stack.
    LogicalOr,          // Pop two values and perform the '||' operation. Push the result onto the stack.
    LogicalXor,         // Pop two values and perform the '^^' operation. Push the result onto the stack.
    LogicalNegation,    // Pop two values and perform the '!' operation. Push the result onto the stack.

    BitwiseAnd,         // Pop two values and perform the '&' operation. Push the result onto the stack.
    BitwiseOr,          // Pop two values and perform the '|' operation. Push the result onto the stack.
    BitwiseXor,         // Pop two values and perform the '^' operation. Push the result onto the stack.
    BitwiseComplement,  // Pop two values and perform the '~' operation. Push the result onto the stack.
    BitwiseLeftShift,   // Pop two values and perform the '<<' operation. Push the result onto the stack.
    BitwiseRightShift,  // Pop two values and perform the '>>' operation. Push the result onto the stack.

    PushField,          //
    SetField,           //

    Call,               // Fetch an 8-byte integer as the number of arguments. Pop a callable value and call it.
    Return,             //

    Capture,            // Pop a value

};

} // Spark
