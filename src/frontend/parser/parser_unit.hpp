#pragma once

#include "frontend/ast.hpp"
#include "token_producer.hpp"
#include "utils/error.hpp"
#include "utils/result.hpp"

namespace Spark::FrontEnd {

class ParserUnit {
protected:
    TokenProducer& _producer;
    AST& _ast;

public:
    ParserUnit(TokenProducer& producer, AST& ast) noexcept : _producer(producer), _ast(ast) { }
    virtual ~ParserUnit() = default;

    virtual Result<Node*, Error> parse() noexcept = 0;

protected:
    void advance() const { _producer.next(); }

    [[nodiscard]]
    const Token& next() const { return _producer.next(); }

    [[nodiscard]]
    const Token& peek() const { return _producer.peek(); }

    template <typename T, typename... Args>
    T* make(Args&&... args) {
        return _ast.make<T>(std::forward<Args>(args)...);
    }
};

class BodyParser final : public ParserUnit {
private:
    TokenType _terminatorType;

public:
    BodyParser(TokenProducer& producer, AST& ast, TokenType terminatorType) noexcept
        : ParserUnit(producer, ast), _terminatorType(terminatorType) { }

    Result<Node*, Error> parse() noexcept override;
};

class ExprParser final : public ParserUnit {
public:
    ExprParser(TokenProducer& producer, AST& ast) noexcept : ParserUnit(producer, ast) { }

    Result<Node*, Error> parse() noexcept override;

private:
    Result<Node*, Error> parse(int minBp) noexcept;
    Result<Node*, Error> parseNud() noexcept;

    static PrefixOp tokenToPrefix(TokenType t) noexcept;
    static int prefixBp(PrefixOp op) noexcept;

    static PostfixOp tokenToPostfix(TokenType t) noexcept;
    static int postfixBp(PostfixOp op) noexcept;

    static InfixOp tokenToInfix(TokenType t) noexcept;
    static std::pair<int, int> infixBp(InfixOp op) noexcept;
};

class BlockParser final : public ParserUnit {
public:
    BlockParser(TokenProducer& producer, AST& ast) noexcept : ParserUnit(producer, ast) { }

    Result<Node*, Error> parse() noexcept override;
};

class ExprBlockParser final : public ParserUnit {
public:
    ExprBlockParser(TokenProducer& producer, AST& ast) noexcept : ParserUnit(producer, ast) { }

    Result<Node*, Error> parse() noexcept override;
};

class AssignParser final : public ParserUnit {
public:
    AssignParser(TokenProducer& producer, AST& ast) noexcept : ParserUnit(producer, ast) { }

    Result<Node*, Error> parse() noexcept override;
};

class IfElseParser final : public ParserUnit {
public:
    IfElseParser(TokenProducer& producer, AST& ast) noexcept : ParserUnit(producer, ast) { }

    Result<Node*, Error> parse() noexcept override;
};

class WhileParser final : public ParserUnit {
public:
    WhileParser(TokenProducer& producer, AST& ast) noexcept : ParserUnit(producer, ast) { }

    Result<Node*, Error> parse() noexcept override;
};

class ForParser final : public ParserUnit {
public:
    ForParser(TokenProducer& producer, AST& ast) noexcept : ParserUnit(producer, ast) { }

    Result<Node*, Error> parse() noexcept override;
};

class IfThenParser final : public ParserUnit {
public:
    IfThenParser(TokenProducer& producer, AST& ast) noexcept : ParserUnit(producer, ast) { }

    Result<Node*, Error> parse() noexcept override;
};

class PrimaryParser final : public ParserUnit {
public:
    PrimaryParser(TokenProducer& producer, AST& ast) noexcept : ParserUnit(producer, ast) { }

    Result<Node*, Error> parse() noexcept override;
};

class ParenthesisExprParser final : public ParserUnit {
public:
    ParenthesisExprParser(TokenProducer& producer, AST& ast) noexcept : ParserUnit(producer, ast) { }

    Result<Node*, Error> parse() noexcept override;
};

class PatternParser final : public ParserUnit {
private:
    bool _isLhs;

public:
    PatternParser(TokenProducer& producer, AST& ast, bool isLhs = false) noexcept
        : ParserUnit(producer, ast), _isLhs(isLhs) { }

    Result<Node*, Error> parse() noexcept override;
};

class LhsPatternParser final : public ParserUnit {
public:
    LhsPatternParser(TokenProducer& producer, AST& ast) noexcept : ParserUnit(producer, ast) { }

    Result<Node*, Error> parse() noexcept override;
};

/**
 * Represents a parser for patterns like: x: T, _: T, let x: T, const^ x: T.\n
 * If no modifier is explicitly specified, it defaults to const^ (non-reassignable, immutable).
 * Patterns produced by this parser are guaranteed to be LHS patterns.\n
 * Not allowed:\n
 * (x): T\n
 * (x: T) <- This is a parenthesis pattern
 */
class BindingPatternParser final : public ParserUnit {
public:
    BindingPatternParser(TokenProducer& producer, AST& ast) noexcept : ParserUnit(producer, ast) { }

    Result<Node*, Error> parse() noexcept override;
};

class TuplePatternParser final : public ParserUnit {
private:
    bool _isLhs;

public:
    TuplePatternParser(TokenProducer& producer, AST& ast, bool isLhs = false) noexcept
        : ParserUnit(producer, ast), _isLhs(isLhs) { }

    Result<Node*, Error> parse() noexcept override;
};

/**
 * [...], [x, ...], [..., x], [x, ..., y]
 */
class CollectionPatternParser final : public ParserUnit {
private:
    bool _isLhs;

public:
    CollectionPatternParser(TokenProducer& producer, AST& ast, bool isLhs = false) noexcept
        : ParserUnit(producer, ast), _isLhs(isLhs) { }

    Result<Node*, Error> parse() noexcept override;
};

class RecordPatternParser final : public ParserUnit {
private:
    bool _isLhs;

public:
    RecordPatternParser(TokenProducer& producer, AST& ast, bool isLhs = false) noexcept
        : ParserUnit(producer, ast), _isLhs(isLhs) { }

    Result<Node*, Error> parse() noexcept override;
};

class QualifiedNameParser final : public ParserUnit {
public:
    QualifiedNameParser(TokenProducer& producer, AST& ast) noexcept : ParserUnit(producer, ast) { }

    Result<Node*, Error> parse() noexcept override;
};

class LiteralParser final : public ParserUnit {
public:
    LiteralParser(TokenProducer& producer, AST& ast) noexcept : ParserUnit(producer, ast) { }

    Result<Node*, Error> parse() noexcept override;
};

} // Spark::FrontEnd
