#include "StackBuffer.hpp"

#include <cstring>

#include <iostream> // TODO: Delete this include

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
        Value* p = stackPointer - 1;
        while (p >= buffer) {
            const Value& value = *p;
            if (value.isReferenceType())
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
        std::memcpy(newBuffer, buffer, length * sizeof(Value));

        // Update registers
        basePointer = newBuffer + (basePointer - buffer);
        stackPointer = newBuffer + (stackPointer - buffer);

        // Deallocate previous buffer and set new buffer
        delete[] buffer;
        buffer = newBuffer;
    }

    void StackBuffer::push(const Value& value) {
        size_t newLength = length + 1;
        if (newLength > maxCapacity)
            throw std::runtime_error("Stack overflow.");
        else if (newLength > capacity) {
            // Calculate new capacity and resize the buffer
            size_t newCapacity = capacity * 2;
            if (newCapacity > maxCapacity)
                newCapacity = maxCapacity;
            else if (newCapacity < newLength)
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

    void StackBuffer::pop(Int64 n) {
#ifndef NDEBUG
        if (n < 0)
            throw std::runtime_error("Pop number cannot be negative.");
#endif
        if (n == 0)
            return;

        // Move stack pointer n values back
        Value* targetSP = stackPointer - static_cast<ptrdiff_t>(n);
#ifndef NDEBUG
        if (targetSP < basePointer)
            throw std::runtime_error("Stack underflow.");
#endif

        // Decrease the length
        length -= static_cast<size_t>(n);

        // Unregister nodes
        while (stackPointer > targetSP) {
            stackPointer--;
            const Value& value = *stackPointer;
            if (value.isReferenceType())
                gc.unregisterEntryNode(value.nodePtr);
        }
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

    void StackBuffer::move(StackBuffer& from, StackBuffer& to, Int64 n) {
        // Move back source stack pointer and decrease stack length
        from.stackPointer -= n;
#ifndef NDEBUG
        if (from.stackPointer < from.basePointer)
            throw std::runtime_error("Stack underflow.");
#endif
        from.length -= n;

        // Make sure destination stack have enough capacity
        size_t newLen = to.length + n;
        if (newLen > to.maxCapacity)
            throw std::runtime_error("Stack overflow.");
        if (newLen > to.capacity) {
            size_t newCapacity = to.capacity * 2;
            if (newCapacity < newLen)
                newCapacity = newLen;
            to.resize(newCapacity);
        }

        // Copy values from source to destination
        std::memcpy(to.stackPointer, from.stackPointer, n * sizeof(Value));

        // Move forward destination stack pointer and increase stack length
        to.stackPointer += n;
        to.length += newLen;
    }

    void StackBuffer::startCall(StackBuffer& opStack, StackBuffer& stStack, Int64 narg) {
        // Open a new storage stack frame
        stStack.openFrame();

        // Move arguments from operation stack to storage stack
        move(opStack, stStack, narg);

        // Open a new operation stack frame
        opStack.openFrame();
    }

    void StackBuffer::endCall(StackBuffer& opStack, StackBuffer& stStack, Int64 nreturn) {
        // Pop all values in the current storage stack frame, then resume the base pointer
        stStack.pop(static_cast<Int64>(stStack.stackPointer - stStack.basePointer));
        stStack.basePointer = stStack.buffer + stStack.prevBPOffset.front();
        stStack.prevBPOffset.pop_front();

        // Record the pointer of the first return value
        Value* returnBegin = opStack.stackPointer - static_cast<ptrdiff_t>(nreturn);
#ifndef NDEBUG
        if (returnBegin < opStack.basePointer)
            throw std::runtime_error("Stack underflow.");
#endif

        // Pop all values in the current operation stack frame except for the return values
        opStack.stackPointer = returnBegin;
        opStack.pop(static_cast<Int64>(opStack.stackPointer - opStack.basePointer));

        // Move the return values back, then update operation stack pointer
        std::memcpy(opStack.basePointer, returnBegin, nreturn * sizeof(Value));
        opStack.stackPointer += nreturn;

        // Resume operation stack base pointer
        opStack.basePointer = opStack.buffer + opStack.prevBPOffset.front();
        opStack.prevBPOffset.pop_front();
    }

    void StackBuffer::openFrame() {
        // Record previous base pointer offset
        prevBPOffset.push_front(basePointer - buffer);

        // Update base pointer
        basePointer = stackPointer;
    }

    std::vector<std::reference_wrapper<const Value>> StackBuffer::toVector() {
        if (length == 0)
            return {};

        std::vector<std::reference_wrapper<const Value>> vec;
        vec.reserve(length);
        Value* p = buffer;
        for (int i = 0; p < stackPointer; i++, p++)
            vec.emplace_back(*p);
        return vec;
    }

} // Spark
