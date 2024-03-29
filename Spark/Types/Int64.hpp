#pragma once

#include <cstdint>

namespace Spark {

    using Int64 = int64_t;

    static_assert(sizeof(Int64) == 8, "The size of 'Int64' has to be 8 bytes.");

} // Spark
