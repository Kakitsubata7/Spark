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
    std::unordered_map<std::string, Value> _capturedMap;

public:
    [[nodiscard]]
    constexpr void* programCounter() const {
        return _programCounter;
    }

    [[nodiscard]]
    constexpr std::unordered_map<std::string, Value>& capturedMap() {
        return _capturedMap;
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
