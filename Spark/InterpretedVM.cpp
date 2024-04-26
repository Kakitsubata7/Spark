#include "InterpretedVM.hpp"

#include <stdexcept>

#include "GC/ConcurrentGC.hpp"
#include "GC/SingleThreadedGC.hpp"
#include "NewThread.hpp"

namespace Spark {

    /* ===== Thread ===== */

    NewThread& InterpretedVM::createThread(size_t stackCapacity, size_t stackMaxCapacity) {
        NewThread* thread = new NewThread(*gc, stackCapacity, stackMaxCapacity);
        threadSet.insert(thread);
        return *thread;
    }

    void InterpretedVM::deleteThread(const NewThread& thread) {
        threadSet.erase(const_cast<NewThread*>(&thread));
    }



    /* ===== Constructor & Destructor ===== */

    InterpretedVM::InterpretedVM(GCType gcType, size_t mainThreadStackCapacity, size_t mainThreadStackMaxCapacity) {
        // Create GC
        switch (gcType) {
            case GCType::SingleThreaded:
                gc = std::make_unique<SingleThreadedGC>();
                break;

            case GCType::Concurrent:
                gc = std::make_unique<ConcurrentGC>();
                break;

            default:
                throw std::runtime_error("GC type is not implemented.");
        }

        // Create main thread
        mainThread = &(createThread(mainThreadStackCapacity, mainThreadStackMaxCapacity));
    }

    InterpretedVM::~InterpretedVM() {
        // Deallocate threads
        for (NewThread* thread : threadSet)
            delete thread;
    }



    /* ===== Execution ===== */

    void InterpretedVM::run() {
        bool isHalted = false;
        while (!isHalted) {
            isHalted = true;

            for (NewThread* thread : threadSet) {
                if (!thread->execute())
                    isHalted = false;
            }

            gc->step();
        }
    }

} // Spark
