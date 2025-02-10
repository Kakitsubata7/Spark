#pragma once

#include <istream>
#include <sstream>
#include <string_view>
#include <vector>

#include "result.hpp"
#include "token.hpp"

namespace spark {

class Lexer {

    /* ===== Syntax ===== */

public:
    static bool isKeyword(std::string_view sv);
    static bool isSymbol(std::string_view sv);
    static bool isIdentifier(std::string_view sv);



    /* ===== Data ===== */

private:
    const bool ignoreComments;



    /* ===== Constructor ===== */

public:
    explicit Lexer(bool ignoreComments = true) : ignoreComments(ignoreComments) { }



    /* ===== Operations ===== */

public:
    Result<std::vector<Token>> lex(std::istream& is);

    Result<std::vector<Token>> lex(const std::string& s) {
        std::istringstream iss(s);
        return lex(iss);
    }

};

} // spark
