#include "Thread.hpp"

#include <cstring>
#include <iomanip>
#include <sstream>
#include <stdexcept>

#include "Config.hpp"
#include "Types/Value.hpp"

namespace Spark {

    /* ===== Constructors & Destructor */

    Thread::Thread(size_t stackCapacity, size_t maxStackCapacity)
        : programCounter(nullptr),
          stackLength(0),
          stackCapacity(stackCapacity),
          maxStackCapacity(maxStackCapacity) {
        stackBuffer = new Value[stackCapacity];
        stackPointer = basePointer = stackBuffer;
    }

    Thread::Thread(size_t stackCapacity) : Thread(stackCapacity, stackCapacity) { }

    Thread::Thread() : Thread(Config::DEFAULT_STACK_CAPACITY) { }


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

            default:
                std::ostringstream ss;
                ss << "Invalid opcode: 0x"
                   << std::hex
                   << std::setw(2)
                   << std::setfill('0')
                   << static_cast<unsigned int>(opcode);
                throw std::runtime_error(ss.str());
        }

        return false;
    }

    void Thread::push(const Value& value) {
        stackLength++;
        *stackPointer = value;
        stackPointer++;
    }

    void Thread::pop(int count) {
        for (int i = 0; i < count; i++) {
            stackLength--;
#ifdef NDEBUG
            stackPointer--;
#else

#endif
        }
    }

} // Spark