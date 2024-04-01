#include "Env.hpp"

#include "Config.hpp"

namespace Spark {

    /* ===== Constructor & Destructor ===== */

    Env::Env(size_t opStackCapacity,
             size_t maxOpStackCapacity,
             size_t varStackCapacity,
             size_t maxVarStackCapacity) {
        mainThread = new Thread(gc,
                                opStackCapacity,
                                maxOpStackCapacity,
                                varStackCapacity,
                                maxVarStackCapacity);
    }

    Env::~Env() {
        // Deallocate threads
        delete mainThread;
        delete asyncThread;
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
