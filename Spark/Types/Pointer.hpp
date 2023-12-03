#pragma once

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
        os << pointer.value;
        return os;
    }

};

}
