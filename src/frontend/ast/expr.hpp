#pragma once

#include "node.hpp"
#include "node_visitor.hpp"
#include "pattern.hpp"
#include "type.hpp"
#include "utils.hpp"
#include "utils/bigint.hpp"
#include "utils/bigreal.hpp"

namespace Spark::FrontEnd::AST {

struct Expr : Node {
    Type* type = nullptr;
};

struct IntLiteralExpr final : Expr {
    BigInt value;

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct RealLiteralExpr final : Expr {
    BigReal value;

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct BoolLiteralExpr final : Expr {
    bool value = false;

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct StringLiteralExpr final : Expr {
    std::string value;

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct NilLiteralExpr final : Expr {
    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct VarExpr final : Expr {
    Name varName;

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct FnExpr final : Expr {
    struct Arg {

    };

    std::optional<Path> returnTypePath = std::nullopt;

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct LambdaExpr final : Expr {
    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct SequenceExpr final : Expr {
    std::vector<Expr*> exprs;

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct AssignExpr final : Expr {
    enum class OperatorType {
        None,
        Assign,
        AddAssign, SubAssign, MulAssign, DivAssign, ModAssign,
        BitAndAssign, BitOrAssign, BitXorAssign, BitShlAssign, BitShrAssign,
        CoalesceAssign
    };

    OperatorType rator = OperatorType::None;
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
    Expr* right = nullptr;

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

} // Spark::FrontEnd::AST
