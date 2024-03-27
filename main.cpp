#include <cstdlib>
#include <iostream>

#include "Spark/Compilation/BytecodeBuffer.hpp"
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
    BytecodeBuffer buffer;
    buffer.append(Opcode::PushInteger);
    buffer.append<Int>(7);
    buffer.append(Opcode::PushFloat);
    buffer.append<Float>(3.14);
    buffer.append(Opcode::PushBoolean);
    buffer.append<Bool>(true);
    buffer.append(Opcode::PushString);
    buffer.append<Int>(3);
    buffer.appendString("abc");
    buffer.append(Opcode::PushNil);
    buffer.append(Opcode::Halt);

    GC gc;
    Thread thread(gc);
    thread.programCounter = buffer.getOpcode();
    while (!thread.execute()) { }

    // Print the stack
    std::vector<Value> stack = thread.stackToVector();
    if (!stack.empty()) {
        std::cout << "[";
        for (size_t i = 0; i < stack.size(); i++) {
            std::cout << stack[i];
            if (i != stack.size() - 1)
                std::cout << ", ";
        }
        std::cout << "]" << std::endl;
    }

    return EXIT_SUCCESS;
}
