#pragma once

#include <unordered_set>

#include "GC/GC.hpp"
#include "Thread.hpp"

#ifdef __cplusplus

namespace Spark {

class Env {

    /* ===== Threads ===== */

private:
    std::unordered_set<Thread*> threadSet;


    
    /* ===== GC ===== */

private:
    GC gc;

};

#else

#endif

} // Spark
