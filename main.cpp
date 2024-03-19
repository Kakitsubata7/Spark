#include <cstdlib>
#include <iostream>
#include "Spark/GC/CollectOperation.hpp"
using namespace Spark;

int main() {

    CollectOperation op = CollectOperation({}, {});

    while (!op.step()) { }

    return EXIT_SUCCESS;
}
