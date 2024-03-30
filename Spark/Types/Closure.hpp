#pragma once

#include <cstring>
#include <string>
#include <unordered_map>

#include "../Opcode.hpp"
#include "Value.hpp"

namespace Spark {

class Closure {

    /* ===== Data ===== */

private:
    void* _programCounter;
    std::unordered_map<std::string, Value> capturedMap;

public:
    [[nodiscard]]
    constexpr const void* programCounter() const {
        return _programCounter;
    }



    /* ===== Constructor & Destructor ===== */

public:
    Closure(void* programCounter, size_t size) {
        _programCounter = std::malloc(size);
        if (!_programCounter)
            throw std::bad_alloc();
        std::memcpy(_programCounter, programCounter, size);
    }

    ~Closure() {
        std::free(_programCounter);
    }

};

} // Spark
