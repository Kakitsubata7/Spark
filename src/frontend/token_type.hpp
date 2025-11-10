#pragma once

#include <ostream>

#include <parser.tab.hpp>

namespace Spark::FrontEnd {

/**
 * Type alias to `yy::parser::token_kind_type`.
 * Used by Flex, as returned by `yylex`.
 */
using TokenType = yy::parser::token_kind_type;

std::string_view toString(TokenType type) noexcept;

std::ostream& operator<<(std::ostream& os, TokenType type);

} // Spark::FrontEnd
