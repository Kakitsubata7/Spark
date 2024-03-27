#pragma once

#include <cstdint>
#include <ostream>

namespace Spark {

/**
 * The size of this type has to be 8 bits.
 */
enum class Type : uint8_t {
    Nil     = 0x00,
    Integer = 0x01,
    Float   = 0x02,
    Boolean = 0x03,
    Type    = 0x04,
    String  = 0x05,
    Array   = 0x06,
    Set     = 0x07,
    Map     = 0x08,
    Object  = 0x09,
    Thread  = 0x0A
};

std::ostream& operator<<(std::ostream& os, Type type);

} // Spark
