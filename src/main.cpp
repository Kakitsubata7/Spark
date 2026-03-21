#include <cstdlib>
#include <iostream>

int main(int argc, char* argv[]) {
    // Checks argument
    if (argc != 2) {
        const char* name = "spark";
        if (argc > 1) {
            name = argv[0];
        }
        std::cerr << "Usage: " << name << " <file>" << std::endl;
        return EXIT_FAILURE;
    }

    // Get path
    const char* path = argv[1];

    return EXIT_SUCCESS;
}
