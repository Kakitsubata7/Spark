#pragma once

#include <cstddef>

namespace Spark {

struct Location {
    size_t row;
    size_t col;

    constexpr Location(size_t row, size_t col) noexcept : row{row}, col{col} { }
    constexpr Location() noexcept : Location{0, 0} { }
};

} // Spark
