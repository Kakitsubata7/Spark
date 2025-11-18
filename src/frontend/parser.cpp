#include "parser.hpp"

#include <new>

#include <parser.tab.hpp>

#include "ast/ast.hpp"
#include "lexer.hpp"
#include "parser_context.hpp"

namespace Spark::FrontEnd {

Result<AST::AST, ParserError> Parser::parse(std::istream& stream) {
    AST::AST ast;
    Lexer lexer(stream);
    ParserContext ctx(ast);
    yy::parser parser(lexer._scanner, ctx);

    int result = parser.parse();
    if (result == 1) {
        return Result<AST::AST, ParserError>::err(std::move(ctx.error));
    }
    if (result == 2) {
        throw std::bad_alloc();
    }
    return Result<AST::AST, ParserError>::ok(std::move(ast));
}

} // Spark::FrontEnd
