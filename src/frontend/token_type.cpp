#include "token_type.hpp"

namespace Spark::FrontEnd {

const std::unordered_map<TokenType, std::string_view> tokenNameMap {
    {TokenType::EndOfFile, "EndOfFile"},
    {TokenType::Let, "Let"},
    {TokenType::Const, "Const"}
};

std::ostream& operator<<(std::ostream& os, TokenType type) {
    os << (tokenNameMap.find(type) != tokenNameMap.end() ? tokenNameMap.at(type) : "Unknown");
    return os;
}

} // Spark::FrontEnd
