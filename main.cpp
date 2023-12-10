#include <iostream>

#include "Spark/GC/GC.hpp"
#include "Spark/GC/GCPtr.hpp"

using namespace Spark;

int main() {

//    Value value = Value::make(1);
//    value = nullptr;
//    std::cout << "Type: " << value.type() << std::endl;
//    std::cout << "Is constant: " << std::boolalpha << value.isConstant() << std::endl;
//    std::cout << "Value: " << value << std::endl;

    GC gc;

    GCPtr<int> p = gc.make<int>(3);

    std::cout << "Value: " << *p << std::endl;
    std::cout << "Ref count: " << p.referenceCount() << std::endl;

    return 0;
}
