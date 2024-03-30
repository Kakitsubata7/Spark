#pragma once

#include <forward_list>
#include <vector>

#include "GC/GC.hpp"
#include "Opcode.hpp"
#include "Types/Value.hpp"

namespace Spark {

class Thread {

    /* ===== Constructors & Destructor ===== */

public:
    Thread(size_t stackCapacity, size_t maxStackCapacity, GC& gc);
    Thread(size_t stackCapacity, GC& gc);
    explicit Thread(GC& gc);

    ~Thread();



    /* ===== Registers ===== */

private:
    Value* stackPointer;    // Points to the next available space on the stack.
    Value* basePointer;     // Points to the start of the current stack frame (start of arguments).
    Value* framePointer;    // Points to the start of operational stack.

    std::forward_list<ptrdiff_t> SPOffsets;
    std::forward_list<ptrdiff_t> BPOffsets;
    std::forward_list<ptrdiff_t> FPOffsets;
    std::forward_list<void*> previousPCs;

public:
    void* programCounter;   // Points to the next instruction to be executed.



    /* ===== Stack ===== */

private:
    Value* stackBuffer;
    size_t stackLength;
    size_t stackCapacity;
    size_t maxStackCapacity;

public:
    [[nodiscard]]
    constexpr size_t getStackLength() const {
        return stackLength;
    }

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

    constexpr void trimStackExcess() {
        resizeStack(stackLength);
    }

    std::vector<Value> stackToVector() {
        if (stackLength == 0)
            return {};

        std::vector<Value> vec(stackLength);
        Value* p = stackBuffer;
        for (int i = 0; p < stackPointer; i++, p++)
            vec[i] = *p;
        return vec;
    }



    /* ===== GC ===== */

private:
    GC& gc;



    /* ===== Operations ===== */

public:
    template <typename T>
    T fetch() {
        T* pc = static_cast<T*>(programCounter);
        T value = *pc;
        programCounter = static_cast<void*>(pc + 1);
        return value;
    }

    std::string fetchString(Int length) {
        // Create the string
        char* pc = static_cast<char*>(programCounter);
        std::string str(pc, length);

        // Update the programmer counter
        programCounter = static_cast<void*>(pc + length);

        return str;
    }

    /**
     * Execute one instruction.
     * @return Whether the thread is halted.
     */
    bool execute();

    /**
     * Push the value at the top of the stack. If the value is a reference type, it will be registered as an entry node
     * in the GC.
     * @param value Value to push.
     */
    void push(const Value& value);

    void pushArg(int index);

    [[nodiscard]]
    Value& getArg(int index);

    /**
     * Pop the value at the top of the stack. If the value is a reference type, its entry reference count in the GC will
     * be decreased. When reaches zero, the value is unregistered as a entry node.
     */
    void pop();

    /**
     * Pop n number of values at the top of the stack.
     * @param n Number of values to pop.
     */
    void pop(int n);

    /**
     * Pop the value at the top of the stack and return it.
     * @return Popped value.
     */
    Value popGet();

    /**
     * @return The value at the top of the stack.
     */
    [[nodiscard]] Value& top();

};

} // Spark
