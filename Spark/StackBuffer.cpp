#include "StackBuffer.hpp"

#include <cstring>

#include "GC/GC.hpp"

namespace Spark {

    /* ===== Constructor & Destructor ===== */

    StackBuffer::StackBuffer(GC& gc, size_t capacity, size_t maxCapacity) : gc(gc),
                                                                                   length(0),
                                                                                   capacity(capacity),
                                                                                   maxCapacity(maxCapacity) {
        buffer = new Value[capacity];
        basePointer = buffer;
        stackPointer = buffer;
    }

    StackBuffer::~StackBuffer() {
        // Unregister remaining reference type values from the GC
        Value* p = stackPointer--;
        while (p >= buffer) {
            const Value& value = *p;
            if (p->isReferenceType())
                gc.unregisterEntryNode(value.nodePtr);
            p--;
        }

        // Deallocate the buffer
        delete[] buffer;
    }



    /* ===== Operations ===== */

    void StackBuffer::resize(size_t newCapacity) {
        if (newCapacity < length)
            throw std::runtime_error("Stack capacity cannot be less than the current length.");

        // Allocate and initialize new buffer
        Value* newBuffer = new Value[newCapacity];
        std::memcpy(newBuffer, buffer, length);

        // Update registers
        basePointer = newBuffer + (basePointer - buffer);
        stackPointer = newBuffer + (stackPointer - buffer);

        // Deallocate previous buffer and set new buffer
        buffer = newBuffer;
        delete[] buffer;
    }

    void StackBuffer::push(const Value& value) {
        size_t newLength = length + 1;
        if (newLength > maxCapacity)
            throw std::runtime_error("Stack overflow.");
        else if (newLength > capacity) {
            // Calculate new capacity and resize the buffer
            size_t newCapacity = capacity * 2;
            if (newCapacity < newLength)
                newCapacity = newLength;
            resize(newCapacity);
        }

        // Push the value
        *stackPointer = value;

        // Update stack pointer
        stackPointer++;

        // Update length
        length = newLength;

        // Register reference type values as entry values for the GC
        if (value.isReferenceType())
            gc.registerEntryNode(value.nodePtr);
    }

    void StackBuffer::pop() {
        // Move stack pointer one value back
#ifdef NDEBUG
        stackPointer--;
#else
        Value* newSP = stackPointer - 1;
        if (newSP < basePointer)
            throw std::runtime_error("Stack underflow.");
        stackPointer = newSP;
#endif

        // Decrease the length
        length--;

        // Unregister the node as an entry node if the popped value is a reference type
        const Value& value = *stackPointer;
        if (value.isReferenceType())
            gc.unregisterEntryNode(value.nodePtr);
    }

    void StackBuffer::pop(int n) {
        for (int i = 0; i < n; i++)
            pop();
    }

    Value& StackBuffer::top() {
        Value* p = stackPointer - 1;
#ifndef NDEBUG
        if (p < basePointer)
            throw std::runtime_error("Stack underflow.");
#endif
        return *p;
    }

    Value& StackBuffer::top(Int64 offset) {
        Value* p = stackPointer - 1 + static_cast<ptrdiff_t>(offset);
#ifndef NDEBUG
        if (p < basePointer || p >= stackPointer)
            throw std::runtime_error("Invalid stack position.");
#endif
        return *p;
    }

    Value& StackBuffer::bottom() {
#ifndef NDEBUG
        if (basePointer == stackPointer)
            throw std::runtime_error("Stack underflow.");
#endif
        return *basePointer;
    }

    Value& StackBuffer::bottom(Int64 offset) {
        Value* p = basePointer + static_cast<ptrdiff_t>(offset);
#ifndef NDEBUG
        if (p < basePointer || p >= stackPointer)
            throw std::runtime_error("Invalid stack position.");
#endif
        return *p;
    }

    Value StackBuffer::popGet() {
        // Move stack pointer one value back
#ifdef NDEBUG
        stackPointer--;
#else
        Value* newSP = stackPointer - 1;
        if (newSP < basePointer)
            throw std::runtime_error("Stack underflow.");
        stackPointer = newSP;
#endif

        // Decrease the length
        length--;

        // Unregister the node as an entry node if the popped value is a reference type
        Value value = *stackPointer;
        if (value.isReferenceType())
            gc.unregisterEntryNode(value.nodePtr);

        return value;
    }

} // Spark
