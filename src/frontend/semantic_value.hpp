#pragma once

#include <string>

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

    SemanticValue() = default;

    static SemanticValue makeToken(std::string lexeme, size_t line, size_t column) noexcept {
        return SemanticValue{ .line = line, .column = column, .lexeme = std::move(lexeme), .node = nullptr };
    }

    bool operator==(const SemanticValue& rhs) const noexcept {
        return line == rhs.line && column == rhs.column && lexeme == rhs.lexeme && node == rhs.node;
    }

    bool operator!=(const SemanticValue& rhs) const noexcept {
        return !(*this == rhs);
    }
};

} // Spark::FrontEnd
