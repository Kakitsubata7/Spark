#pragma once

#include <cstring>
#include <string>

#include "../Opcode.hpp"
#include "Value.hpp"

namespace Spark {

class Function {

    /* ===== Data ===== */

private:
    void* _programCounter;

public:
    [[nodiscard]]
    constexpr const void* programCounter() const {
        return _programCounter;
    }



    /* ===== Constructor & Destructor ===== */

public:
    Function(void* programCounter, size_t size) {
        _programCounter = std::malloc(size);
        if (!_programCounter)
            throw std::bad_alloc();
        std::memcpy(_programCounter, programCounter, size);
    }

    ~Function() {
        std::free(_programCounter);
    }

};

} // Spark
