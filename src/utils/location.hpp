#pragma once

#include <cstddef>

namespace Spark {

struct Location {
    size_t lineno;
    size_t columnno;

    constexpr Location() : lineno(0), columnno(0) { }
    constexpr Location(size_t lineno, size_t columnno) : lineno(lineno), columnno(columnno) { }
};

} // Spark
