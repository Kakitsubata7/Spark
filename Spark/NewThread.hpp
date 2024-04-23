#pragma once

#include <cstddef>

#include "Config.hpp"
#include "Opcode.hpp"
#include "Types/Int64.hpp"

namespace Spark {

class GCBase;
class Value;

class NewThread {

    /* ===== GC ===== */

private:
    GCBase& gc;



    /* ===== Registers ===== */

private:
    Value* basePointer;
    Value* stackPointer;

    void* programCounter = nullptr;



    /* ===== Stack ===== */

private:
    Value* stackBuffer;

    size_t stackCapacity;
    size_t stackMaxCapacity;

    size_t stackLength;



    /* ===== Constructor & Destructor ===== */

public:
    explicit NewThread(GCBase& gc,
                       size_t stackCapacity = DEFAULT_STACK_CAPACITY,
                       size_t maxStackCapacity = DEFAULT_MAX_STACK_CAPACITY);

    ~NewThread();



    /* ===== Operations ===== */

public:
    /**
     * Get a value from the current program counter, then update the program counter.
     * @tparam T Should be a fixed-sized value type.
     * @return Fetched value.
     */
    template <typename T>
    T fetch() {
        // Get the value
        T* pc = static_cast<T*>(programCounter);
        T value = *pc;

        // Increase program counter
        programCounter = static_cast<void*>(pc + 1);

        return value;
    }

    /**
     * Fetch a 64-bit integer as the length, then create and return a 'std::string' using the length to fetch characters.
     * @param length Length of the string (not NULL terminated).
     * @return Fetched string.
     */
    std::string fetchString(Int64 length);

    /**
     * Execute until halt or cadence.
     * @return Whether the thread is halted.
     */
    bool execute();

    /**
     * Resize the stack buffer.
     * @param newCapacity Capacity to resize to. It cannot be greater than the current stack max capacity or less than
     *                    the current stack length.
     * @throws std::runtime_error If new capacity is greater than the current stack max capacity or less than the
     *                            current stack length.
     */
    void resize(size_t newCapacity);

    /**
     * Push a value to the top of the stack.
     * @param value Value to push.
     * @throws std::runtime_error If stack overflow (new stack length is greater than the current max stack capacity).
     */
    void push(const Value& value);

    /**
     * Remove the value from the top of the stack.
     * @throws std::runtime_error If stack underflow (only in DEBUG).
     */
    void pop();

    /**
     * Remove n values from the top of the stack.
     * @param n Number of values to pop.
     * @throws std::runtime_error If stack underflow (only in DEBUG).
     */
    void pop(Int64 n);

};

} // Spark
