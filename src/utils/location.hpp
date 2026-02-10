#pragma once

#include <cstddef>

namespace Spark {

struct Location {
    size_t line;
    size_t column;

    constexpr Location() noexcept : line(0), column(0) { }
    constexpr Location(size_t line, size_t column) noexcept : line(line), column(column) { }

    constexpr bool operator==(Location rhs) const noexcept {
        return line == rhs.line && column == rhs.column;
    }

    constexpr bool operator!=(Location rhs) const noexcept {
        return !(*this == rhs);
    }

    constexpr bool operator<(Location rhs) const noexcept {
        if (line < rhs.line) {
            return true;
        }
        if (line > rhs.line) {
            return false;
        }
        return column < rhs.column;
    }

    constexpr bool operator>(Location rhs) const noexcept {
        return rhs < *this;
    }

    constexpr bool operator<=(Location rhs) const noexcept {
        return !(*this > rhs);
    }

    constexpr bool operator>=(Location rhs) const noexcept {
        return !(*this < rhs);
    }
};

} // Spark
