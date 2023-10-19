#pragma once

#include <cstdint>

namespace Spark {

    enum class Type : uint64_t {
        None = 0x00,
        Type = 0x01,
        Integer = 0x02,
        Float = 0x03,
        Boolean = 0x04,
        String = 0x05,
        Array = 0x06,
        Table = 0x07,
        Object = 0x08,
        Function = 0x09,
        Coroutine = 0x0A,
        Thread = 0x0B
    };

    static_assert(sizeof(Type) == 8, R"(The size of type "Spark::Type" should to be exactly 8 bytes.)");

} // Spark
