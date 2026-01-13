#include "parser.hpp"

#include "lexer.hpp"

namespace Spark::FrontEnd {

std::pair<AST, std::vector<Error>> Parser::parse(std::istream& stream,
                                                 std::optional<std::string_view> filename) {
    // Set up
    Lexer lexer(stream, filename);
    AST ast;
    std::vector<Error> errors;
    yy::parser::location_type loc;
    yy::parser parser(lexer._scanner, &loc, ast, errors);

    // Parse
    int result = parser.parse();
    if (result == 2) {
        throw std::bad_alloc();
    }
    return std::make_pair(std::move(ast), std::move(errors));
}

} // Spark::FrontEnd
