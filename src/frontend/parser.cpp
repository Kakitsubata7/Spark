#include "parser.hpp"

#include "lexer.hpp"

namespace Spark::FrontEnd {

ParseResult Parser::parse(std::istream& stream) {
    // Set up
    Lexer lexer(stream);
    AST ast;
    std::vector<Error> errors;
    yy::parser::location_type loc;
    yy::parser parser(lexer._scanner, &loc, ast, errors);

    // Parse
    int result = parser.parse();
    if (result == 2) {
        throw std::bad_alloc();
    }
    return ParseResult{std::move(ast), {}};
}

} // Spark::FrontEnd
