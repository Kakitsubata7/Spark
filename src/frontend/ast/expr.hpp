#pragma once

#include "core/type.hpp"
#include "node.hpp"
#include "node_visitor.hpp"
#include "pattern.hpp"
#include "utils.hpp"
#include "utils/bigint.hpp"
#include "utils/bigreal.hpp"

namespace Spark::FrontEnd {

struct Expr : Node {
    Type* type = nullptr;

    Expr(Location start, Location end) noexcept : Node(start, end) { }
};

struct IntLiteralExpr final : Expr {
    BigInt value;

    IntLiteralExpr(Location start, Location end, BigInt value)
        : Expr(start, end), value(std::move(value)) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct RealLiteralExpr final : Expr {
    BigReal value;

    RealLiteralExpr(Location start, Location end, BigReal value)
        : Expr(start, end), value(std::move(value)) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct BoolLiteralExpr final : Expr {
    bool value = false;

    BoolLiteralExpr(Location start, Location end, bool value) noexcept
        : Expr(start, end), value(value) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct StringLiteralExpr final : Expr {
    std::string value;

    StringLiteralExpr(Location start, Location end, std::string value) noexcept
        : Expr(start, end), value(std::move(value)) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct NilLiteralExpr final : Expr {
    NilLiteralExpr(Location start, Location end) noexcept : Expr(start, end) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct VarExpr final : Expr {
    std::string name;

    VarExpr(Location start, Location end, std::string name) noexcept
        : Expr(start, end), name(std::move(name)) { }

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

    AssignExpr(Location start, Location end, AssignType rator, Expr* target, Expr* expr) noexcept
        : Expr(start, end), rator(rator), target(target), expr(expr) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct UnaryExpr final : Expr {
    enum class OperatorType {
        None, Pos, Neg, BitNot, LogNot, Ref
    };

    OperatorType rator = OperatorType::None;
    Expr* rand = nullptr;

    UnaryExpr(Location start, Location end, OperatorType rator, Expr* rand) noexcept
        : Expr(start, end), rator(rator), rand(rand) { }

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
        StrictEq, StrictNe,
        Coalesce
    };

    OperatorType rator = OperatorType::None;
    Expr* left = nullptr;
    Expr* right = nullptr;

    BinaryExpr(Location start, Location end, OperatorType rator, Expr* left, Expr* right) noexcept
        : Expr(start, end), rator(rator), left(left), right(right) { }

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

    IfThenExpr(Location start, Location end, Expr* condition, Expr* trueExpr, Expr* falseExpr) noexcept
        : Expr(start, end), condition(condition), trueExpr(trueExpr), falseExpr(falseExpr) { }

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

} // Spark::FrontEnd
