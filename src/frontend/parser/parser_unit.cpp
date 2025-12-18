#include "parser_unit.hpp"

#include <string>
#include <typeinfo>

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
        const Token& token = peek();

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
            case TokenType::Cref:
            case TokenType::LParen:
            case TokenType::LBracket:
            case TokenType::Identifier: {
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
    Node* node = make<Body>(start, end, std::move(nodes));
    return Result<Node*, Error>::ok(node);
}

Result<Node*, Error> ExprParser::parse() noexcept {
    // TODO
}

Result<Node*, Error> BlockParser::parse() noexcept {
    // {
    const Token& lBrace = ASSERT_TOKEN_TYPE(next(), TokenType::LBrace);

    // Body
    Body* body = static_cast<Body*>(PARSE_OR_ERROR(BodyParser(_producer, _ast, TokenType::RBrace)));

    // }
    const Token& rBrace = ASSERT_TOKEN_TYPE(next(), TokenType::RBrace);

    // Construct AST node
    BlockStmt* block = make<BlockStmt>(lBrace.start, rBrace.end, body);
    return Result<Node*, Error>::ok(block);
}

Result<Node*, Error> VarDefParser::parse() noexcept {
    // LHS pattern
    Node* lhs = PARSE_OR_ERROR(LhsPatternParser(_producer, _ast));
    if (auto* p = dynamic_cast<TuplePatternParser*>(lhs)) {

    } else {

    }

    // = (optional assignment part)
    Location start = lhs->start;
    Location end = lhs->end;
    Node* rhs = nullptr;
    if (peek().type == TokenType::Assign) {
        advance(); // Consume =
        rhs = PARSE_OR_ERROR(ExprParser(_producer, _ast));
        end = rhs->end;
    }

    // Construct AST node
    Node* node = make<VarDefStmt>(start, end, lhs, rhs);
    return Result<Node*, Error>::ok(node);
}

Result<Node*, Error> WhileParser::parse() noexcept {
    // while
    const Token& whileTok = ASSERT_TOKEN_TYPE(next(), TokenType::While);

    // Condition expression
    Node* condition = PARSE_OR_ERROR(ExprParser(_producer, _ast));

    // Body block
    BlockStmt* block = static_cast<BlockStmt*>(PARSE_OR_ERROR(BlockParser(_producer, _ast)));

    // Construct AST node
    WhileStmt* whileStmt = make<WhileStmt>(whileTok.start, block->end, condition, block);
    return Result<Node*, Error>::ok(whileStmt);
}

Result<Node*, Error> PatternParser::parse() noexcept {
    const Token& token = peek();
    switch (token.type) {
        // Binding pattern / record pattern
        case TokenType::Identifier:
        case TokenType::Discard: {
            // Record pattern
            RewindTokenProducer producer(_producer);
            Result<Node*, Error> result = RecordPatternParser(producer, _ast, _isLhs).parse();
            if (result.hasValue()) {
                return result;
            }

            // Binding pattern
            producer.rewind();
            return BindingPatternParser(producer, _ast).parse();
        }

        // Literal pattern
        case TokenType::Integer:
        case TokenType::Real:
        case TokenType::True:
        case TokenType::False:
        case TokenType::String:
        case TokenType::Nil: {
            Result<Node*, Error> result = LiteralParser(_producer, _ast).parse();
            if (result.hasValue() && _isLhs) {
                Node* node = result.value();
                return Result<Node*, Error>::err("syntax error: literal cannot be used as a binding pattern",
                    node->start, node->end);
            }
            return result;
        }

        // Tuple pattern
        case TokenType::LParen:
            return TuplePatternParser(_producer, _ast, _isLhs).parse();

        // Collection pattern
        case TokenType::LBracket:
            return CollectionPatternParser(_producer, _ast, _isLhs).parse();

        default:
            return unexpectedToken(token);
    }
}

Result<Node*, Error> LhsPatternParser::parse() noexcept {
    return PatternParser(_producer, _ast, true).parse();
}

Result<Node*, Error> BindingPatternParser::parse() noexcept {
    // Modifier keyword
    BindingModifier mod;
    switch (peek().type) {
        case TokenType::Let:
            mod.kind = VarKind::Let;
            break;

        case TokenType::Const:
            mod.kind = VarKind::Const;
            break;

        case TokenType::Ref:
            mod.kind = VarKind::Ref;
            break;

        case TokenType::Cref:
            mod.kind = VarKind::Cref;
            break;

        default:
            mod.kind = VarKind::None;
            break;
    }

    // Modifier operators
    if (mod.kind != VarKind::None) {
        advance(); // Consume modifier keyword

        while (true) {
            TokenType t = peek().type;
            if (t == TokenType::Caret) {
                mod.isImmutable = true;
            } else if (t == TokenType::Question || t == TokenType::Coalesce) {
                mod.isNullable = true;
            } else if (t == TokenType::Bang) {
                mod.isNonNullable = true;
            } else {
                break;
            }
            advance(); // Consume modifier operator
        }
    }

    // Bind
    const Token& tok = peek();
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
    const Token& colon = peek();
    if (colon.type == TokenType::Colon) {
        advance(); // Consume :

        // Type expression
        typeAnnot = PARSE_OR_ERROR(ExprParser(_producer, _ast));

        end = typeAnnot->end; // Update end location
    }

    // Construct AST node
    Node* node = make<BindingPattern>(bind->start, end, mod, bind, typeAnnot);
    return Result<Node*, Error>::ok(node);
}

