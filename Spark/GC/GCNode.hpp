#pragma once

#include <mutex>
#include <unordered_set>

namespace Spark {

    class GCNode {

    private:
        void* data;
        void (*destructorFunc)(void*);
        size_t referenceCount = 0;
        std::unordered_set<GCNode*> edges;
        mutable std::mutex mtx;

    private:
        template<typename T>
        static void callDestructor(void* obj) {
            static_assert(std::is_object_v<T>, "T must be an object type to call its destructor.");
            reinterpret_cast<T*>(obj)->~T();
        }

    public:
        template <typename T>
        explicit GCNode(const T& value) : destructorFunc(&callDestructor<T>) {
            data = operator new(sizeof(T));
            new (data) T(value);
        }

    public:
        GCNode(const GCNode& other) = delete;
        GCNode& operator=(const GCNode& other) = delete;
        GCNode(GCNode&&) = delete;
        GCNode& operator=(GCNode&&) = delete;

    public:
        [[nodiscard]]
        size_t getReferenceCount() const {
            std::lock_guard<std::mutex> lock(mtx);
            return referenceCount;
        }

    public:
        void increase() {
            std::lock_guard<std::mutex> lock(mtx);
            referenceCount++;
        }

        void decrease() {
            std::lock_guard<std::mutex> lock(mtx);
            referenceCount--;
            tryReclaim();
        }

        void reference(GCNode* other) {
            std::lock_guard<std::mutex> lock(mtx);
            other->increase();
            edges.insert(other);
        }

        void dereference(GCNode* other) {
            std::lock_guard<std::mutex> lock(mtx);
            other->decrease();
            edges.erase(other);
        }

        bool tryReclaim() {
            if (referenceCount == 0) {
                // Deallocate the memory
                destructorFunc(data);
                operator delete(data);

                return true;
            }

            return false;
        }

    };

} // Spark
