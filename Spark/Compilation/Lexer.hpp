#pragma once

#include <vector>

#include "Token.hpp"

namespace Spark {

namespace Lexer {

    [[nodiscard]]
    std::vector<Token> lex(const std::string& code);

};

} // Spark
