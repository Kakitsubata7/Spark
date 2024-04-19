#include <cstdlib>
#include <iostream>

#ifdef _WIN32
#   include <Windows.h>
#endif

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
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    BytecodeBuffer buffer;

//    buffer.append(Opcode::PushStorage);
//    buffer.append<Int64>(-1);
//
//    buffer.append(Opcode::Pop);
//
//    buffer.append(Opcode::PushEmptyArray);
//
//    buffer.append(Opcode::PushStorage);
//    buffer.append<Int64>(-1);
//
//    buffer.append(Opcode::PushInteger);
//    buffer.append<Int64>(37);
//
//    buffer.append(Opcode::Call);
//    buffer.append<Int64>(1); // Callable index
//    buffer.append<Int64>(2); // Number of arguments

    buffer.append(Opcode::PushEmptyNamespace);

    buffer.append(Opcode::Halt);

    GC gc;
    Thread thread(gc);
//    thread.push(Value::makeCFunction(Array::append));
    thread.programCounter = buffer.getOpcode();
    while (!thread.execute()) { }

    // Print the stacks
    auto printStack = [](const std::vector<std::reference_wrapper<const Value>>& stack, const std::string& prefix = "") {
        std::cout << prefix << "[";
        if (!stack.empty()) {
            for (size_t i = 0; i < stack.size(); i++) {
                std::cout << stack[i];
                if (i != stack.size() - 1)
                    std::cout << ", ";
            }
        }
        std::cout << "]" << std::endl;
    };
    printStack(thread.operationStackToVector(), "Operation: ");
    printStack(thread.storageStackToVector(), "Storage:   ");

    return EXIT_SUCCESS;
}
