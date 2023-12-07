#include <iostream>
#include <string>

#include "Spark.hpp"
#include "Spark/GC/GCHandle.hpp"
#include "Spark/GC/GCNode.hpp"

using namespace Spark;

int main() {

    Value value = Value::make<Integer>(1);
    value = Pointer(&value);

    std::cout << "Type: " << value.type() << std::endl;
    std::cout << "Is Constant: " << value.isConstant() << std::endl;
    std::cout << "Value: " << value << std::endl;

    Float a = 1;
    Integer b = a;
    Float c = b;
    std::cout << c << std::endl;

    return 0;
}
