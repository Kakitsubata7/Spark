#include "Lexer.hpp"

namespace Spark {

    /* ===== Operations ===== */

    std::vector<Token> Lexer::lex(const std::string& str) {
        return Lexer(str).lex();
    }

    std::vector<Token> Lexer::lex() {
        std::vector<Token> tokens;

        char c;
        while (iss.get(c)) {

        }

        return tokens;
    }

} // Spark
