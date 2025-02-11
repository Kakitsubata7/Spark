#include "token.hpp"

namespace spark {

std::ostream& operator<<(std::ostream& os, const Token& token) {
    os << "<";
    switch (token.type) {
        case TokenType::Identifier:
            os << "Identifier: '" << token.lexeme << '\'';
            break;

        case TokenType::Keyword:
            os << "Keyword: '" << token.lexeme << '\'';
            break;

        case TokenType::Symbol:
            os << "Symbol: '" << token.lexeme << '\'';
            break;

        case TokenType::Integer:
            os << "Integer: '" << token.lexeme << '\'';
            break;

        case TokenType::Real:
            os << "Real: '" << token.lexeme << '\'';
            break;

        case TokenType::Boolean:
            os << "Boolean: '" << token.lexeme << '\'';
            break;

        case TokenType::String:
            os << "String: '" << token.lexeme << '\'';
            break;

        case TokenType::Nil:
            os << "Nil";
            break;

        case TokenType::Ignore:
            os << "Ignore";
            break;

        case TokenType::Comment:
            os << "Comment: '" << token.lexeme << '\'';
            break;

        case TokenType::End:
            os << "End";
            break;

        case TokenType::Unknown:
            os << "Unknown: '" << token.lexeme << '\'';
            break;
    }
    os << ' ' << token.line << ':' << token.column;
    os << ">";
    return os;
}

} // spark
