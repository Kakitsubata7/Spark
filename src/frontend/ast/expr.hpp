#pragma once

#include "identifier.hpp"
#include "node.hpp"
#include "node_visitor.hpp"
#include "pattern.hpp"
#include "type.hpp"

namespace Spark::FrontEnd::AST {

struct Expr : Node {
    Type* type = nullptr;
};

struct VarExpr final : Expr {
    Name varName;

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct SequenceExpr final : Expr {
    std::vector<Expr*> exprs;

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct AssignExpr final : Expr {
    Expr* target = nullptr;

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct UnaryExpr final : Expr {
    enum class OperatorType {
        None, Neg, BitNot, Ref, LogNot
    };

    OperatorType rator = OperatorType::None;
    Expr* rand = nullptr;

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct BinaryExpr final : Expr {
    enum class OperatorType {
        None, Add, Sub, Mul, Div, Mod, BitAnd, BitOr, BitXor, Eq, Ne, Lt, Gt
    };

    OperatorType rator = OperatorType::None;
    Expr* left = nullptr;
    Expr* right = nullptr;

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct IfThenExpr final : Expr {
    Expr* condition = nullptr;
    Expr* trueExpr = nullptr;
    Expr* falseExpr = nullptr;

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct ThrowExpr final : Expr {
    Expr* thrown = nullptr;

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct TryElseExpr final : Expr {
    Expr* attempt = nullptr;
    Expr* fallback = nullptr;

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

} // Spark::FrontEnd::AST
