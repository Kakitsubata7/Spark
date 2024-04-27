#pragma once

#include <ostream>
#include <stdexcept>

namespace Spark {

enum class TokenType {
    Keyword,
    Operator,
    Literal,
    Identifier
};

std::ostream& operator<<(std::ostream& os, TokenType tokenType);

} // Spark
