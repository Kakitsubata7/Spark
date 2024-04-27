#include "TokenType.hpp"

namespace Spark {

    std::ostream& operator<<(std::ostream& os, TokenType tokenType) {
        switch (tokenType) {
            case TokenType::Identifier:
                os << "Identifier";
                break;

            case TokenType::Keyword:
                os << "Keyword";
                break;

            case TokenType::Literal:
                os << "Literal";
                break;

            case TokenType::Operator:
                os << "Operator";
                break;

            case TokenType::Delimiter:
                os << "Delimiter";
                break;

            default:
                throw std::runtime_error("Token type is not implemented.");
        }

        return os;
    }

} // Spark
