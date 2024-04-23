#pragma once

#include <memory>
#include <unordered_set>

#include "Config.hpp"
#include "GC/GCType.hpp"

namespace Spark {

class GCBase;
class NewThread;

class InterpretedVM {

    /* ===== Thread ===== */

private:
    std::unique_ptr<NewThread> mainThread;
    std::unordered_set<std::unique_ptr<NewThread>> threadSet;



    /* ===== GC ===== */

private:
    std::unique_ptr<GCBase> gc;



    /* ===== Constructor ===== */

public:
    explicit InterpretedVM(GCType gcType,
                           size_t mainThreadCapacity = DEFAULT_STACK_CAPACITY,
                           size_t mainThreadMaxCapacity = DEFAULT_STACK_MAX_CAPACITY);

};

} // Spark
