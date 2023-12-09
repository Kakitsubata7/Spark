#include <iostream>

#include "Spark/Value.hpp"

using namespace Spark;

int main() {

//    Value value = Value::make(1);
//    value = nullptr;
//    std::cout << "Type: " << value.type() << std::endl;
//    std::cout << "Is constant: " << std::boolalpha << value.isConstant() << std::endl;
//    std::cout << "Value: " << value << std::endl;

    auto p = GCPtr<int>::make(3);
    std::cout << "Value: " << (*p) << std::endl;
    std::cout << "Ref count: " << p.count() << std::endl;

    return 0;
}
