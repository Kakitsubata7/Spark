#include "Integer.hpp"

namespace Spark {

    constexpr Float::Float(Integer integer) : _value(static_cast<double>(integer)) { }

} // Spark
