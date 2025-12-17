#include "parser_unit.hpp"

namespace Spark::FrontEnd {

Result<Node*, Error> BodyParser::parse() noexcept {
    std::vector<Node*> nodes;

    while (true) {
        Token& token = _producer.next();
        if (token.type == TokenType::EndOfFile) {
            break;
        }
    }

    // Construct AST node
    Location start(0, 0);
    Location end(0, 0);
    if (!nodes.empty()) {
        start = nodes.front()->start;
        end = nodes.back()->end;
    }
    Node* node = _ast.make<BodyNode>(start, end, std::move(nodes));

    return Result<Node*, Error>::ok(node);
}


} // Spark::FrontEnd
