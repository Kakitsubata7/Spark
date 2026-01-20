#pragma once

#include <cstdint>

#include "core/type.hpp"

namespace Spark::Runtime {

/**
 * Integer type used for reference counting.
 */
using RCInt = uintptr_t;

/**
 * Represents the header of a Spark object that uses reference counting.
 */
struct RCHeader {
    RCInt refCount;
    const Type* type;
};

} // Spark::Runtime
