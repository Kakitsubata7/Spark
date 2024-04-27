#include <cstdlib>
#include <iostream>
#include <string>

#include "Spark/Compilation/Lex/Lexer.hpp"

using namespace Spark;

int main() {

    std::string code = R"(
let a=/* Group comment */1; // Line comment
)";
    std::vector<Token> tokens = Lexer::lex(code);

    for (const Token& token : tokens)
        std::cout << token << std::endl;

    return EXIT_SUCCESS;
}
