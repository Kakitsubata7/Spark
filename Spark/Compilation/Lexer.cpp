#include "Lexer.hpp"

#include <cctype>

namespace Spark {

    /* ===== Operations ===== */

    void Lexer::append(const char c, const std::optional<char>& next = std::nullopt) {

        //
        if (shouldSkipNextCharacter) {
            shouldSkipNextCharacter = false;
            return;
        }

        // Skip if line commenting
        if (isLineCommenting) {

            // Check for newlines
            if (c == '\r' || c == '\n') {
                isLineCommenting = false;
                return;
            }
        }

        // Skip if group commenting
        if (isGroupCommenting) {

            // Check for '*/'
            if (c == '*' && next.has_value() && next.value() == '/') {
                isGroupCommenting = false;
                return;
            }
        }

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

        if (next.has_value()) {

        }

        // Check if current is a keyword
        if (isKeyword(current)) {
            tokens.push_back(current);
            current.clear();
        }

        // Check for the end of current
        if (isspace(next.value())) {
            tokens.push_back(current);
            current.clear();
        }
    }

    void Lexer::append(char* p) {

        char c = *p;
        if (c == '\0')
            return;

        // Skip if it's line commenting
        if (isLineCommenting) {

            // Check for newlines
            if (c == '\n') {
                isLineCommenting = false;
                return;
            } else if (c == '\r') {

            }
        }
    }

    void Lexer::append(const std::string& str) {
        size_t lastIndex = str.length() - 1;
        for (size_t i = 0; i < lastIndex; i++)
            append(str[i], str[i + 1]);
        append(str[lastIndex]);
    }

} // Spark
