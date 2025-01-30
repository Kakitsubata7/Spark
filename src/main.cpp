#include <cstdlib>
#include <iostream>

#include "compiler/lexer.hpp"

using namespace spark;

int main(int argc, const char* argv[]) {
    Lexer lexer;
    Result<std::vector<Token>> result = lexer.lex("if condition do statement else statement end");

    if (result.value.has_value()) {
        for (const Token& token : result.value.value()) {
            std::cout << "{" << static_cast<int>(token.type) << ": " << token.lexeme << "}" << std::endl;
        }
    }

    return EXIT_SUCCESS;
}
