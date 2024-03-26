#pragma once

#include "Opcode.hpp"
#include "Types/Value.hpp"

namespace Spark {

class Thread {

    /* ===== Constructors & Destructor ===== */

public:
    Thread(size_t stackCapacity, size_t maxStackCapacity);
    explicit Thread(size_t stackCapacity);
    Thread();

    ~Thread();



    /* ===== Registers ===== */

private:
    Value* stackPointer;
    Value* basePointer;

public:
    Opcode* programCounter;



    /* ===== Stack ===== */

private:
    Value* stackBuffer;
    size_t stackLength;
    size_t stackCapacity;
    size_t maxStackCapacity;

public:
    [[nodiscard]]
    constexpr size_t getStackCapacity() const {
        return stackCapacity;
    }

    void resizeStack(size_t stackCapacity);

    [[nodiscard]]
    constexpr size_t getMaxStackCapacity() const {
        return maxStackCapacity;
    }

    void setMaxStackCapacity(size_t maxStackCapacity);



    /* ===== Operations ===== */

public:
    template <typename T>
    T fetch() {
        T* pc = reinterpret_cast<T*>(programCounter);
        T value = *pc;
        programCounter = reinterpret_cast<Opcode*>(pc + 1);
        return value;
    }

    bool execute();

    void push(const Value& value);
    void pop(int count);

};

} // Spark
