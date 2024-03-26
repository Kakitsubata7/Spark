#include "Env.hpp"

#include "Config.hpp"

namespace Spark {

    /* ===== Constructor & Destructor ===== */

    Env::Env(size_t stackCapacity, size_t maxStackCapacity) {
        mainThread = new Thread(stackCapacity, maxStackCapacity);
    }

    Env::Env(size_t stackCapacity) : Env(stackCapacity, stackCapacity) { }

    Env::Env() : Env(Config::DEFAULT_STACK_CAPACITY) { }

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
        }
    }

} // Spark