Result<Node*, Error> TuplePatternParser::parse() noexcept {
    // (
    Location start = ASSERT_TOKEN_TYPE(next(), TokenType::LParen).start;
    Location end;

    // Patterns
    std::vector<Node*> patterns;
    patterns.push_back(PARSE_OR_ERROR(PatternParser(_producer, _ast, _isLhs)));
    while (true) {
        // )
        if (peek().type == TokenType::RParen) {
            end = next().end;
            break;
        }

        // ,
        ASSERT_TOKEN_TYPE(next(), TokenType::Comma);

        // Pattern
        patterns.push_back(PARSE_OR_ERROR(PatternParser(_producer, _ast, _isLhs)));
    }

    // Construct AST node
    Node* node = patterns.size() == 1 ? patterns[0] : make<TuplePattern>(start, end, std::move(patterns));
    return Result<Node*, Error>::ok(node);
}

Result<Node*, Error> CollectionPatternParser::parse() noexcept {
    // [
    Location start = ASSERT_TOKEN_TYPE(next(), TokenType::LBracket).start;
    Location end;

    // ]
    if (peek().type == TokenType::RBracket) {
        advance(); // Consume ]
        return Result<Node*, Error>::ok(make<CollectionPattern>(start, end));
    }

    std::vector<Node*> prefix;
    std::vector<Node*> suffix;
    bool hasRest = false;
    std::vector<Node*>* patterns = &prefix;

    // Parse the first pattern
    if (peek().type == TokenType::Range) {
        advance(); // Consume ...
        hasRest = true;
        patterns = &suffix;
    } else {
        patterns->push_back(PARSE_OR_ERROR(PatternParser(_producer, _ast, _isLhs)));
    }

    // Parse the rest
    while (true) {
        // ]
        if (peek().type == TokenType::RBracket) {
            end = next().end;
            break;
        }

        // ,
        ASSERT_TOKEN_TYPE(next(), TokenType::Comma);

        // ... or pattern
        if (peek().type == TokenType::Range) {
            if (hasRest) {
                return Result<Node*, Error>::err("syntax error: collection pattern may contain at most one '...'",
                    peek().start, peek().end);
            }

            advance(); // Consume ...
            hasRest = true;
            patterns = &suffix;
        } else {
            patterns->push_back(PARSE_OR_ERROR(PatternParser(_producer, _ast, _isLhs)));
        }
    }

    // Construct AST node
    Node* node = make<CollectionPattern>(start, end, std::move(prefix), std::move(suffix), hasRest);
    return Result<Node*, Error>::ok(node);
}

Result<Node*, Error> RecordPatternParser::parse() noexcept {
    // TODO: Implement the actual record pattern parser
    return BindingPatternParser(_producer, _ast).parse();
}

Result<Node*, Error> QualifiedNameParser::parse() noexcept {
    const Token& token = next();
    if (token.type == TokenType::Identifier) {
        Node* node = make<Identifier>(token.start, token.end, std::move(token.lexeme));
        return Result<Node*, Error>::ok(node);
    }
    if (token.type == TokenType::Discard) {
        Node* node = make<Discard>(token.start, token.end);
        return Result<Node*, Error>::ok(node);
    }
    return unexpectedToken(token);
}

Result<Node*, Error> LiteralParser::parse() noexcept {
    const Token& token = next();
    Node* node;
    switch (token.type) {
        case TokenType::Integer:
            node = make<IntLiteral>(token.start, token.end, BigInt(token.lexeme));
            break;

        case TokenType::Real:
            node = make<RealLiteral>(token.start, token.end, BigReal(token.lexeme));
            break;

        case TokenType::True:
            node = make<BoolLiteral>(token.start, token.end, true);
            break;

        case TokenType::False:
            node = make<BoolLiteral>(token.start, token.end, false);
            break;

        case TokenType::String:
            node = make<StringLiteral>(token.start, token.end, std::move(token.lexeme));
            break;

        case TokenType::Nil:
            node = make<NilLiteral>(token.start, token.end);
            break;

        case TokenType::Undefined:
            node = make<Undefined>(token.start, token.end);
            break;

        default:
            return unexpectedToken(token);
    }
    return Result<Node*, Error>::ok(node);
}

} // Spark::FrontEnd
