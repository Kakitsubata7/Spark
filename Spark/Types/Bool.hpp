#pragma once

#include <cstdint>

namespace Spark {

/**
 * This has to be a 8-bit boolean.
 */
struct Bool {

    /* ===== Data ===== */

private:
    uint8_t value;



    /* ===== Constructors ===== */

public:
    constexpr Bool() : value(0) { }
    constexpr Bool(bool b) : value(b ? 1 : 0) { }



    /* ===== Implicit Conversion Operator ===== */

public:
    constexpr operator bool() const { return value != 0; }



    /* ===== Assignment Operator ===== */

public:
    constexpr Bool& operator=(bool b) {
        value = b;
        return *this;
    }



    /* ===== Operators ===== */

public:
    constexpr bool operator!() const { return !value; }

    constexpr bool operator==(Bool other) const { return value == other.value; }

    constexpr bool operator!=(Bool other) const { return value != other.value; }

};

} // Spark
