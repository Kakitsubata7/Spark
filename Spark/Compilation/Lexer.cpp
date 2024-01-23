#include "Lexer.hpp"

#include <cctype>

namespace Spark {

    /* ===== Operations ===== */

    void Lexer::append(const char c, const std::optional<char>& next = std::nullopt) {

        // Ignore empty space characters for identifiers
        if (isspace(c))
            return;

        // Check for comment beginnings
        if (c == '/' && next.has_value()) {

            // Check for line comment beginning
            if (next.value() == '/') {
                isLineCommenting = true;
                return;
            }
            // Check for group comment beginning
            else if (next.value() == '*') {
                isGroupCommenting = true;
                return;
            }
        }

        // Append the character to current
        current += c;

        // Check if current is a keyword
        if (isKeyword(current)) {
            tokens.push_back(current);
            current.clear();
        }

        // Check for the end of current
        if (next.has_value() && isspace(next.value())) {
            tokens.push_back(current);
            current.clear();
        }
    }

} // Spark
