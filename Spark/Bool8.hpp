#pragma once

#include <cstdint>

namespace Spark {

struct Bool8 {

    /* ===== Data ===== */

private:
    uint8_t value;



    /* ===== Constructors ===== */

public:
    constexpr Bool8() : value(0) { }
    constexpr Bool8(bool b) : value(b ? 1 : 0) { }



    /* ===== Implicit Conversion Operator ===== */

public:
    constexpr operator bool() const { return value != 0; }



    /* ===== Assignment Operator ===== */

public:
    constexpr Bool8& operator=(bool b) {
        value = b;
        return *this;
    }



    /* ===== Operators ===== */

public:
    constexpr bool operator!() const { return !value; }

    constexpr bool operator==(Bool8 other) const { return value == other.value; }

    constexpr bool operator!=(Bool8 other) const { return value != other.value; }

};

} // Spark
