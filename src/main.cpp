#include <cstdlib>
#include <iostream>
#include <sstream>

#include "frontend/lexer.hpp"
#include "frontend/lexer_state.hpp"
#include "frontend/token.hpp"

using namespace Spark::FrontEnd;

int main(int argc, char* argv[]) {
    std::string code = R"(
let x = "abc" if 'xyz\n' > 0 do x = x + 1 end
    )";
    std::istringstream iss(code);
    Lexer lexer(iss);

    while (true) {
        Token token = lexer.lex();
        if (token.type == TokenType::EndOfFile) {
            std::cout << "<EOF>" << std::endl;
            break;
        }

        std::cout << "Token("
                  << token.type << ", "
                  << token.value.lexeme << ", "
                  << "line=" << token.value.line << ", "
                  << "col=" << token.value.column
                  << ")\n";
    }

    return EXIT_SUCCESS;
}
