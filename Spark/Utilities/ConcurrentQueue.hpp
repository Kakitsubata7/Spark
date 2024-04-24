#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>

namespace Spark {

template <typename T>
class ConcurrentQueue {

    // Make sure templated type has move semantics
    static_assert(std::is_move_constructible<T>::value, "T must be move constructible.");

    /* ===== Data ===== */

private:
    std::queue<T> queue;
    mutable std::mutex mutex;
    std::condition_variable cv;



    /* ===== Constructor & Destructor ===== */

public:
    ConcurrentQueue() = default;

    ~ConcurrentQueue() = default;



    /* ===== Operations ===== */

public:
    [[nodiscard]]
    bool empty() const {
        std::lock_guard<std::mutex> lock(mutex);
        return queue.empty();
    }

    [[nodiscard]]
    size_t size() const {
        std::lock_guard<std::mutex> lock(mutex);
        return queue.size();
    }

    void push(const T& value) {
        {
            std::lock_guard<std::mutex> lock(mutex);
            queue.push(value);
        }
        cv.notify_one();
    }

    void push(T&& value) {
        {
            std::lock_guard<std::mutex> lock(mutex);
            queue.push(std::move(value));
        }
        cv.notify_one();
    }

    template <typename... Args>
    void emplace(Args&&... args) {
        {
            std::lock_guard<std::mutex> lock(mutex);
            queue.emplace(std::forward<Args>(args)...);
        }
        cv.notify_one();
    }

    [[nodiscard]]
    bool tryPeek(T& out) {
        std::lock_guard<std::mutex> lock(mutex);

        if (queue.empty())
            return false;

        out = queue.front();
        return true;
    }

    [[nodiscard]]
    bool tryPop() {
        std::lock_guard<std::mutex> lock(mutex);

        if (queue.empty())
            return false;

        queue.pop();
        return true;
    }

    [[nodiscard]]
    bool tryPop(T& out) {
        std::lock_guard<std::mutex> lock(mutex);

        if (queue.empty())
            return false;

        out = queue.front();
        queue.pop();
        return true;
    }

    void waitUntilNotEmpty() {
        std::unique_lock<std::mutex> lock(mutex);
        while (queue.empty())
            cv.wait(lock);
    }

    [[nodiscard]]
    T& waitUntilPeek() {
        std::unique_lock<std::mutex> lock(mutex);

        // Wait until queue is not empty
        while (queue.empty())
            cv.wait(lock);

        return queue.front();
    }

    T waitUntilPop() {
        std::unique_lock<std::mutex> lock(mutex);

        // Wait until queue is not empty
        while (queue.empty())
            cv.wait(lock);

        T result = std::move(queue.front());
        queue.pop();
        return result;
    }

};

} // Spark
