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
};

class BlockParser final : public ParserUnit {
public:
    BlockParser(TokenProducer& producer, AST& ast) noexcept : ParserUnit(producer, ast) { }

    Result<Node*, Error> parse() noexcept override;
};

class VarDefParser final : public ParserUnit {
public:
    VarDefParser(TokenProducer& producer, AST& ast) noexcept : ParserUnit(producer, ast) { }

    Result<Node*, Error> parse() noexcept override;
};

class WhileParser final : public ParserUnit {
public:
    WhileParser(TokenProducer& producer, AST& ast) noexcept : ParserUnit(producer, ast) { }

    Result<Node*, Error> parse() noexcept override;
};

} // Spark::FrontEnd
