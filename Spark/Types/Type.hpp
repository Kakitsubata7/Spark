#pragma once

#include <cstdint>

namespace Spark {

enum class Type : uint8_t {
    None,
    Type,
    Integer,
    Float,
    Boolean,
    Pointer,
    String,
    Object,
    Array,
    Table
};

} // Spark
