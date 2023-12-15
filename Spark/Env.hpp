#pragma once

#include <cstddef>
#include <stdexcept>

#include "GC/GC.hpp"
#include "Value.hpp"

namespace Spark {

class Env {

    /* ===== Constructors ===== */

public:
    Env(size_t stackCapacity, size_t maxStackCapacity) : maxStackCapacity(maxStackCapacity) {
        stackBuffer = new Value[stackCapacity];
        stackPointer = basePointer = stackBuffer;
    }

    explicit Env(size_t stackCapacity) : Env(stackCapacity, stackCapacity) { }

    Env() : Env(256, 4096) { }



    /* ===== Destructor ===== */

public:
    ~Env() {
        delete stackBuffer;
    }



    /* ===== Stack ==== */

private:
    Value* stackBuffer;
    size_t maxStackCapacity;

public:
    void push(const Value& value) {
        // TODO: Implement
        throw std::runtime_error("Not implemented.");
    }

    Value top() {
        // TODO: Implement
        throw std::runtime_error("Not implemented.");
    }

    void pop() {
        // TODO: Implement
        throw std::runtime_error("Not implemented.");
    }



    /* ===== Registers ===== */

private:
    Value* stackPointer;
    Value* basePointer;
    std::byte* programCounter = nullptr;



    /* ===== Threads ===== */



    /* ===== GC ===== */

private:
    GC gc;

};

} // Spark
