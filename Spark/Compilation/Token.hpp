#pragma once

#include <string>

#include "TokenType.hpp"

namespace Spark {

class Token {

    /* ===== Data ===== */

private:
    TokenType type;
    std::string content;



    /* ===== Constructors ===== */

public:
    Token(TokenType type, const std::string& content) : type(type), content(content) { }
    Token(TokenType type, std::string&& content) : type(type), content(std::move(content)) { }

};

} // Spark
