#pragma once

#include <cstddef>
#include <cstdint>
#include <sstream>
#include <stdexcept>

#include "Config.hpp"
#include "Exception.hpp"
#include "Opcode.hpp"
#include "Value.hpp"

namespace Spark {

class Thread {

    /* ===== Constructors ===== */

public:
    Thread(size_t stackCapacity, size_t maxStackCapacity) : _stackCapacity(stackCapacity), _maxStackCapacity(maxStackCapacity) {
        stackBuffer = reinterpret_cast<Value*>(new std::byte[stackCapacity]);
        stackPointer = basePointer = stackBuffer;
    }

    explicit Thread(size_t stackCapacity) : Thread(stackCapacity, stackCapacity) { }

    Thread() : Thread(DEFAULT_STACK_CAPACITY, DEFAULT_MAX_STACK_CAPACITY) { }



    /* ===== Destructor ===== */

public:
    ~Thread() {
        delete[] reinterpret_cast<std::byte*>(stackBuffer);
    }



    /* ===== Stack ===== */

private:
    Value* stackBuffer;

    size_t _stackCapacity;
    size_t _maxStackCapacity;

    size_t _stackLength = 0;

public:
    [[nodiscard]]
    constexpr size_t stackCapacity() const {
        return _stackCapacity;
    }

    [[nodiscard]]
    constexpr size_t maxStackCapacity() const {
        return _maxStackCapacity;
    }

    [[nodiscard]]
    constexpr size_t stackLength() const {
        return _stackLength;
    }



    /* ===== Registers ===== */

private:
    Value* stackPointer;
    Value* basePointer;
    Opcode* programCounter = nullptr;

public:
    [[nodiscard]]
    constexpr Opcode* getProgramCounter() const {
        return programCounter;
    }

    constexpr void setProgramCounter(Opcode* programCounter) {
        this->programCounter = programCounter;
    }



    /* ===== Operations ===== */

public:
    void push(const Value& value) {
        // Check for growing the stack and stack overflow
        size_t newStackLength = _stackLength + sizeof(Value);
        if (newStackLength > _stackCapacity) {
            // Need to grow the stack
        } else if (newStackLength > _maxStackCapacity) {
            // Stack overflow
            throw Exception("Stack overflow.", "StackOverflowException");
        }

        // Push the value
        *stackPointer = value;

        // Update the stack pointer
        stackPointer++;

        // Update the stack length
        _stackLength = newStackLength;
    }

    [[nodiscard]]
    Value top() const {
        if (_stackLength == 0)
            throw Exception("Stack underflow.", "StackUnderflowException");
        return *stackPointer;
    }

    void pop() {
        if (_stackLength == 0)
            throw Exception("Stack underflow.", "StackUnderflowException");

        // Update the stack pointer to pop the value
        stackPointer--;

        // Update the stack length
        _stackLength--;
    }

    void execute() {
        if (programCounter == nullptr)
            throw std::runtime_error("The program counter is null.");

        Opcode opcode = *programCounter;
        programCounter++;
        switch (opcode) {
            case Opcode::Halt:
                return;

            case Opcode::PushNil:
                push(Value::makeNil());
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
                break;
        }
    }

    [[nodiscard]]
    std::string stackToString() const {
        std::ostringstream ss;
        ss << '[';
        const size_t count = _stackLength / sizeof(Value);
        for (size_t i = 0; i < count; i++) {
            ss << stackBuffer[i];
            if (i != (count - 1))
                ss << ", ";
        }
        ss << ']';
        return ss.str();
    }

};

} // Spark
