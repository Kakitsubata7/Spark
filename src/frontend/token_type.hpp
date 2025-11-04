#pragma once

#include <parser.tab.hpp>

namespace Spark::FrontEnd {

/**
 * Type alias to `yy::parser::token_kind_type`.
 * Used by Flex, as returned by `yylex`.
 */
using TokenType = yy::parser::token_kind_type;

} // Spark::FrontEnd
