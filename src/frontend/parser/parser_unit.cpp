#include "parser_unit.hpp"

#include <string>

#include "frontend/ast.hpp"

namespace Spark::FrontEnd {

Result<Node*, Error> unexpectedToken(const Token& token) {
    std::string message = "syntax error: unexpected ";
    message += token.type == TokenType::EndOfFile ? "EOF" : "'" + token.lexeme + "'";
    return Result<Node*, Error>::err(message, token.start, token.end);
}

Result<Node*, Error> BodyParser::parse() noexcept {
    std::vector<Node*> nodes;

    // Parse
    bool canExpr = true;
    while (true) {
        const Token& token = _producer.peek();

        // Check for terminator token type
        if (token.type == _terminatorType) {
            break;
        }

        switch (token.type) {
            // Block
            case TokenType::LBrace: {
                Result<Node*, Error> result = BlockParser(_producer, _ast).parse();
                if (result.hasError()) {
                    return result;
                }
                nodes.push_back(result.value());
                break;
            }

            // Variable definition
            case TokenType::Let:
            case TokenType::Const:
            case TokenType::Ref:
            case TokenType::Cref: {
                Result<Node*, Error> result = VarDefParser(_producer, _ast).parse();
                if (result.hasError()) {
                    return result;
                }
                nodes.push_back(result.value());
                break;
            }

            // While loop
            case TokenType::While: {
                Result<Node*, Error> result = WhileParser(_producer, _ast).parse();
                if (result.hasError()) {
                    return result;
                }
                nodes.push_back(result.value());
                break;
            }

            case TokenType::Semicolon:
                break;

            default:
                return unexpectedToken(token);
        }

        // Expression can only follow a ';'
        canExpr = token.type == TokenType::Semicolon;
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

Result<Node*, Error> ExprParser::parse() noexcept {

}

Result<Node*, Error> BlockParser::parse() noexcept {
    // '{'
    const Token& lBrace = _producer.next();
    if (lBrace.type != TokenType::LBrace) {
        return unexpectedToken(lBrace);
    }

    // Body
    Result<Node*, Error> bodyResult = BodyParser(_producer, _ast, TokenType::RBrace).parse();
    if (bodyResult.hasError()) {
        return bodyResult;
    }
    BodyNode* body = static_cast<BodyNode*>(bodyResult.value());

    // '}'
    const Token& rBrace = _producer.next();
    if (rBrace.type != TokenType::RBrace) {
        return unexpectedToken(rBrace);
    }

    // Construct AST node
    BlockStmt* block = _ast.make<BlockStmt>(lBrace.start, rBrace.end, body);
    return Result<Node*, Error>::ok(block);
}

Result<Node*, Error> VarDefParser::parse() noexcept {

}

Result<Node*, Error> WhileParser::parse() noexcept {
    // 'while'
    const Token& whileTok = _producer.next();
    if (whileTok.type != TokenType::While) {
        return unexpectedToken(whileTok);
    }

    // Condition expression
    Result<Node*, Error> condResult = ExprParser(_producer, _ast).parse();
    if (condResult.hasError()) {
        return condResult;
    }
    Node* condition = condResult.value();

    // Body block
    Result<Node*, Error> blockResult = BlockParser(_producer, _ast).parse();
    if (blockResult.hasError()) {
        return blockResult;
    }
    BlockStmt* block = static_cast<BlockStmt*>(blockResult.value());

    // Construct AST node
    WhileStmt* whileStmt = _ast.make<WhileStmt>(whileTok.start, block->end, condition, block);
    return Result<Node*, Error>::ok(whileStmt);
}

} // Spark::FrontEnd
