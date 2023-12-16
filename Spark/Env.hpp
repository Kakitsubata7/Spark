#pragma once

#include <cstddef>
#include <memory>
#include <stdexcept>
#include <unordered_set>

#include "Config.hpp"
#include "GC/GC.hpp"
#include "Thread.hpp"
#include "Value.hpp"

namespace Spark {

class Env {

    /* ===== Constructors ===== */

public:
    Env(size_t mainThreadStackCapacity, size_t mainThreadMaxStackCapacity) {
        mainThread = Thread(mainThreadStackCapacity, mainThreadMaxStackCapacity);
    }

    explicit Env(size_t mainThreadStackCapacity) : Env(mainThreadStackCapacity, mainThreadStackCapacity) { }

    Env() : Env(DEFAULT_STACK_CAPACITY, DEFAULT_MAX_STACK_CAPACITY) { }



    /* ===== Threads ===== */

private:
    Thread mainThread;
    std::unordered_set<std::unique_ptr<Thread>> threadSet;



    /* ===== GC ===== */

private:
    GC gc;

};

} // Spark
