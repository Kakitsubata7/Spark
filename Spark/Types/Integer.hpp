#pragma once

#include <cstdint>

namespace Spark {

    using Integer = int64_t;

    static_assert(sizeof(Integer) == 8, R"(The size of type "Spark::Integer" should to be exactly 8 bytes.)");

} // Spark
