#include <iostream>

#include "Spark/GC/GC.hpp"
#include "Spark/GC/GCPtr.hpp"
#include "Spark/Bool8.hpp"
#include "Spark/Float64.hpp"
#include "Spark/Int64.hpp"
#include "Spark/Type.hpp"
#include "Spark/Value.hpp"

using namespace Spark;

int main() {

//    Value value = Value::make(1);
//    value = nullptr;
//    std::cout << "Type: " << value.type() << std::endl;
//    std::cout << "Is constant: " << std::boolalpha << value.isConstant() << std::endl;
//    std::cout << "Value: " << value << std::endl;

    GC gc;

    GCPtr<Int64> p = gc.make<Int64>(3);

    std::cout << "Value: " << *p << std::endl;
    std::cout << "Ref count: " << p.referenceCount() << std::endl;

    return 0;
}
