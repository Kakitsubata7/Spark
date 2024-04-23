#include "NewThread.hpp"

#include <iomanip>
#include <sstream>

#include "GC/GCBase.hpp"
#include "Types/Value.hpp"

namespace Spark {

    /* ===== Constructor & Destructor ===== */

    NewThread::NewThread(GCBase& gc, size_t stackCapacity, size_t maxStackCapacity)
        : gc(gc), stackCapacity(stackCapacity), maxStackCapacity(maxStackCapacity), stackLength(0) {
        // Allocate stack buffer and initialize stack registers
        stackBuffer = new Value[stackCapacity];
        basePointer = stackPointer = stackBuffer;
    }

    NewThread::~NewThread() {
        // Deallocate stack buffer
        delete[] stackBuffer;
    }



    /* ===== Operations ===== */

    std::string NewThread::fetchString(Int64 length) {
        // Create the string
        char* pc = static_cast<char*>(programCounter);
        std::string str(pc, length);

        // Update programmer counter
        programCounter = static_cast<void*>(pc + length);

        return str;
    }

    bool NewThread::execute() {
        // A thread is considered as halted if its program counter is null
        if (programCounter == nullptr)
            return true;

        while (true) {
            Opcode opcode = fetch<Opcode>();

            switch (opcode) {
                case Opcode::Halt:
                    return true;

                case Opcode::Cadence:
                    return false;

                case Opcode::PushNil:
                    push(Value::makeNil());
                    break;

                default: {
                    std::ostringstream ss;
                    ss << "Invalid opcode: 0x"
                       << std::hex
                       << std::setw(2)
                       << std::setfill('0')
                       << static_cast<uintptr_t>(opcode)
                       << std::dec;
                    throw std::runtime_error(ss.str());
                }
            }
        }
    }

    void NewThread::resize(size_t newCapacity) {
        if (newCapacity < stackLength)
            throw std::runtime_error("Stack capacity cannot be less than the current length.");
        if (newCapacity > maxStackCapacity)
            throw std::runtime_error("Stack capacity cannot be greater than the current max stack capacity.");

        // Allocate new buffer, then copy data to it
        Value* newBuffer = new Value[newCapacity];
        std::memcpy(newBuffer, stackBuffer, stackLength * sizeof(Value));

        // Update registers
        basePointer = newBuffer + (basePointer - stackBuffer);
        stackPointer = newBuffer + (stackPointer - stackBuffer);

        // Deallocate previous buffer and set new buffer as stack buffer
        delete[] stackBuffer;
        stackBuffer = newBuffer;
    }

    void NewThread::push(const Value& value) {
        size_t newLength = stackLength + 1;
        if (newLength > maxStackCapacity)
            throw std::runtime_error("Stack overflow.");
        if (newLength > stackCapacity) {
            // Calculate new capacity and resize the buffer
            size_t newCapacity = stackCapacity * 2;
            if (newCapacity > maxStackCapacity)
                newCapacity = maxStackCapacity;
            else if (newCapacity < newLength)
                newCapacity = newLength;
            resize(newCapacity);
        }

        // Push the value and update stack pointer
        *stackPointer = value;
        stackPointer++;

        // Update stack length
        stackLength = newLength;

        // Add node as a GC entry if it's a reference type
        if (value.isReferenceType())
            gc.addEntryNode(value.nodePtr);
    }

    void NewThread::pop() {
#ifdef NDEBUG
        stackPointer--;
#else
        Value* newStackPointer = stackPointer - 1;
        if (newStackPointer < basePointer)
            throw std::runtime_error("Stack underflow.");
        stackPointer = newStackPointer;
#endif

        // Decrease stack length
        stackLength--;

        // Remove it from GC entry if it's a reference type
        const Value& value = *stackPointer;
        if (value.isReferenceType())
            gc.removeEntryNode(value.nodePtr);
    }

    void NewThread::pop(Int64 n) {
#ifndef NDEBUG
        if (n < 0)
            throw std::runtime_error("Number of values to pop cannot be negative.");
#endif
        if (n == 0)
            return;

        // Move stack pointer n values back
        Value* targetStackPointer = stackPointer - static_cast<ptrdiff_t>(n);
#ifndef NDEBUG
        if (targetStackPointer < basePointer)
            throw std::runtime_error("Stack underflow.");
#endif

        // Decrease stack length
        stackLength -= static_cast<size_t>(n);

        // Remove reference values from GC entry
        while (stackPointer > targetStackPointer) {
            stackPointer--;
            const Value& value = *stackPointer;
            if (value.isReferenceType())
                gc.removeEntryNode(value.nodePtr);
        }
    }

} // Spark
