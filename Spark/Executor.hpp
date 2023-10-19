#pragma once

#include "Types.hpp"

namespace Spark {

    class Executor {

        /* ===== Registers ===== */

    private:
        void* programCounter;
        void* stackPointer;
        void* basePointer;

    public:
        [[nodiscard]]
        constexpr void* getProgramCounter() const {
            return programCounter;
        }

        constexpr void setProgramCounter(void* programCounter) {
            this->programCounter = programCounter;
        }



        /* ===== Operations ===== */

    public:
        template <typename T>
        void push(Type type, T value) {

        }



        /* ===== Execution ===== */

    public:
        bool execute();

    };

} // Spark
