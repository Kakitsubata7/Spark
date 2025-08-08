#pragma once

#include "allocator.hpp"
#include "thread.hpp"

namespace Spark::Runtime {

template <typename Allocator = DefaultAllocator>
class Spark {
private:
    Allocator _allocator;

    std::unordered_set<Thread<Allocator>*> _threads;

public:
    explicit Spark(Allocator allocator = {}) : _allocator(std::move(allocator)) { }

    ~Spark() {
        for (Thread<Allocator>* th : _threads) {
            delete th;
        }
    }

    Spark(const Spark& other) = delete;
    Spark& operator=(const Spark& other) = delete;
};

} // Spark::Runtime
