#pragma once

#include <unordered_set>

#include "GC/GC.hpp"
#include "Thread.hpp"

namespace Spark {

class Env {

    /* ===== Constructor & Destructor ===== */

public:
    Env(size_t stackCapacity, size_t maxStackCapacity, bool useConcurrentGC = false);
    explicit Env(size_t stackCapacity, bool useConcurrentGC = false);
    explicit Env(bool useConcurrentGC = false);

    ~Env();



    /* ===== Threads ===== */

private:
    Thread* mainThread;
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
