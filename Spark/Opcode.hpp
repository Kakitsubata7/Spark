#pragma once

#include <cstdint>

namespace Spark {

    enum class Opcode : uint8_t {

        Halt,

        PushNone,
        PushType,
        PushInteger,
        PushFloat,
        PushBoolean,
        PushString,
        PushArray,
        PushTable,
        PushObject,

    };

} // Spark
