#include "parser.hpp"

#include "lexer.hpp"

namespace Spark::FrontEnd {

Result<AST, Error> Parser::parse(std::istream& stream, std::optional<std::string_view> filename) {
    // TODO
    return Result<AST, Error>::ok(AST());
}

} // Spark::FrontEnd
