#pragma once

#include <cstdint>
#include <ostream>

namespace Spark {

enum class Type : uint8_t {
    Nil       = 0x00,
    Integer   = 0x01,
    Float     = 0x02,
    Boolean   = 0x03,
    CFunction = 0x04,
    Type      = 0x05,
    String    = 0x06,
    Array     = 0x07,
    Set       = 0x08,
    Map       = 0x09,
    Function  = 0x0A,
    Closure   = 0x0B,
    Thread    = 0x0C,
    Namespace = 0x0D,
    Object    = 0x0E
};

std::ostream& operator<<(std::ostream& os, Type type);

} // Spark
