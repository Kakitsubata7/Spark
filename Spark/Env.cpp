#include "Env.hpp"

#include "Config.hpp"

namespace Spark {

    /* ===== Constructor & Destructor ===== */

    Env::Env(size_t stackCapacity, size_t maxStackCapacity, bool useConcurrentGC) : useConcurrentGC(useConcurrentGC) {
        mainThread = new Thread(stackCapacity, maxStackCapacity);
    }

    Env::Env(size_t stackCapacity, bool useConcurrentGC) : Env(stackCapacity, stackCapacity, useConcurrentGC) { }

    Env::Env(bool useConcurrentGC) : Env(Config::DEFAULT_STACK_CAPACITY, useConcurrentGC) { }

    Env::~Env() {
        // Deallocate threads
        delete mainThread;
        for (Thread* thread : threadSet)
            delete thread;
    }



    /* ===== Operations ====== */

    void Env::run() {

        bool isFinished = false;

        while (!isFinished) {

            isFinished = true;
            
            if (!mainThread->execute())
                isFinished = false;

            for (Thread* thread : threadSet) {
                if (!thread->execute())
                    isFinished = false;
            }

            if (!useConcurrentGC)
                gc.step();
        }
    }

} // Spark
