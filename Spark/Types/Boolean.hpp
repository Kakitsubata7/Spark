#pragma once

namespace Spark {

    using Boolean = bool;
    inline constexpr Boolean TRUE = true;
    inline constexpr Boolean FALSE = false;

    static_assert(sizeof(Boolean) <= 8, R"(The size of type "Spark::Boolean" should be less than or equal to 8 bytes.)");

} // Spark
