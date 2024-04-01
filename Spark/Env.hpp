#pragma once

#include <unordered_set>

#include "GC/GC.hpp"
#include "Thread.hpp"

namespace Spark {

class Env {

    /* ===== Constructor & Destructor ===== */

public:
    explicit Env(size_t opStackCapacity = DEFAULT_OPERATION_STACK_CAPACITY,
                 size_t maxOpStackCapacity = DEFAULT_MAX_OPERATION_STACK_CAPACITY,
                 size_t varStackCapacity = DEFAULT_VARIABLE_STACK_CAPACITY,
                 size_t maxVarStackCapacity = DEFAULT_MAX_VARIABLE_STACK_CAPACITY);

    ~Env();



    /* ===== Threads ===== */

private:
    Thread* mainThread;
    Thread* asyncThread;
    std::unordered_set<Thread*> threadSet;


    
    /* ===== GC ===== */

private:
    GC gc;
    bool useConcurrentGC;



    /* ===== Operations ===== */

public:
    void run();

};

} // Spark
