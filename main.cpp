#include <cstdlib>
#include <iostream>
#include <string>

#include "Spark/Compilation/Lex/Lexer.hpp"

using namespace Spark;

int main() {

    std::string code = R"(
let a=r'Kaishu is a ball';
)";
    std::vector<Token> tokens = Lexer::lex(code);

    for (const Token& token : tokens)
        std::cout << token << std::endl;

    return EXIT_SUCCESS;
}
