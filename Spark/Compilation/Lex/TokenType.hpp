#pragma once

#include <ostream>
#include <stdexcept>

namespace Spark {

enum class TokenType {
    Identifier,
    Keyword,
    Literal,
    Operator
};

std::ostream& operator<<(std::ostream& os, TokenType tokenType);

} // Spark
