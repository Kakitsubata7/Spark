#pragma once

#include <istream>
#include <sstream>
#include <vector>

#include "result.hpp"
#include "token.hpp"

namespace spark {

class Lexer {

    /* ===== Syntax ===== */

public:
    static void initializeSyntax();



    /* ===== Data ===== */

private:
    bool ignoreComments;



    /* ===== Constructor ===== */

public:
    explicit Lexer(bool ignoreComments = true) : ignoreComments(ignoreComments) {
        initializeSyntax();
    }



    /* ===== Operations ===== */

public:
    Result<std::vector<Token>> lex(std::istream& is);

    Result<std::vector<Token>> lex(const std::string& s) {
        std::istringstream iss(s);
        return lex(iss);
    }

};

} // spark
