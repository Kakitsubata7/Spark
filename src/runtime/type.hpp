#pragma once

#include <cstddef>

namespace Spark::Runtime {

/**
 * Represents a Spark runtime type object.
 */
class Type {
private:
    using Destructor = void (*)(void*);

    std::size_t _size;

    Destructor _destructor = nullptr;

public:
    explicit Type(std::size_t size, Destructor destructor = nullptr) : _size(size), _destructor(destructor) { }

    Type(const Type& other) = delete;
    Type& operator=(const Type& other) = delete;

    Type(Type&& other) noexcept = default;
    Type& operator=(Type&& other) noexcept = default;

    /**
     * Calls the destructor of the type (if exist).
     *
     * @param p Pointer to the Spark object to destruct.
     */
    void destruct(void* p) const {
        if (_destructor != nullptr) {
            _destructor(p);
        }
    }

    /**
     * Size of the type in bytes for the current platform.
     */
    [[nodiscard]]
    constexpr std::size_t size() const {
        return _size;
    }
};

} // Spark::Runtime
