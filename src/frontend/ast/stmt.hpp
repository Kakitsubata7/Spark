#pragma once

#include <vector>

#include "expr.hpp"
#include "node.hpp"
#include "node_visitor.hpp"

namespace Spark::FrontEnd::AST {

struct Stmt : Node {
    Stmt(size_t line, size_t column) noexcept : Node(line, column) { }
};

struct BlockStmt final : Stmt {
    std::vector<Stmt> stmts;

    BlockStmt(size_t line, size_t column, std::vector<Stmt> stmts) noexcept
        : Stmt(line, column), stmts(std::move(stmts)) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct LetStmt final : Stmt {
    IdentifierNode* identifier;
    Expr* expr;

    LetStmt(size_t line, size_t column, IdentifierNode* identifier, Expr* expr) noexcept
        : Stmt(line, column), identifier(identifier), expr(expr) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

} // Spark::FrontEnd::AST
