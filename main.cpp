#include <cstdlib>
#include <iostream>

#include <Windows.h>

#include "Spark/Compilation/BytecodeBuffer.hpp"
#include "Spark/Env.hpp"
#include "Spark/GC/AllocateOperation.hpp"
#include "Spark/GC/CollectOperation.hpp"
#include "Spark/GC/GC.hpp"
#include "Spark/GC/GCNode.hpp"
#include "Spark/GC/GCOperation.hpp"
#include "Spark/GC/ReferenceOperation.hpp"
#include "Spark/GC/UnreferenceOperation.hpp"
#include "Spark/Types.hpp"

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
    SetConsoleOutputCP(CP_UTF8);

    BytecodeBuffer buffer;
    
    buffer.append(Opcode::PushInteger);
    buffer.append<Int64>(7);

    buffer.append(Opcode::PushFloat);
    buffer.append<Float64>(3.14);

    buffer.append(Opcode::PushBoolean);
    buffer.append<Bool>(true);

    buffer.append(Opcode::PushString);
    buffer.appendString(u8"凯");

    buffer.append(Opcode::PushString);
    buffer.appendString(u8"书");

    buffer.append(Opcode::Add);

    buffer.append(Opcode::PushFloat);
    buffer.append<Float64>(1.01);

    buffer.append(Opcode::PushFloat);
    buffer.append<Float64>(7.0);

    buffer.append(Opcode::Divide);

    buffer.append(Opcode::PushEmptyArray);

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
