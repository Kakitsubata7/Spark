#include "Float.hpp"

namespace Spark {

    constexpr Integer::Integer(Float f) : _value(static_cast<int64_t>(f)) { }

} // Spark
