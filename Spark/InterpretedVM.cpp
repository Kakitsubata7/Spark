#include "InterpretedVM.hpp"

#include <stdexcept>

#include "GC/ConcurrentGC.hpp"
#include "GC/SingleThreadedGC.hpp"
#include "NewThread.hpp"

namespace Spark {

    /* ===== Constructor ===== */

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
        mainThread = std::make_unique<NewThread>(*gc, mainThreadStackCapacity, mainThreadStackMaxCapacity);
    }

} // Spark
