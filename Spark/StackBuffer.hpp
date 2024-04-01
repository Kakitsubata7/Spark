#pragma once

#include <cstddef>

#include "Types/Int64.hpp"
#include "Types/Value.hpp"

namespace Spark {

class GC;

class StackBuffer {

    /* ===== Data ===== */

private:
    GC& gc;
    Value* buffer;
    size_t length;
    size_t capacity;
    size_t maxCapacity;

public:
    Value* basePointer;
    Value* stackPointer;



    /* ===== Constructor & Destructor ===== */

public:
    StackBuffer(GC& gc, size_t capacity, size_t maxCapacity);

    ~StackBuffer();



    /* ===== Operations ===== */

public:
    void resize(size_t newCapacity);

    void push(const Value& value);

    void pop();
    void pop(int n);
    [[nodiscard]] Value popGet();

    [[nodiscard]] Value& top();
    [[nodiscard]] Value& top(Int64 offset);

    [[nodiscard]] Value& bottom();
    [[nodiscard]] Value& bottom(Int64 offset);

};

} // Spark
