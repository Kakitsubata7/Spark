#pragma once

#include "GCNode.hpp"

namespace Spark {

    class GCHandle {

    private:
        GCNode* node;

    public:
        explicit GCHandle(GCNode* node) : node(node) {
            node->increase();
        }

    public:
        ~GCHandle() {
            node->decrease();
        }

    public:
        GCHandle(const GCHandle& other) : node(other.node) {
            node->increase();
        }

        GCHandle& operator=(const GCHandle& other) {

            // Prevent self-assignment
            if (this != &other) {
                node = other.node;
                node->increase();
            }

            return *this;
        }

    };

} // Spark
