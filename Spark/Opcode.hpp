#pragma once

#include <cstdint>

namespace Spark {

enum class Opcode : uint8_t {

    Halt,

    PushNil,
    PushInteger,
    PushFloat,
    PushBoolean,
    PushString,

    Pop,

    Add,
    Subtract,
    Multiply,
    Divide,
    Modulus,

    PushField,
    SetField,

};

} // Spark
