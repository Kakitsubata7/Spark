#pragma once

#include "node.hpp"
#include "node_visitor.hpp"

namespace Spark::FrontEnd::AST {

struct Expr : Node {
    Expr(size_t line, size_t column) : Node(line, column) { }
};

enum class UnaryOperator {
    None, Neg, BitNot, Ref, LogNot
};

struct UnaryExpr final : Expr {
    UnaryOperator rator;
    Expr* rand;

    UnaryExpr(size_t line, size_t column, UnaryOperator rator, Expr* rand) noexcept
        : Expr(line, column), rator(rator), rand(rand) { }

    void accept(NodeVisitor& v) override { return v.visit(*this); }
};

enum class BinaryOperator {
    None, Add, Sub, Mul, Div, Mod, BitAnd, BitOr, BitXor, Eq, Ne, Lt, Gt
};

struct BinaryExpr final : Expr {
    BinaryOperator rator;
    Expr* left;
    Expr* right;

    BinaryExpr(size_t line, size_t column, BinaryOperator rator, Expr* left, Expr* right) noexcept
        : Expr(line, column), rator(rator), left(left), right(right) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct IfThenExpr final : Expr {
    Expr* condition;
    Expr* trueExpr;
    Expr* falseExpr;

    IfThenExpr(size_t line, size_t column, Expr* condition, Expr* trueExpr, Expr* falseExpr) noexcept
        : Expr(line, column), condition(condition), trueExpr(trueExpr), falseExpr(falseExpr) { }

    void accept(NodeVisitor& v) override { return v.visit(*this); }
};

} // Spark::FrontEnd::AST
