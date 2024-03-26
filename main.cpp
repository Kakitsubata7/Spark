#include <cstdlib>
#include <iostream>

#include "Spark/GC/AllocateOperation.hpp"
#include "Spark/GC/CollectOperation.hpp"
#include "Spark/GC/GC.hpp"
#include "Spark/GC/GCNode.hpp"
#include "Spark/GC/GCOperation.hpp"
#include "Spark/GC/ReferenceOperation.hpp"
#include "Spark/GC/UnreferenceOperation.hpp"
#include "Spark/Env.hpp"

using namespace Spark;

int main() {

//    /* GC Test */
//    std::unordered_set<GCNode*> allNodeSet;
//    std::unordered_map<GCNode*, long> entryNodeMap;
//
//    GCNode* a = new GCNode(GCNode::make<std::string>("a"));
//    GCNode* b = new GCNode(GCNode::make<std::string>("b"));
//    GCNode* c = new GCNode(GCNode::make<std::string>("c"));
//    GCNode* d = new GCNode(GCNode::make<std::string>("d"));
//    GCNode* e = new GCNode(GCNode::make<std::string>("e"));
//
//    entryNodeMap.insert({a, 1});
//
//    std::queue<GCOperation*> queue;
//
//    queue.emplace(new AllocateOperation(a, allNodeSet));
//    queue.emplace(new AllocateOperation(b, allNodeSet));
//    queue.emplace(new AllocateOperation(c, allNodeSet));
//    queue.emplace(new AllocateOperation(d, allNodeSet));
//    queue.emplace(new AllocateOperation(e, allNodeSet));
//
//    queue.emplace(new ReferenceOperation(a, b));
//    queue.emplace(new ReferenceOperation(a, c));
//
//    queue.emplace(new CollectOperation(allNodeSet, entryNodeMap));
//
//    while (!queue.empty()) {
//        GCOperation* op = queue.front();
//
//        while (!op->step()) { }
//
//        queue.pop();
//        delete op;
//    }
//
//    std::cout << "Nodes: " << std::endl;
//    for (GCNode* node : allNodeSet)
//        std::cout << node->getData<std::string>() << std::endl;



    /* Thread Test */
    Opcode opcodes[] = {
        Opcode::PushNil,
        Opcode::Halt,
    };

    GC gc;
    Thread thread(gc);
    thread.programCounter = opcodes;
    while (!thread.execute()) { }

    for (const Value& value : thread.stackToVector()) {
        std::cout << value << std::endl;
    }

    return EXIT_SUCCESS;
}
