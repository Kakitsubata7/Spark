#include <cstdlib>
#include <iostream>

#include "Spark/GC/AllocateOperation.hpp"
#include "Spark/GC/CollectOperation.hpp"
#include "Spark/GC/GC.hpp"
#include "Spark/GC/GCNode.hpp"
#include "Spark/GC/GCOperation.hpp"
#include "Spark/GC/GCPtr.hpp"
#include "Spark/GC/ReferenceOperation.hpp"
#include "Spark/GC/UnreferenceOperation.hpp"

using namespace Spark;

int main() {

    GCNode* a = new GCNode(GCNode::make<std::string>("a"));
    GCNode* b = new GCNode(GCNode::make<std::string>("b"));
    GCNode* c = new GCNode(GCNode::make<std::string>("c"));
    GCNode* d = new GCNode(GCNode::make<std::string>("d"));
    GCNode* e = new GCNode(GCNode::make<std::string>("e"));

    a->referenceCount++;
    a->neighbors().push_back(b); // 'a' references 'b'
    b->referenceCount++;
    a->neighbors().push_back(c); // 'a' references 'c'
    c->referenceCount++;

    std::unordered_set<GCNode*> allNodeSet = { a, b, c, d, e };
    std::vector<GCNode*> entryNodes = { a };
    CollectOperation op = CollectOperation(allNodeSet, entryNodes);

    while (!op.step()) { }

    std::cout << "Collect finished" << std::endl;

    std::cout << allNodeSet.size() << std::endl;
    for (GCNode* node : allNodeSet) {
        std::cout << node->getData<std::string>() << std::endl;
    }

    return EXIT_SUCCESS;
}
