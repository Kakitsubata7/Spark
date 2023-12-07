#include <iostream>

#include "Spark/Value.hpp"

using namespace Spark;

int main() {

    Value value = Value::make<bool>(true);
    std::cout << value << std::endl;

    return 0;
}
