#include <iostream>
#include <string>

#include "Spark.hpp"
#include "Spark/GC/GCHandle.hpp"
#include "Spark/GC/GCNode.hpp"

using namespace Spark;

int main() {

    Value value = Value::makeConstant<Integer>(1);
    std::cout << value.isConstant() << std::endl;
    std::cout << value << std::endl;

    return 0;
}
