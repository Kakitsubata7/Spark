#include "lexer.hpp"

#include <cstring>

using namespace Spark;
using namespace Spark::FrontEnd;

Result<Token> Lexer::lex() const {
    std::string lexeme;

    char c;
    while (_stream.get(c)) {
        if (std::isspace(c)) {
            continue;
        }

        lexeme += c;
    }
}

Result<std::vector<Token>> Lexer::lexAll() const {

}
