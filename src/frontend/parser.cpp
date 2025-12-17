#include "parser.hpp"

#include "lexer.hpp"
#include "parser/parser_unit.hpp"

namespace Spark::FrontEnd {

Result<AST, Error> Parser::parse(std::istream& stream, std::optional<std::string_view> filename) {
    // Lexer lexer(stream, filename);
    // LexerTokenProducer producer(lexer);
    // AST ast;
    // BodyParser parser(producer, ast);
    // Result<Node*, Error> result = parser.parse();
    // return Result<AST, Error>::ok(std::move(ast));
    return Result<AST, Error>::ok(AST());
}

} // Spark::FrontEnd
