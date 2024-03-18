#include <cstdlib>
#include <iostream>

int main() {

    int i = 10;
    auto lambda = [i](int a) { return i + a; };
    std::cout << "Size of the lambda: " << sizeof(lambda) << " bytes\n";

    return EXIT_SUCCESS;
}
