#include "parser_unit.hpp"

#include <string>
#include <typeinfo>

#include "frontend/ast.hpp"

/**
 * Helper macro that takes a callable and tries parsing.
 * If the result is an error, it returns the error in the caller's scope.
 * Otherwise, it gives the value (`Node*`) of the result.
 * @param parser `ParserUnit` instance.
 */
#define PARSE_OR_ERROR(parse)    \
({                                \
    auto&& _r = (parse)(); \
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
                node = PARSE_OR_ERROR(BlockParser(_producer, _ast).parse);
                break;
            }

            // If-else statement / if-then expression
            case TokenType::If: {
                // If-else statement
                RewindTokenProducer producer(_producer);
                Result<Node*, Error> result = IfElseParser(producer, _ast).parse();
                if (result.hasValue()) {
                    return result;
                }

                // If-then expression
                producer.rewind();
                node = PARSE_OR_ERROR(IfThenParser(producer, _ast).parse);
                break;
            }

            // Match statement / match expression

            // While loop
            case TokenType::While: {
                node = PARSE_OR_ERROR(WhileParser(_producer, _ast).parse);
                break;
            }

            // Break
            case TokenType::Break: {
                node = make<BreakStmt>(token.start, token.end);
                break;
            }

            // Continue
            case TokenType::Continue: {
                node = make<ContinueStmt>(token.start, token.end);
                break;
            }

            // ;
            case TokenType::Semicolon:
                break;

            default:
                return unexpectedToken(token);
        }
        nodes.push_back(node);

        // Expression can only follow a semicolon
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
    return Result<Node*, Error>::ok(make<IntLiteral>(Location(), Location(), BigInt()));
}

Result<Node*, Error> BlockParser::parse() noexcept {
    // {
    const Token& lBrace = ASSERT_TOKEN_TYPE(next(), TokenType::LBrace);

    // Body
    Body* body = static_cast<Body*>(PARSE_OR_ERROR(BodyParser(_producer, _ast, TokenType::RBrace).parse));

    // }
    const Token& rBrace = ASSERT_TOKEN_TYPE(next(), TokenType::RBrace);

    // Construct AST node
    BlockStmt* block = make<BlockStmt>(lBrace.start, rBrace.end, body);
    return Result<Node*, Error>::ok(block);
}

Result<Node*, Error> IfElseParser::parse() noexcept {
    // Helper
    Location start = peek().start;
    Location end = start;
    std::vector<IfElseStmt::Branch> branches;
    auto parseBranch = [&]() -> Result<Node*, Error> {
        Node* cond = PARSE_OR_ERROR(ExprParser(_producer, _ast).parse);
        BlockStmt* block = static_cast<BlockStmt*>(PARSE_OR_ERROR(BlockParser(_producer, _ast).parse));
        branches.push_back({ .condition = cond, .block = block });
        end = block->end;
        return Result<Node*, Error>::ok(nullptr);
    };

    // If branch
    ASSERT_TOKEN_TYPE(next(), TokenType::If);
    PARSE_OR_ERROR(parseBranch);

    // Else if / else branch(es)
    BlockStmt* elseBlock = nullptr;
    while (peek().type == TokenType::Else) {
        advance(); // Consume else

        if (peek().type == TokenType::If) {
            advance(); // Consume if
            PARSE_OR_ERROR(parseBranch);
            continue;
        }

        // else
        elseBlock = static_cast<BlockStmt*>(PARSE_OR_ERROR(BlockParser(_producer, _ast).parse));
        end = elseBlock->end;
        break;
    }

    // Construct AST node
    Node* node = make<IfElseStmt>(start, end, std::move(branches), elseBlock);
    return Result<Node*, Error>::ok(node);
}

Result<Node*, Error> WhileParser::parse() noexcept {
    // while
    Location start = peek().start;
    ASSERT_TOKEN_TYPE(next(), TokenType::While);

    // Condition expression
    Node* condition = PARSE_OR_ERROR(ExprParser(_producer, _ast).parse);

    // Body block
    BlockStmt* block = static_cast<BlockStmt*>(PARSE_OR_ERROR(BlockParser(_producer, _ast).parse));

    // Construct AST node
    Node* node = make<WhileStmt>(start, block->end, condition, block);
    return Result<Node*, Error>::ok(node);
}

Result<Node*, Error> ForParser::parse() noexcept {
    // for
    Location start = peek().start;
    ASSERT_TOKEN_TYPE(next(), TokenType::For);

    // Iterator pattern
    Node* iterator = PARSE_OR_ERROR(LhsPatternParser(_producer, _ast).parse);

    // in
    ASSERT_TOKEN_TYPE(next(), TokenType::In);

    // Range expression
    Node* range = PARSE_OR_ERROR(ExprParser(_producer, _ast).parse);

    // Body block
    BlockStmt* block = static_cast<BlockStmt*>(PARSE_OR_ERROR(BlockParser(_producer, _ast).parse));

    // Construct AST node
    Node* node = make<ForStmt>(start, block->end, iterator, range, block);
    return Result<Node*, Error>::ok(node);
}

Result<Node*, Error> IfThenParser::parse() noexcept {
    // if
    Location start = peek().start;
    ASSERT_TOKEN_TYPE(next(), TokenType::If);

    // Condition
    Node* cond = PARSE_OR_ERROR(ExprParser(_producer, _ast).parse);

    // then
    ASSERT_TOKEN_TYPE(next(), TokenType::Then);

    // True
    Node* trueNode;
    {
        RewindTokenProducer producer(_producer);
        Result<Node*, Error> result = BlockParser(producer, _ast).parse();
        if (result.hasValue()) {
            trueNode = result.value();
        } else {
            producer.rewind();
            trueNode = PARSE_OR_ERROR(ExprParser(producer, _ast).parse);
        }
    }

    // else
    ASSERT_TOKEN_TYPE(next(), TokenType::Else);

    // False
    Node* falseNode;
    {
        RewindTokenProducer producer(_producer);
        Result<Node*, Error> result = BlockParser(producer, _ast).parse();
        if (result.hasValue()) {
            falseNode = result.value();
        } else {
            producer.rewind();
            falseNode = PARSE_OR_ERROR(ExprParser(producer, _ast).parse);
        }
    }

    // Construct AST node
    Node* node = make<IfThenExpr>(start, falseNode->end, cond, trueNode, falseNode);
    return Result<Node*, Error>::ok(node);
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
            bind = PARSE_OR_ERROR(QualifiedNameParser(_producer, _ast).parse);
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
        typeAnnot = PARSE_OR_ERROR(ExprParser(_producer, _ast).parse);

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
    patterns.push_back(PARSE_OR_ERROR(PatternParser(_producer, _ast, _isLhs).parse));
    while (true) {
        // )
        if (peek().type == TokenType::RParen) {
            end = next().end;
            break;
        }

        // ,
        ASSERT_TOKEN_TYPE(next(), TokenType::Comma);

        // Pattern
        patterns.push_back(PARSE_OR_ERROR(PatternParser(_producer, _ast, _isLhs).parse));
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
        patterns->push_back(PARSE_OR_ERROR(PatternParser(_producer, _ast, _isLhs).parse));
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
            patterns->push_back(PARSE_OR_ERROR(PatternParser(_producer, _ast, _isLhs).parse));
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
