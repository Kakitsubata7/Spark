#pragma once

#include <ostream>

namespace Spark {

enum class Type {
    Nil,
    Int,
    Float,
    Bool,
    String
};

std::ostream& operator<<(std::ostream& os, Type type);

} // Spark
