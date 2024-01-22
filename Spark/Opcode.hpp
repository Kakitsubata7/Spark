#pragma once

#include <cstdint>

namespace Spark {

enum class Opcode : uint8_t {

    Halt,

    PushNil,
    PushInteger,
    PushFloat,
    PushBoolean,
    PushType,
    Pop,

    Add,
    Subtract,
    Multiply,
    Divide,
    Modulus,
    BitwiseComplement,
    BitwiseAnd,
    BitwiseOr,
    BitwiseXor,
    BitwiseLeftShift,
    BitwiseRightShift,
    LogicalNot,
    LogicalAnd,
    LogicalOr,

    Call,
    Return,

};

} // Spark
