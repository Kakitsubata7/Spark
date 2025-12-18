#include "parser_unit.hpp"

#include <string>

#include "frontend/ast.hpp"

/**
 * Helper macro that takes a `ParserUnit` instance and tries parsing.
 * If the result is an error, it returns the error in the caller's scope.
 * Otherwise, it gives the value (`Node*`) of the result.
 * @param parser `ParserUnit` instance.
 */
#define PARSE_OR_ERROR(parser)    \
({                                \
    auto&& _r = (parser).parse(); \
    if (_r.hasError()) {          \
        return _r;                \
    }                             \
    _r.value();                   \
})

/**
 * Helper macro that checks if a `Token` instance has token type @p expectedType.
 * If not, it returns an error with `unexpectedToken` helper function in the caller's scope.
 * Otherwise, it gives the `token` back.
 * @param token `Token` instance to assert.
 * @param expectedType Expected type of @p token.
 */
#define ASSERT_TOKEN_TYPE(token, expectedType) \
({                                             \
    auto& _t = (token);                        \
    if (_t.type != (expectedType)) {           \
        return unexpectedToken(_t);            \
    }                                          \
    _t;                                        \
})

/**
 * Generates an error result with message "syntax error: unexpected '{token}'/EOF".
 * @param token Token used to generate the error result.
 * @return Generated error result.
 */
Spark::Result<Spark::FrontEnd::Node*, Spark::Error> unexpectedToken(const Spark::FrontEnd::Token& token) {
    std::string message = "syntax error: unexpected ";
    message += token.type == Spark::FrontEnd::TokenType::EndOfFile ? "EOF" : "'" + token.lexeme + "'";
    return Spark::Result<Spark::FrontEnd::Node*, Spark::Error>::err(message, token.start, token.end);
}

namespace Spark::FrontEnd {

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

        // Parse a statement or expression
        Node* node;
        switch (token.type) {
            // Block
            case TokenType::LBrace: {
                node = PARSE_OR_ERROR(BlockParser(_producer, _ast));
                break;
            }

            // Variable definition
            case TokenType::Let:
            case TokenType::Const:
            case TokenType::Ref:
            case TokenType::Cref: {
                node = PARSE_OR_ERROR(VarDefParser(_producer, _ast));
                break;
            }

            // While loop
            case TokenType::While: {
                node = PARSE_OR_ERROR(WhileParser(_producer, _ast));
                break;
            }

            case TokenType::Semicolon:
                break;

            default:
                return unexpectedToken(token);
        }
        nodes.push_back(node);

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
    // TODO
}

Result<Node*, Error> BlockParser::parse() noexcept {
    // {
    const Token& lBrace = ASSERT_TOKEN_TYPE(_producer.next(), TokenType::LBrace);

    // Body
    BodyNode* body = static_cast<BodyNode*>(PARSE_OR_ERROR(BodyParser(_producer, _ast, TokenType::RBrace)));

    // }
    const Token& rBrace = ASSERT_TOKEN_TYPE(_producer.next(), TokenType::RBrace);

    // Construct AST node
    BlockStmt* block = _ast.make<BlockStmt>(lBrace.start, rBrace.end, body);
    return Result<Node*, Error>::ok(block);
}

Result<Node*, Error> VarDefParser::parse() noexcept {
    // TODO
}

Result<Node*, Error> WhileParser::parse() noexcept {
    // while
    const Token& whileTok = ASSERT_TOKEN_TYPE(_producer.next(), TokenType::While);

    // Condition expression
    Node* condition = PARSE_OR_ERROR(ExprParser(_producer, _ast));

    // Body block
    BlockStmt* block = static_cast<BlockStmt*>(PARSE_OR_ERROR(BlockParser(_producer, _ast)));

    // Construct AST node
    WhileStmt* whileStmt = _ast.make<WhileStmt>(whileTok.start, block->end, condition, block);
    return Result<Node*, Error>::ok(whileStmt);
}

Result<Node*, Error> PatternParser::parse() noexcept {
    const Token& token = _producer.peek();
    switch (token.type) {
        // Binding pattern
        case TokenType::Identifier:
        case TokenType::Discard:
            return BindingPatternParser(_producer, _ast).parse();

        // Literal patterns
        case TokenType::Integer:
        case TokenType::Real:
        case TokenType::True:
        case TokenType::False:
        case TokenType::String:
        case TokenType::Nil:
            return LiteralParser(_producer, _ast).parse();

        default:
            return unexpectedToken(token);
    }
}

Result<Node*, Error> BindingPatternParser::parse() noexcept {
    // Bind
    const Token& tok = _producer.peek();
    Node* bind;
    switch (tok.type) {
        case TokenType::Identifier:
        case TokenType::Discard:
            bind = PARSE_OR_ERROR(QualifiedNameParser(_producer, _ast));
            break;

        default:
            return unexpectedToken(tok);
    }

    // : (optional typing annotation part)
    Location end = bind->end;
    Node* typeAnnot = nullptr;
    const Token& colon = _producer.peek();
    if (colon.type == TokenType::Colon) {
        _producer.next(); // Consume :

        // Type expression
        typeAnnot = PARSE_OR_ERROR(ExprParser(_producer, _ast));

        end = typeAnnot->end; // Update end location
    }

    // Construct AST node
    Node* node = _ast.make<BindingPattern>(bind->start, end, bind, typeAnnot);
    return Result<Node*, Error>::ok(node);
}

Result<Node*, Error> QualifiedNameParser::parse() noexcept {
    const Token& token = _producer.next();
    if (token.type == TokenType::Identifier) {
        Node* node = _ast.make<Identifier>(token.start, token.end, std::move(token.lexeme));
        return Result<Node*, Error>::ok(node);
    }
    if (token.type == TokenType::Discard) {
        Node* node = _ast.make<Discard>(token.start, token.end);
        return Result<Node*, Error>::ok(node);
    }
    return unexpectedToken(token);
}

Result<Node*, Error> LiteralParser::parse() noexcept {
    const Token& token = _producer.next();
    Node* node;
    switch (token.type) {
        case TokenType::Integer:
            node = _ast.make<IntLiteral>(token.start, token.end, BigInt(token.lexeme));
            break;

        case TokenType::Real:
            node = _ast.make<RealLiteral>(token.start, token.end, BigReal(token.lexeme));
            break;

        case TokenType::True:
            node = _ast.make<BoolLiteral>(token.start, token.end, true);
            break;

        case TokenType::False:
            node = _ast.make<BoolLiteral>(token.start, token.end, false);
            break;

        case TokenType::String:
            node = _ast.make<StringLiteral>(token.start, token.end, std::move(token.lexeme));
            break;

        case TokenType::Nil:
            node = _ast.make<NilLiteral>(token.start, token.end);
            break;

        case TokenType::Undefined:
            node = _ast.make<UndefinedExpr>(token.start, token.end);
            break;

        default:
            return unexpectedToken(token);
    }
    return Result<Node*, Error>::ok(node);
}

} // Spark::FrontEnd
