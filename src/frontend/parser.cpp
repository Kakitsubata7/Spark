#include "parser.hpp"

#include <new>

#include <parser.tab.hpp>

#include "lexer.hpp"

namespace Spark::FrontEnd {

std::pair<AST, std::vector<Error>> Parser::parse(std::istream& stream,
                                                 std::optional<std::string_view> filename,
                                                 SourceBuffer& srcbuf) {
    AST ast;
    Lexer lexer(stream, filename, srcbuf);
    ParserContext ctx(ast);
    yy::parser parser(lexer._scanner, ctx);

    int result = parser.parse();
    if (result == 1) {
        std::string msg;
        msg += '\n';
        for (const Error& error : ctx.errors()) {
            msg += error.render(filename, srcbuf);
            msg += '\n';
        }
        throw std::runtime_error("msg");
    }
    if (result == 2) {
        throw std::bad_alloc();
    }

    return std::make_pair<AST, std::vector<Error>>(std::move(ast),
        std::move(const_cast<std::vector<Error>&>(ctx.errors())));
}

} // Spark::FrontEnd
