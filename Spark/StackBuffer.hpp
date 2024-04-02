#pragma once

#include <cstddef>
#include <forward_list>

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

    Value* basePointer;
    Value* stackPointer;

    std::forward_list<ptrdiff_t> prevBPOffset;



    /* ===== Constructor & Destructor ===== */

public:
    StackBuffer(GC& gc, size_t capacity, size_t maxCapacity);

    ~StackBuffer();



    /* ===== Operations ===== */

public:
    void resize(size_t newCapacity);

    void push(const Value& value);

    void pop();
    void pop(Int64 n);
    [[nodiscard]] Value popGet();

    [[nodiscard]] Value& top();
    [[nodiscard]] Value& top(Int64 offset);

    [[nodiscard]] Value& bottom();
    [[nodiscard]] Value& bottom(Int64 offset);

    std::vector<std::reference_wrapper<const Value>> toVector();

    static void move(StackBuffer& from, StackBuffer& to, Int64 n);

    static void startCall(StackBuffer& opStack, StackBuffer& stStack, Int64 narg);
    static void endCall(StackBuffer& opStack, StackBuffer& stStack, Int64 nreturn);

private:
    void openFrame();

};

} // Spark
