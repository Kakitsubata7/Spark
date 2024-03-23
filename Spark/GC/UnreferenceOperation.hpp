#pragma once

#include <list>
#include <stdexcept>

#include "GCNode.hpp"
#include "GCOperation.hpp"

namespace Spark {

class UnreferenceOperation : public GCOperation {

public:
    UnreferenceOperation(GCNode* unreferencer, GCNode* unreferencee)
        : unreferencer(unreferencer), neighborIterator(unreferencer->neighbors().cbegin()), unreferencee(unreferencee) { }

private:
    GCNode* unreferencer;
    std::list<GCNode*>::const_iterator neighborIterator;
    GCNode* unreferencee;

public:
    bool step() override {
        if (neighborIterator == unreferencer->neighbors().cend())
            throw std::runtime_error("Reference cannot be found in the GC node.");

        GCNode* node = *neighborIterator;
        if (node == unreferencee) {
            unreferencee->referenceCount--;
            neighborIterator = unreferencer->neighbors().erase(neighborIterator);
            return true;
        }
        neighborIterator++;
        return false;
    }

};

} // Spark
