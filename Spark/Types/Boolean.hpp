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



    /* ===== Constants ===== */

public:
    static constexpr Boolean TRUE = 1;
    static constexpr Boolean FALSE = 0;

};

}
