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

    Thread::Thread(GC& gc,
                   size_t opStackCapacity,
                   size_t maxOpStackCapacity,
                   size_t varStackCapacity,
                   size_t maxVarStackCapacity)
        : programCounter(nullptr),
          gc(gc),
          opStack(gc, opStackCapacity, maxOpStackCapacity),
          stStack(gc, varStackCapacity, maxVarStackCapacity) { }

    Thread::~Thread() = default;



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

            case Opcode::PushEmptySet:
                push(Value::makeSet(gc));
                break;

            case Opcode::PushEmptyMap:
                push(Value::makeMap(gc));
                break;

            case Opcode::Push:
                push(get(fetch<Int64>()));
                break;

            case Opcode::PushStorage:
                pushStorage(get(fetch<Int64>()));
                break;

            case Opcode::Pop:
                pop();
                break;

            case Opcode::PopStorage:
                popStorage();
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
                // Get callable index and number of arguments
                Int64 index = fetch<Int64>();
                Int64 narg = fetch<Int64>();

                // Get callable
                Value callable = get(index);
                if (!callable.isCallable()) {
                notCallableError:
                    std::ostringstream ss;
                    ss << "Type '";
                    ss << callable.type;
                    ss << "' is not callable.";
                    throw std::runtime_error(ss.str());
                }

                // Signal both stacks to start a call
                StackBuffer::startCall(opStack, stStack, narg);

                // Call the function/closure
                switch (callable.type) {
                    case Type::CFunction: {
                        Int returnCount = callable.cFuncPtr(this);
                        push(Value::makeInt(returnCount));
                        goto cFuncReturn;
                    }

                    case Type::Function: {
                        prevPCs.push_front(programCounter);
                        const Function& func = callable.nodePtr->getData<Function>();
                        programCounter = func.programCounter();
                    }
                        break;

                    case Type::Closure: {
                        prevPCs.push_front(programCounter);
                        const Closure& closure = callable.nodePtr->getData<Closure>();
                        programCounter = closure.programCounter();
                    }
                        break;

                    default:
                        goto notCallableError;
                }
            }
                break;

            case Opcode::Return: {
                // Restore PC
                programCounter = prevPCs.front();
                prevPCs.pop_front();

            cFuncReturn:
                // Get the number of returned values
                Int returnCount = top().intValue;

                // Signal both stacks to end call
                // One is added to return count because the number of returned values are also pushed onto the stack
                StackBuffer::endCall(opStack, stStack, returnCount + 1);
            }
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

        return false;
    }

    void Thread::push(const Value& value) {
        opStack.push(value);
    }

    void Thread::pushStorage(const Value& value) {
        stStack.push(value);
    }

    void Thread::pop() {
        opStack.pop();
    }

    void Thread::pop(int n) {
        opStack.pop(n);
    }

    void Thread::popStorage() {
        stStack.pop();
    }

    void Thread::popStorage(int n) {
        stStack.pop(n);
    }

    Value Thread::popGet() {
        return opStack.popGet();
    }

    Value& Thread::top() {
        return opStack.top();
    }

    Value& Thread::get(Int64 index) {
        if (index < 0)
            return opStack.top(index + 1);
        else if (index > 0)
            return stStack.bottom(index - 1);
        else
            return top();
    }

    Value& Thread::getArg(Int64 index) {
        return stStack.bottom(index - 1);
    }

} // Spark