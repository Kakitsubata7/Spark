#pragma once

#include <forward_list>
#include <vector>

#include "Config.hpp"
#include "GC/GC.hpp"
#include "Opcode.hpp"
#include "StackBuffer.hpp"
#include "Types/Value.hpp"

namespace Spark {

class Thread {

    /* ===== Constructors & Destructor ===== */

public:
    explicit Thread(GC& gc,
           size_t opStackCapacity = DEFAULT_OPERATION_STACK_CAPACITY,
           size_t maxOpStackCapacity = DEFAULT_MAX_OPERATION_STACK_CAPACITY,
           size_t varStackCapacity = DEFAULT_VARIABLE_STACK_CAPACITY,
           size_t maxVarStackCapacity = DEFAULT_MAX_VARIABLE_STACK_CAPACITY);

    ~Thread();



    /* ===== Registers ===== */

private:
    std::forward_list<void*> prevPCs;

public:
    void* programCounter;   // Points to the next instruction to be executed.



    /* ===== Stack ===== */

private:
    StackBuffer opStack;    // Operation stack
    StackBuffer stStack;    // Storage stack

public:
    std::vector<std::reference_wrapper<const Value>> operationStackToVector() {
        return opStack.toVector();
    }

    std::vector<std::reference_wrapper<const Value>> storageStackToVector() {
        return stStack.toVector();
    }



    /* ===== GC ===== */

private:
    GC& gc;



    /* ===== Operations ===== */

public:
    template <typename T>
    T fetch() {
        T* pc = static_cast<T*>(programCounter);
        T value = *pc;
        programCounter = static_cast<void*>(pc + 1);
        return value;
    }

    std::string fetchString(Int length) {
        // Create the string
        char* pc = static_cast<char*>(programCounter);
        std::string str(pc, length);

        // Update the programmer counter
        programCounter = static_cast<void*>(pc + length);

        return str;
    }

    [[nodiscard]] static Value makeNil() { return Value::makeNil(); }
    [[nodiscard]] static Value makeInt(Int i) { return Value::makeInt(i); }
    [[nodiscard]] static Value makeFloat(Float f) { return Value::makeFloat(f); }
    [[nodiscard]] static Value makeBool(Bool b) { return Value::makeBool(b); }
    [[nodiscard]] static Value makeCFunc(CFunction cFunc) { return Value::makeCFunction(cFunc); }
    [[nodiscard]] static Value makeType(Type type) { return Value::makeType(type); }
    [[nodiscard]] Value makeString(const std::string& str) const { return Value::makeString(gc, str); }
    [[nodiscard]] Value makeArray(const std::vector<Value>& vec) const { return Value::makeArray(gc, vec); }

    /**
     * Execute one instruction.
     * @return Whether the thread is halted.
     */
    bool execute();

    void push(const Value& value);
    void pushStorage(const Value& value);

    void pop();
    void pop(int n);

    void popStorage();
    void popStorage(int n);

    Value popGet();

    Value& top();

    Value& get(Int64 index);
    Value& getArg(Int64 index);

};

} // Spark
