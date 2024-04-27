#include "TokenType.hpp"

namespace Spark {

    std::ostream& operator<<(std::ostream& os, TokenType tokenType) {
        switch (tokenType) {
            case TokenType::Keyword:
                os << "Keyword";
                break;

            case TokenType::Operator:
                os << "Operator";
                break;

            case TokenType::NumericalLiteral:
                os << "NumericalLiteral";
                break;

            case TokenType::StringLiteral:
                os << "StringLiteral";
                break;

            case TokenType::Identifier:
                os << "Identifier";
                break;

            default:
                throw std::runtime_error("Token type is not implemented.");
        }

        return os;
    }

} // Spark
