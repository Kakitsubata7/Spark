#pragma once

#include <cstddef>

namespace Spark {

struct Location {
    size_t lineno;
    size_t columnno;

    constexpr Location() noexcept : lineno(0), columnno(0) { }
    constexpr Location(size_t lineno, size_t columnno) noexcept : lineno(lineno), columnno(columnno) { }

    constexpr bool operator==(Location rhs) const noexcept {
        return lineno == rhs.lineno && columnno == rhs.columnno;
    }

    constexpr bool operator!=(Location rhs) const noexcept {
        return !(*this == rhs);
    }
};

} // Spark
