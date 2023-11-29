#pragma once

#include <cstdint>

namespace Spark {

struct Boolean {

    /* ===== Data ===== */

private:
    constexpr uint8_t value;



    /* ===== Constructors ===== */

public:
    constexpr Boolean() : value(0) { }
    constexpr Boolean(const bool value) : value(value ? 1 : 0) { }
    explicit constexpr Boolean(const uint8_t value) : value(value) { }



    /* ===== Conversion Operators ===== */

public:
    constexpr operator bool() const { return value != 0; }



    /* ===== Operators ===== */

public:
    constexpr Boolean operator !() const { return !value; }



    /* ===== Constants ===== */

public:
    static constexpr Boolean TRUE = 1;
    static constexpr Boolean FALSE = 0;

};

}
