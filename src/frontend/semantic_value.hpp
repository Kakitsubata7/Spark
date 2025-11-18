#pragma once

#include <string>
#include <vector>

#include "ast/node.hpp"

namespace Spark::FrontEnd {

/**
 * Represents a semantic value type used by Flex and Bison (as `yylval`).
 */
struct SemanticValue {
    size_t line = 0;
    size_t column = 0;

    std::string lexeme;

    AST::Node* node = nullptr;
    std::vector<AST::Node*> seq;

    SemanticValue() = default;
};

} // Spark::FrontEnd
