#pragma once

#include <cstddef>

#include "Types/Value.hpp"

namespace Spark {

    constexpr size_t DEFAULT_OPERATION_STACK_CAPACITY = 256 * sizeof(Value);
    constexpr size_t DEFAULT_MAX_OPERATION_STACK_CAPACITY = 131072 * sizeof(Value);

    constexpr size_t DEFAULT_VARIABLE_STACK_CAPACITY = 256 * sizeof(Value);
    constexpr size_t DEFAULT_MAX_VARIABLE_STACK_CAPACITY = 131072 * sizeof(Value);

    constexpr size_t DEFAULT_STACK_CAPACITY = 256;
    constexpr size_t DEFAULT_MAX_STACK_CAPACITY = 131072;

} // Spark
