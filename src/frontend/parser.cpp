#include "parser.hpp"

#include <new>

#include <parser.tab.hpp>

#include "lexer.hpp"

namespace Spark::FrontEnd {

AST Parser::parse(std::istream& stream) {
    AST ast;
    Lexer lexer(stream);
    ParserContext ctx(ast);
    yy::parser parser(lexer._scanner, lexer._lstate, ctx);

    int result = parser.parse();
    if (result == 1) {
        throw std::runtime_error(ctx.error.message); // TODO
    }
    if (result == 2) {
        throw std::bad_alloc();
    }
    return ast;
}

} // Spark::FrontEnd
