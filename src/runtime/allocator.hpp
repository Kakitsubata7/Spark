#pragma once

#include <cstdlib>

namespace Spark::Runtime {

/**
 * Default allocator used by Spark.
 * Uses std::malloc and std::free.
 * @see std::malloc
 * @see std::free
 */
class DefaultAllocator {
    void* alloc(std::size_t size) noexcept {
        return std::malloc(size);
    }

    void free(void* ptr) noexcept {
        std::free(ptr);
    }
};

} // Spark::Runtime
