#pragma once

#include <cstdint>
#include <ostream>

namespace Spark {

enum class Type : uint8_t {
    Nil,
    Integer,
    Float,
    Boolean,
    Pointer,
    Type,
    Box,
    String,
    Array,
    Set,
    Map,
    Object,
    Function,
    Exception,
    Thread,
    Promise
};

std::ostream& operator<<(std::ostream& os, Type type);

} // Spark
