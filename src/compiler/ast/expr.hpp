#pragma once

#include "node.hpp"
#include "node_visitor.hpp"
#include "pattern.hpp"
#include "type.hpp"
#include "utils.hpp"
#include "utils/bigint.hpp"
#include "utils/bigreal.hpp"

namespace Spark::Compiler::AST {

struct Expr : Node {
    Type* type = nullptr;

    Expr(size_t line, size_t column) noexcept : Node(line, column) { }
};

struct IntLiteralExpr final : Expr {
    BigInt value;

    IntLiteralExpr(size_t line, size_t column, BigInt value)
        : Expr(line, column), value(std::move(value)) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct RealLiteralExpr final : Expr {
    BigReal value;

    RealLiteralExpr(size_t line, size_t column, BigReal value)
        : Expr(line, column), value(std::move(value)) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct BoolLiteralExpr final : Expr {
    bool value = false;

    BoolLiteralExpr(size_t line, size_t column, bool value) noexcept
        : Expr(line, column), value(value) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct StringLiteralExpr final : Expr {
    std::string value;

    StringLiteralExpr(size_t line, size_t column, std::string value) noexcept
        : Expr(line, column), value(std::move(value)) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct NilLiteralExpr final : Expr {
    NilLiteralExpr(size_t line, size_t column) noexcept : Expr(line, column) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct VarExpr final : Expr {
    std::string name;

    VarExpr(size_t line, size_t column, std::string name) noexcept
        : Expr(line, column), name(std::move(name)) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct FnExpr final : Expr {
    std::vector<ArgDecl> args;
    std::optional<Path> returnTypePath = std::nullopt;
    BlockStmt* body = nullptr;

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct LambdaExpr final : Expr {
    std::vector<ArgDecl> args;
    std::optional<Path> returnTypePath = std::nullopt;
    std::vector<Node*> body;

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct AssignExpr final : Expr {
    AssignType rator = AssignType::None;
    Expr* target = nullptr;
    Expr* expr = nullptr;

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct UnaryExpr final : Expr {
    enum class OperatorType {
        None, Neg, BitNot, LogNot, Ref
    };

    OperatorType rator = OperatorType::None;
    Expr* rand = nullptr;

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct ForceUnwrapExpr final : Expr {
    Expr* expr = nullptr;

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct BinaryExpr final : Expr {
    enum class OperatorType {
        None,
        Add, Sub, Mul, Div, Mod,
        BitAnd, BitOr, BitXor, BitShl, BitShr,
        LogAnd, LogOr,
        Eq, Ne, Lt, Gt, Le, Ge,
        Coalesce
    };

    OperatorType rator = OperatorType::None;
    Expr* left = nullptr;
    Expr* right = nullptr;

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct FieldAccessExpr final : Expr {
    Expr* left = nullptr;
    Name right;

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct CallExpr final : Expr {
    Expr* func = nullptr;
    std::vector<Expr*> args;

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct SubscriptExpr final : Expr {
    Expr* expr = nullptr;
    Expr* index = nullptr;

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct RangeExpr final : Expr {
    Expr* begin = nullptr;
    Expr* end = nullptr;
    Expr* step = nullptr;
    bool isExclusive = false;

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct IfThenExpr final : Expr {
    Expr* condition = nullptr;
    Expr* trueExpr = nullptr;
    Expr* falseExpr = nullptr;

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct MatchExpr final : Expr {
    struct CaseClause {
        Pattern* pattern = nullptr;
        Expr* guard = nullptr;
        Expr* expr = nullptr;
    };

    Expr* scrutinee = nullptr;
    std::optional<VarDecl> asVar = std::nullopt;
    std::vector<CaseClause> cases;

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

} // Spark::Compiler::AST
