#pragma once

#include <ostream>
#include <string>
#include <string_view>

namespace spark {

enum class TokenType {
    Identifier,
    Keyword,
    Symbol,
    Integer,
    Real,
    Boolean,
    String,
    Nil,
    Discard,
    Comment,
    End,
    Unknown
};

struct Token {
    const TokenType type;
    const std::string lexeme;
    const size_t line;
    const size_t column;

    Token(TokenType type, std::string_view lexeme, size_t line, size_t column)
        : type(type), lexeme(lexeme), line(line), column(column) { }

    /**
     * @brief Outputs a Token in the format \<type: 'lexeme' line:column\>.
     *
     * This overload allows <c>Token</c>s to be printed directly to an output stream
     * for debugging purposes.
     *
     * @param os The output stream to write to.
     * @param token The Token to print.
     * @return A reference to the output stream.
     */
    friend std::ostream& operator<<(std::ostream& os, const Token& token);
};

} // spark
