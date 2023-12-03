#pragma once

#include <cstdint>
#include <iomanip>

namespace Spark {

struct Pointer {

    /* ===== Data ===== */

private:
    void* value;



    /* ===== Constructor ===== */

public:
    constexpr Pointer(void* value = nullptr) : value(value) { }



    /* ===== Conversion Operator ===== */

public:
    constexpr operator void*() const { return value; }



    /* ===== Operators ===== */

public:
    constexpr Pointer& operator=(const Pointer pointer) {
        value = pointer.value;
        return *this;
    }

    friend std::ostream& operator<<(std::ostream& os, const Pointer pointer) {
        os << "0x"
           << std::hex
           << std::setw(2 * sizeof(void*))
           << std::setfill('0')
           << reinterpret_cast<uintptr_t>(pointer.value);
        return os;
    }

};

}
