#include "Thread.hpp"

#include <cstdint>
#include <cstring>
#include <iomanip>
#include <sstream>
#include <stdexcept>

#include <iostream> // TODO: Remove this include

#include "Config.hpp"
#include "Types.hpp"

namespace Spark {

    /* ===== Constructors & Destructor */

    Thread::Thread(size_t stackCapacity, size_t maxStackCapacity, GC& gc)
        : programCounter(nullptr),
          stackLength(0),
          stackCapacity(stackCapacity),
          maxStackCapacity(maxStackCapacity),
          gc(gc) {
        stackBuffer = new Value[stackCapacity];
        stackPointer = basePointer = framePointer = stackBuffer;
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
                push(Value::makeInt(static_cast<Int>(fetch<Int64>())));
                break;

            case Opcode::PushFloat:
                push(Value::makeFloat(static_cast<Float>(fetch<Float64>())));
                break;

            case Opcode::PushBoolean:
                push(Value::makeBool(fetch<Bool>()));
                break;

            case Opcode::PushString:
                push(Value::makeString(gc, fetchString(static_cast<Int>(fetch<Int64>()))));
                break;

            case Opcode::PushEmptyArray:
                push(Value::makeArray(gc));
                break;

            case Opcode::Pop:
                pop();
                break;

            case Opcode::Add: {
                const Value b = popGet();
                const Value a = popGet();
                push(a + b);
            }
                break;

            case Opcode::Subtract: {
                const Value b = popGet();
                const Value a = popGet();
                push(a - b);
            }
                break;

            case Opcode::Multiply: {
                const Value b = popGet();
                const Value a = popGet();
                push(a * b);
            }
                break;

            case Opcode::Divide: {
                const Value b = popGet();
                const Value a = popGet();
                push(a / b);
            }
                break;

            case Opcode::Modulus: {
                const Value b = popGet();
                const Value a = popGet();
                push(a % b);
            }
                break;

            case Opcode::Equal: {
                const Value b = popGet();
                const Value a = popGet();
                push(Value::makeBool(a == b));
            }
                break;

            case Opcode::NotEqual: {
                const Value b = popGet();
                const Value a = popGet();
                push(Value::makeBool(a != b));
            }
                break;

            case Opcode::Call: {
                // Get callable
                Value& value = top();
                if (!value.isCallable()) {
                    std::ostringstream ss;
                    ss << value.type;
                    ss << " is not callable.";
                    throw std::runtime_error(ss.str());
                }

                // Get number of arguments and prepare stack registers
                Int64 narg = fetch<Int64>();
                SPOffsets.push_front(stackPointer - stackBuffer);
                BPOffsets.push_front(basePointer - stackBuffer);
                FPOffsets.push_front(framePointer - stackBuffer);
                basePointer = stackPointer - (static_cast<ptrdiff_t>(narg) + 1);
                framePointer = stackPointer;

                // Call the function/closure
                switch (value.type) {
                    case Type::CFunction: {
                        Int returnCount = value.cFuncPtr(this);
                        push(Value::makeInt(returnCount));
                        goto cFuncReturn;
                    }
                        break;

                    case Type::Function: {
                        previousPCs.push_front(programCounter);
                        const Function& func = value.nodePtr->getData<Function>();
                        programCounter = func.programCounter();
                    }
                        break;

                    case Type::Closure: {
                        previousPCs.push_front(programCounter);
                        const Closure& closure = value.nodePtr->getData<Closure>();
                        programCounter = closure.programCounter();
                    }
                        break;

                    default:
                        break;
                }
            }
                break;

            case Opcode::Return: {
                // Restore PC
                programCounter = previousPCs.front();
                previousPCs.pop_front();

            cFuncReturn:
                // Get the number of returned values
                Int returnCount = top().intValue;

                Value* newSP = stackBuffer + SPOffsets.front() + returnCount + 1;
                SPOffsets.pop_front();

                // Restore SP, BP and FP
                stackPointer = newSP;
                basePointer = stackBuffer + BPOffsets.front();
                BPOffsets.pop_front();
                framePointer = stackBuffer + FPOffsets.front();
                FPOffsets.pop_front();
            }
                break;

            default: {
                std::ostringstream ss;
                ss << "Invalid opcode: 0x"
                   << std::hex
                   << std::setw(2)
                   << std::setfill('0')
                   << static_cast<uintptr_t>(opcode);
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
            framePointer = newStackBuffer + (framePointer - stackBuffer);
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

    void Thread::pushArg(int index) {
        push(getArg(index));
    }

    Value& Thread::getArg(int index) {
#ifdef NDEBUG
        return *(basePointer + index);
#else
        Value* ptr = basePointer + index;
        if (ptr >= framePointer || index < 0)
            throw std::runtime_error("Index out of bounds.");
        return *ptr;
#endif
    }

    void Thread::pop() {
        // Move stack pointer one value back
#ifndef NDEBUG
        if ((stackPointer - 1) < framePointer)
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

    void Thread::pop(int n) {
        for (int i = 0; i < n; i++)
            pop();
    }

    Value Thread::popGet() {
        // Move stack pointer one value back
#ifndef NDEBUG
        if ((stackPointer - 1) < framePointer)
            throw std::runtime_error("Stack underflow.");
#endif
        stackPointer--;

        // Decrease stack length
        stackLength--;

        // Unregister the node as an entry node if the popped value is a reference type
        Value value = *stackPointer;
        if (value.isReferenceType())
            gc.unregisterEntryNode(value.nodePtr);

        return value;
    }

    Value& Thread::top() {
        Value* p = stackPointer - 1;
#ifndef NDEBUG
        if (p < framePointer)
            throw std::runtime_error("Stack underflow.");
#endif
        return *p;
    }

} // Spark