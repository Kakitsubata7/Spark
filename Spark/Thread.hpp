#pragma once

#include <cstddef>
#include <stdexcept>

#include "Config.hpp"
#include "Opcode.hpp"
#include "Value.hpp"

namespace Spark {

class Thread {

    /* ===== Constructors ===== */

public:
    Thread(size_t stackCapacity, size_t maxStackCapacity) : _stackCapacity(stackCapacity), _maxStackCapacity(maxStackCapacity) {
        stackBuffer = reinterpret_cast<Value*>(new std::byte[stackCapacity]);
        stackPointer = basePointer = stackBuffer;
    }

    explicit Thread(size_t stackCapacity) : Thread(stackCapacity, stackCapacity) { }

    Thread() : Thread(DEFAULT_STACK_CAPACITY, DEFAULT_MAX_STACK_CAPACITY) { }



    /* ===== Destructor ===== */

public:
    ~Thread() {
        delete[] reinterpret_cast<std::byte*>(stackBuffer);
    }



    /* ===== Stack ===== */

private:
    Value* stackBuffer;

    size_t _stackCapacity;
    size_t _maxStackCapacity;

public:
    [[nodiscard]]
    constexpr size_t stackCapacity() const {
        return _stackCapacity;
    }

    [[nodiscard]]
    constexpr size_t maxStackCapacity() const {
        return _maxStackCapacity;
    }



    /* ===== Registers ===== */

private:
    Value* stackPointer;
    Value* basePointer;
    Opcode* programCounter = nullptr;



    /* ===== Operations ===== */

public:
    void push(const Value& value) {
        // TODO: Implement
        throw std::runtime_error("Not implemented.");
    }

    [[nodiscard]]
    Value top() const {
        // TODO: Implement
        throw std::runtime_error("Not implemented.");
    }

    void pop() {
        // TODO: Implement
        throw std::runtime_error("Not implemented.");
    }

};

} // Spark
