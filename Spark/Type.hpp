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
    String,
    Array,
    Set,
    Map,
    Object,
    Function,
    Thread,
    Promise
};

std::ostream& operator<<(std::ostream& os, const Type type);

} // Spark
