#pragma once

#include <string>

namespace Spark::FrontEnd {

/**
 * Represents a semantic value type used by Flex and Bison (as `yylval`).
 */
struct SemanticValue {
    size_t line = 0;
    size_t column = 0;

    std::string lexeme;

    SemanticValue() = default;

    static SemanticValue makeToken(std::string lexeme, size_t line, size_t column) noexcept {
        return SemanticValue{ .line = line, .column = column, .lexeme = std::move(lexeme) };
    }

    bool operator==(const SemanticValue& rhs) const noexcept {
        return true; // TODO
    }

    bool operator!=(const SemanticValue& rhs) const noexcept {
        return !(*this == rhs);
    }
};

} // Spark::FrontEnd
