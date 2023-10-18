#pragma once

namespace Spark {

    using Float = double;

    static_assert(sizeof(Float) == 8, R"(The size of type "Spark::Float" should to be exactly 8 bytes.)");

} // Spark
