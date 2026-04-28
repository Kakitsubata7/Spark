#include "parser.hpp"

#include "lexer.hpp"

namespace Spark::FrontEnd {

ParseResult Parser::parse(std::istream& stream) {
    // Set up
    Lexer lexer(stream);
    yy::parser::location_type loc;
    AST ast;
    Diagnostics diagnostics;
    yy::parser parser(lexer._scanner, &loc, ast, diagnostics);

    // Parse
    int result = parser.parse();
    if (result == 2) {
        throw std::bad_alloc();
    }
    return ParseResult{std::move(ast), std::move(diagnostics)};
}

} // Spark::FrontEnd
