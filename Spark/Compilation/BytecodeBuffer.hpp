#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <stdexcept>

#include "../Opcode.hpp"

namespace Spark {

class BytecodeBuffer {

    /* ===== Constructor & Destructor ===== */

public:
    explicit BytecodeBuffer(size_t capacity = 10) : _size(0), _capacity(capacity) {
        buffer = new uint8_t[capacity];
    }

    ~BytecodeBuffer() {
        delete[] static_cast<uint8_t*>(buffer);
    }



    /* ===== Data ===== */

private:
    void* buffer;
    size_t _size;
    size_t _capacity;

public:
    [[nodiscard]]
    constexpr size_t size() const {
        return _size;
    }

    [[nodiscard]]
    constexpr size_t capacity() const {
        return _capacity;
    }



    /* ===== Operation ===== */

public:
    template <typename T>
    void append(const T& value) {
        size_t newSize = _size + sizeof(T);

        // Resize if needed
        if (newSize > _capacity) {
            size_t newCapacity = _capacity * 2;
            if (newCapacity < newSize)
                newCapacity = newSize;
            resize(newCapacity);
        }

        // Append the value
        T* ptr = reinterpret_cast<T*>(static_cast<uint8_t*>(buffer) + _size);
        *ptr = value;

        // Update size
        _size = newSize;
    }

    void appendString(const std::string& str) {
        size_t strSize = str.length() * sizeof(char);
        size_t newSize = _size + strSize;

        // Resize if needed
        if (newSize > _capacity) {
            size_t newCapacity = _capacity * 2;
            if (newCapacity < newSize)
                newCapacity = newSize;
            resize(newCapacity);
        }

        // Append the string
        std::memcpy(static_cast<char*>(buffer) + _size, str.c_str(), strSize);

        // Update size
        _size = newSize;
    }

    void resize(size_t newCapacity) {
        if (newCapacity < _size)
            throw std::runtime_error("New capacity cannot be less than the current buffer size.");

        // Reallocate the buffer
        void* newBuffer = new uint8_t[newCapacity];
        std::memcpy(newBuffer, buffer, _size);
        delete[] static_cast<uint8_t*>(buffer);
        buffer = newBuffer;

        // Update capacity
        _capacity = newCapacity;
    }

    [[nodiscard]]
    constexpr Opcode* getOpcode() {
        return static_cast<Opcode*>(buffer);
    }

};

} // Spark
