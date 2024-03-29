#include "Thread.hpp"

#include <cstring>
#include <iomanip>
#include <sstream>
#include <stdexcept>

#include <iostream> // TODO: Remove this include

#include "Config.hpp"
#include "Types/Value.hpp"

namespace Spark {

    /* ===== Constructors & Destructor */

    Thread::Thread(size_t stackCapacity, size_t maxStackCapacity, GC& gc)
        : programCounter(nullptr),
          stackLength(0),
          stackCapacity(stackCapacity),
          maxStackCapacity(maxStackCapacity),
          gc(gc) {
        stackBuffer = new Value[stackCapacity];
        stackPointer = basePointer = stackBuffer;
    }

    Thread::Thread(size_t stackCapacity, GC& gc) : Thread(stackCapacity, stackCapacity, gc) { }

    Thread::Thread(GC& gc) : Thread(Config::DEFAULT_STACK_CAPACITY, gc) { }


    Thread::~Thread() {
        delete stackBuffer;
    }



    /* ===== Stack ===== */

    void Thread::resizeStack(size_t stackCapacity) {
        if (stackCapacity < stackLength)
            throw std::runtime_error("Current stack capacity cannot be less than the current stack length.");

        // Allocate the new stack buffer
        this->stackCapacity = stackCapacity;
        Value* newStackBuffer = new Value[stackCapacity];

        // Copy the current stack to the new stack buffer
        std::memcpy(newStackBuffer, this->stackBuffer, stackLength);

        // Deallocate the previous stack buffer and reassign to the new one
        delete stackBuffer;
        stackBuffer = newStackBuffer;
    }

    void Thread::setMaxStackCapacity(size_t maxStackCapacity) {
        if (maxStackCapacity < stackLength)
            throw std::runtime_error("Max stack capacity cannot be less than the current stack length.");

        this->maxStackCapacity = maxStackCapacity;
    }



    /* ===== Operations ===== */

    bool Thread::execute() {
        // Ignore if the program counter is empty
        if (programCounter == nullptr)
            return true;

        Opcode opcode = fetch<Opcode>();

        switch (opcode) {
            case Opcode::Halt:
                return true;

            case Opcode::PushNil:
                push(Value::makeNil());
                break;

            case Opcode::PushInteger:
                push(Value::makeInt(fetch<Int>()));
                break;

            case Opcode::PushFloat:
                push(Value::makeFloat(fetch<Float>()));
                break;

            case Opcode::PushBoolean:
                push(Value::makeBool(fetch<Bool>()));
                break;

            case Opcode::PushString:
                push(Value::makeString(gc, fetchString(fetch<Int>())));
                break;

            case Opcode::Add: {
                const Value& b = popGet();
                const Value& a = popGet();
                push(a + b);
            }
                break;

            case Opcode::Subtract: {
                const Value& b = popGet();
                const Value& a = popGet();
                push(a - b);
            }
                break;

            case Opcode::Multiply: {
                const Value& b = popGet();
                const Value& a = popGet();
                push(a * b);
            }
                break;

            case Opcode::Divide: {
                const Value& b = popGet();
                const Value& a = popGet();
                push(a / b);
            }
                break;

            case Opcode::Modulus: {
                const Value& b = popGet();
                const Value& a = popGet();
                push(a % b);
            }
                break;

            case Opcode::Pop:
                pop(1);
                break;

            default: {
                std::ostringstream ss;
                ss << "Invalid opcode: 0x"
                   << std::hex
                   << std::setw(2)
                   << std::setfill('0')
                   << static_cast<unsigned int>(opcode);
                throw std::runtime_error(ss.str());
            }
        }

        return false;
    }

    void Thread::push(const Value& value) {
#ifndef NDEBUG
        size_t newStackLength = stackLength + 1;
        if (newStackLength > maxStackCapacity)
            throw std::runtime_error("Stack overflow.");
        else if (newStackLength > stackCapacity) {
            // Calculate new stack capacity
            size_t newStackCapacity = stackCapacity * 2;
            if (newStackCapacity < newStackLength)
                newStackCapacity = newStackLength;

            // Allocate new stack buffer
            Value* newStackBuffer = new Value[newStackCapacity];
            std::memcpy(newStackBuffer, stackBuffer, stackLength);

            // Update stack registers and stack buffer
            stackPointer = newStackBuffer + (stackPointer - stackBuffer);
            basePointer = newStackBuffer + (basePointer - stackBuffer);
            delete[] stackBuffer;
            stackBuffer = newStackBuffer;
        }
#endif
        stackLength++;

        *stackPointer = value;
        stackPointer++;

        if (value.isReferenceType())
            gc.registerEntryNode(value.nodePtr);
    }

    void Thread::pop() {
        // Move stack pointer one value back
#ifndef NDEBUG
        if ((stackPointer - 1) < basePointer)
            throw std::runtime_error("Stack underflow.");
#endif
        stackPointer--;

        // Decrease stack length
        stackLength--;

        // Unregister the node as an entry node if the popped value is a reference type
        const Value& value = *stackPointer;
        if (value.isReferenceType())
            gc.unregisterEntryNode(value.nodePtr);
    }

    void Thread::pop(int count) {
        for (int i = 0; i < count; i++)
            pop();
    }

    Value Thread::popGet() {
        // Move stack pointer one value back
#ifndef NDEBUG
        if ((stackPointer - 1) < basePointer)
            throw std::runtime_error("Stack underflow.");
#endif
        stackPointer--;

        // Decrease stack length
        stackLength--;

        // Unregister the node as an entry node if the popped value is a reference type
        const Value& value = *stackPointer;
        if (value.isReferenceType())
            gc.unregisterEntryNode(value.nodePtr);

        return value;
    }

    Value& Thread::top() {
        Value* p = stackPointer - 1;
#ifndef NDEBUG
        if (p < basePointer)
            throw std::runtime_error("Stack underflow.");
#endif
        return *p;
    }

} // Spark