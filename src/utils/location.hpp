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

    constexpr bool operator<(Location rhs) const noexcept {
        if (lineno < rhs.lineno) {
            return true;
        }
        if (lineno > rhs.lineno) {
            return false;
        }
        return columnno < rhs.columnno;
    }

    constexpr bool operator>(Location rhs) const noexcept {
        return rhs < *this;
    }

    constexpr bool operator<=(Location rhs) const noexcept {
        return rhs >= *this;
    }

    constexpr bool operator>=(Location rhs) const noexcept {
        return !(*this < rhs);
    }
};

} // Spark
