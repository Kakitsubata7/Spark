#include <iostream>
#include <string>

#include "Spark.hpp"
#include "Spark/GC/GCHandle.hpp"
#include "Spark/GC/GCNode.hpp"

using namespace Spark;

int main() {

    GCNode node(std::string(""));
    std::cout << node.getReferenceCount() << std::endl;
    std::cout << std::boolalpha << node.tryReclaim() << std::endl;

    return 0;
}
