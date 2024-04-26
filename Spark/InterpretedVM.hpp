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
    NewThread* mainThread;
    std::unordered_set<NewThread*> threadSet;

public:
    [[nodiscard]]
    NewThread& createThread(size_t stackCapacity = DEFAULT_STACK_CAPACITY,
                            size_t stackMaxCapacity = DEFAULT_STACK_MAX_CAPACITY);

    void deleteThread(const NewThread& thread);



    /* ===== GC ===== */

private:
    std::unique_ptr<GCBase> gc;



    /* ===== Constructor & Destructor ===== */

public:
    explicit InterpretedVM(GCType gcType = GCType::SingleThreaded,
                           size_t mainThreadCapacity = DEFAULT_STACK_CAPACITY,
                           size_t mainThreadMaxCapacity = DEFAULT_STACK_MAX_CAPACITY);

    ~InterpretedVM();



    /* ===== Execution ===== */

public:
    void run();

};

} // Spark
