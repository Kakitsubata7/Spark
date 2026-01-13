#pragma once

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

#include "node_visitor.hpp"
#include "utils/bigint.hpp"
#include "utils/bigreal.hpp"
#include "utils/location.hpp"

namespace Spark::FrontEnd {

class NodeVisitor;
enum class OverloadableOpKind;

struct Node {
    Location start;
    Location end;

    Node(Location start, Location end) noexcept
        : start(start), end(end) { }

    virtual ~Node() = default;

    virtual void accept(NodeVisitor& v) = 0;

    bool operator==(const Node& rhs) const noexcept;
    bool operator!=(const Node& rhs) const noexcept;

protected:
    [[nodiscard]]
    virtual bool equalsImpl(const Node& rhs) const noexcept = 0;
};

struct Expr : Node {
    Expr(Location start, Location end) : Node(start, end) { }

    void accept(NodeVisitor &v) override { v.visit(*this); }
};

struct Stmt : Node {
    Stmt(Location start, Location end) : Node(start, end) { }

    void accept(NodeVisitor &v) override { v.visit(*this); }
};

struct Pattern : Node {
    Pattern(Location start, Location end) : Node(start, end) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct Name : Node {
    Name(Location start, Location end) : Node(start, end) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct IdentifierName final : Name {
    std::string name;

    IdentifierName(Location start, Location end, std::string name) noexcept
        : Name(start, end), name(std::move(name)) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct DiscardName final : Name {
    DiscardName(Location start, Location end) noexcept : Name(start, end) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct ConstructorName final : Name {
    ConstructorName(Location start, Location end) noexcept : Name(start, end) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct DestructorName final : Name {
    DestructorName(Location start, Location end) noexcept : Name(start, end) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct OverloadableOpName final : Name {
    OverloadableOpKind op;

    OverloadableOpName(Location start, Location end, OverloadableOpKind op) noexcept
        : Name(start, end), op(op) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct SelfName final : Name {
    SelfName(Location start, Location end) noexcept
        : Name(start, end) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};


struct VarModifier final : Node {
    enum class VarKind {
        None, Let, Const, Ref, Cref
    };

    enum class Optionality {
        None, Optional, OptionalNonNull
    };

    VarKind kind;
    bool isImmutable;
    Optionality optionality;

    VarModifier(Location start, Location end, VarKind kind, bool isImmutable, Optionality optionality) noexcept
        : Node(start, end), kind(kind), isImmutable(isImmutable), optionality(optionality) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct FnParam final : Node {
    /* nullable */ VarModifier* mod;
    Pattern* pattern;
    /* nullable */ Expr* type;
    /* nullable */ Expr* def;

    FnParam(Location start, Location end, VarModifier* mod, Pattern* pattern, Expr* type, Expr* def) noexcept
        : Node(start, end), mod(mod), pattern(pattern), type(type), def(def) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct FnCapture final : Node {
    /* nullable */ VarModifier* mod;
    Pattern* pattern;

    FnCapture(Location start, Location end, VarModifier* mod, Pattern* pattern) noexcept
        : Node(start, end), mod(mod), pattern(pattern) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct FnCaptureClause final : Node {
    std::vector<FnCapture*> captures;
    bool hasRest;
    /* nullable */ VarModifier* restMod;

    FnCaptureClause(Location start, Location end, std::vector<FnCapture*> captures, bool hasRest, VarModifier* restMod)
        noexcept : Node(start, end), captures(std::move(captures)), hasRest(hasRest), restMod(restMod) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct FnReturn final : Node {
    enum class RetKind {
        ByValue, ByRef, ByCref
    };

    RetKind kind;
    Expr* type;

    FnReturn(Location start, Location end, RetKind kind, Expr* type) noexcept
        : Node(start, end), kind(kind), type(type) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct LambdaExpr final : Expr {
    bool isImmutable;
    std::vector<FnParam*> params;
    /* nullable */ FnCaptureClause* captureClause;
    std::vector<FnReturn*> returns;
    bool isThrowing;
    /* nullable */ Expr* throwExpr;
    Expr* body;

    LambdaExpr(Location start, Location end, bool isImmutable,
               std::vector<FnParam*> params, FnCaptureClause* captureClause,
               std::vector<FnReturn*> returns, bool isThrowing, Expr* throwExpr,
               Expr* body) noexcept
        : Expr(start, end), isImmutable(isImmutable), params(std::move(params)), captureClause(captureClause),
          returns(std::move(returns)), isThrowing(isThrowing), throwExpr(throwExpr), body(body) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct IfThenExpr final : Expr {
    Expr* condition;
    Expr* thenExpr;
    Expr* elseExpr;

    IfThenExpr(Location start, Location end, Expr* condition, Expr* thenExpr, Expr* elseExpr) noexcept
        : Expr(start, end), condition(condition), thenExpr(thenExpr), elseExpr(elseExpr) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct TryElseExpr final : Expr {
    Expr* tryExpr;
    Expr* elseExpr;

    TryElseExpr(Location start, Location end, Expr* tryExpr, Expr* elseExpr) noexcept
        : Expr(start, end), tryExpr(tryExpr), elseExpr(elseExpr) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct MatchCase final : Node {
    /* nullable */ Pattern* pattern;
    /* nullable */ Expr* guard;
    Expr* body;

    MatchCase(Location start, Location end, Pattern* pattern, Expr* guard, Expr* body) noexcept
        : Node(start, end), pattern(pattern), guard(guard), body(body) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct MatchExpr final : Expr {
    Expr* scrutinee;
    std::vector<MatchCase*> cases;

    MatchExpr(Location start, Location end, Expr* scrutinee, std::vector<MatchCase*> cases) noexcept
        : Expr(start, end), scrutinee(scrutinee), cases(std::move(cases)) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct CatchClause final : Node {
    /* nullable */ Pattern* pattern;
    /* nullable */ Expr* guard;
    Expr* body;

    CatchClause(Location start, Location end, Pattern* pattern, Expr* guard, Expr* body) noexcept
        : Node(start, end), pattern(pattern), guard(guard), body(body) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct TryCatchExpr final : Expr {
    Expr* expr;
    std::vector<CatchClause*> catches;

    TryCatchExpr(Location start, Location end, Expr* expr, std::vector<CatchClause*> catches) noexcept
        : Expr(start, end), expr(expr), catches(std::move(catches)) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct ThrowExpr final : Expr {
    Expr* expr;

    ThrowExpr(Location start, Location end, Expr* expr) noexcept
        : Expr(start, end), expr(expr) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct BlockExpr final : Expr {
    std::vector<Stmt*> stmts;

    BlockExpr(Location start, Location end, std::vector<Stmt*> stmts) noexcept
        : Expr(start, end), stmts(std::move(stmts)) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct IsExpr final : Expr {
    Expr* expr;
    Expr* type;

    IsExpr(Location start, Location end, Expr* expr, Expr* type) noexcept
        : Expr(start, end), expr(expr), type(type) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct AsExpr final : Expr {
    Expr* expr;
    Pattern* pattern;
    Expr* type;

    AsExpr(Location start, Location end, Expr* expr, Pattern* pattern, Expr* type) noexcept
        : Expr(start, end), expr(expr), pattern(pattern), type(type) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct BinaryExpr final : Expr {
    enum class OpKind {
        Mul, Div, Mod,
        Add, Sub,
        BitShl, BitShr,
        Lt, Le, Gt, Ge,
        Eq, Ne, StrictEq, StrictNe,
        BitAnd,
        BitXor,
        BitOr,
        LogAnd,
        LogOr,
        Coalesce,
        Range, RangeExcl,
        FuncType,
        Pipe
    };

    OpKind op;
    Expr* lhs;
    Expr* rhs;

    BinaryExpr(Location start, Location end, OpKind op, Expr* lhs, Expr* rhs) noexcept
        : Expr(start, end), op(op), lhs(lhs), rhs(rhs) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct PrefixExpr final : Expr {
    enum class OpKind {
        Pos, Neg, BitNot, LogNot, Immut
    };

    OpKind op;
    Expr* expr;

    PrefixExpr(Location start, Location end, OpKind op, Expr* expr) noexcept
        : Expr(start, end), op(op), expr(expr) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct PostfixExpr final : Expr {
    enum class OpKind {
        Optional, NonNull, ForceUnwrap
    };

    OpKind op;
    Expr* expr;

    PostfixExpr(Location start, Location end, OpKind op, Expr* expr) noexcept
        : Expr(start, end), op(op), expr(expr) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct MemberAccessExpr final : Expr {
    Expr* base;
    Name* member;

    MemberAccessExpr(Location start, Location end, Expr* base, Name* member) noexcept
        : Expr(start, end), base(base), member(member) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct CallArg final : Node {
    /* nullable */ IdentifierName* name;
    Expr* expr;

    CallArg(Location start, Location end, IdentifierName* name, Expr* expr) noexcept
        : Node(start, end), name(name), expr(expr) { }
    CallArg(Location start, Location end, Expr* expr) noexcept
        : CallArg(start, end, nullptr, expr) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct CallExpr final : Expr {
    Expr* callee;
    std::vector<CallArg*> args;

    CallExpr(Location start, Location end, Expr* callee, std::vector<CallArg*> args) noexcept
        : Expr(start, end), callee(callee), args(std::move(args)) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct SubscriptExpr final : Expr {
    Expr* base;
    std::vector<Expr*> indices;

    SubscriptExpr(Location start, Location end, Expr* callee, std::vector<Expr*> indices) noexcept
        : Expr(start, end), base(callee), indices(std::move(indices)) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct IntLiteralExpr final : Expr {
    BigInt value;

    IntLiteralExpr(Location start, Location end, BigInt value) noexcept
            : Expr(start, end), value(std::move(value)) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct RealLiteralExpr final : Expr {
    BigReal value;

    RealLiteralExpr(Location start, Location end, BigReal value) noexcept
            : Expr(start, end), value(std::move(value)) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct BoolLiteralExpr final : Expr {
    bool value;

    BoolLiteralExpr(Location start, Location end, bool value) noexcept
            : Expr(start, end), value(value) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct StringLiteralExpr final : Expr {
    std::string value;

    StringLiteralExpr(Location start, Location end, std::string value) noexcept
            : Expr(start, end), value(std::move(value)) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct NilLiteralExpr final : Expr {
    NilLiteralExpr(Location start, Location end) noexcept : Expr(start, end) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct VoidLiteralExpr final : Expr {
    VoidLiteralExpr(Location start, Location end) noexcept : Expr(start, end) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct IdentifierExpr final : Expr {
    std::string name;

    IdentifierExpr(Location start, Location end, std::string name) noexcept
            : Expr(start, end), name(std::move(name)) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct ConstructorExpr final : Expr {
    ConstructorExpr(Location start, Location end) noexcept : Expr(start, end) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct DestructorExpr final : Expr {
    DestructorExpr(Location start, Location end) noexcept : Expr(start, end) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct OverloadableOpExpr final : Expr {
    OverloadableOpKind op;

    OverloadableOpExpr(Location start, Location end, OverloadableOpKind op) noexcept
        : Expr(start, end), op(op) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct SelfExpr final : Expr {
    SelfExpr(Location start, Location end) noexcept : Expr(start, end) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct GlobalAccessExpr final : Expr {
    Name* name;

    GlobalAccessExpr(Location start, Location end, Name* name) noexcept
        : Expr(start, end), name(name) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct UpvalueExpr final : Expr {
    uint64_t level;
    Name* name;

    UpvalueExpr(Location start, Location end, uint64_t level, Name* name) noexcept
        : Expr(start, end), level(level), name(name) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct TupleExpr final : Expr {
    std::vector<Expr*> exprs;

    TupleExpr(Location start, Location end, std::vector<Expr*> exprs) noexcept
        : Expr(start, end), exprs(std::move(exprs)) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct CollectionExpr final : Expr {
    std::vector<Expr*> exprs;

    CollectionExpr(Location start, Location end, std::vector<Expr*> exprs) noexcept
        : Expr(start, end), exprs(std::move(exprs)) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct TypeofExpr final : Expr {
    Expr* expr;

    TypeofExpr(Location start, Location end, Expr* expr) noexcept
        : Expr(start, end), expr(expr) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};



struct VarDefStmt final : Stmt {
    VarModifier* mod;
    Pattern* pattern;
    /* nullable */ Expr* type;
    /* nullable */ Node* rhs;

    VarDefStmt(Location start, Location end, VarModifier* mod, Pattern* pattern,
               Expr* type = nullptr, Node* rhs = nullptr) noexcept
        : Stmt(start, end), mod(mod), pattern(pattern), type(type), rhs(rhs) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct AssignStmt final : Stmt {
    enum class OpKind {
        Assign,
        AddAssign, SubAssign, MulAssign, DivAssign, ModAssign,
        BitAndAssign, BitOrAssign, BitXorAssign, BitShlAssign, BitShrAssign,
        CoalesceAssign
    };

    OpKind op;
    Node* lhs;
    Node* rhs;

    AssignStmt(Location start, Location end, OpKind op, Node* lhs, Node* rhs) noexcept
        : Stmt(start, end), op(op), lhs(lhs), rhs(rhs) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct FnDefStmt final : Stmt {
    bool isImmutable;
    Name* name;
    std::vector<Expr*> generics;
    std::vector<FnParam*> params;
    /* nullable */ FnCaptureClause* captureClause;
    std::vector<FnReturn*> returns;
    bool isThrowing;
    /* nullable */ Expr* throwExpr;
    Expr* body;

    FnDefStmt(Location start, Location end, bool isImmutable, Name* name, std::vector<Expr*> generics,
              std::vector<FnParam*> params, FnCaptureClause* captureClause,
              std::vector<FnReturn*> returns, bool isThrowing, Expr* throwExpr,
              Expr* body) noexcept
        : Stmt(start, end), isImmutable(isImmutable), name(name), generics(std::move(generics)),
          params(std::move(params)), captureClause(captureClause), returns(std::move(returns)),
          isThrowing(isThrowing), throwExpr(throwExpr), body(body) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct TypeDefStmt final : Stmt {
    enum class TypeKind {
        Struct, Class, Enum, EnumStruct, EnumClass, Trait, Alias, Extension
    };

    TypeKind kind;
    bool isImmutable;
    Name* name;
    std::vector<Expr*> generics;
    std::vector<Expr*> bases;
    BlockExpr* body;

    TypeDefStmt(Location start, Location end,
                TypeKind kind, bool isImmutable, Name* name,
                std::vector<Expr*> generics, std::vector<Expr*> bases, BlockExpr* body) noexcept
        : Stmt(start, end), kind(kind), isImmutable(isImmutable), name(name), generics(std::move(generics)),
          bases(std::move(bases)), body(body) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct IfStmt final : Stmt {
    Node* condition;
    Node* thenBody;
    /* nullable */ Node* elseBody;

    IfStmt(Location start, Location end, Node* condition, Node* thenBody, Node* elseBody) noexcept
        : Stmt(start, end), condition(condition), thenBody(thenBody), elseBody(elseBody) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct WhileStmt final : Stmt {
    Expr* condition;
    BlockExpr* body;

    WhileStmt(Location start, Location end, Expr* condition, BlockExpr* body) noexcept
        : Stmt(start, end), condition(condition), body(body) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct ForStmt final : Stmt {
    Pattern* iterator;
    Expr* range;
    BlockExpr* body;

    ForStmt(Location start, Location end, Pattern* iterator, Expr* range, BlockExpr* body) noexcept
        : Stmt(start, end), iterator(iterator), range(range), body(body) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct BreakStmt final : Stmt {
    BreakStmt(Location start, Location end) noexcept : Stmt(start, end) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct ContinueStmt final : Stmt {
    ContinueStmt(Location start, Location end) noexcept : Stmt(start, end) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct ReturnStmt final : Stmt {
    /* nullable */ Node* expr;

    ReturnStmt(Location start, Location end, Node* expr) noexcept
        : Stmt(start, end), expr(expr) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct ModuleStmt final : Stmt {
    std::vector<Name*> names;
    BlockExpr* body;

    ModuleStmt(Location start, Location end, std::vector<Name*> names, BlockExpr* body) noexcept
        : Stmt(start, end), names(std::move(names)), body(body) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct Export final : Node {
    Node* stmt;

    Export(Location start, Location end, Node* stmt) noexcept
        : Node(start, end), stmt(stmt) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct Undefine final : Node {
    Node* var;

    Undefine(Location start, Location end, Node* var) noexcept
        : Node(start, end), var(var) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct ExprStmt final : Stmt {
    Expr* expr;

    ExprStmt(Location start, Location end, Expr* expr) noexcept
        : Stmt(start, end), expr(expr) { }

    void accept(NodeVisitor &v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};



struct IntLiteralPattern final : Pattern {
    BigInt value;

    IntLiteralPattern(Location start, Location end, BigInt value) noexcept
            : Pattern(start, end), value(std::move(value)) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct RealLiteralPattern final : Pattern {
    BigReal value;

    RealLiteralPattern(Location start, Location end, BigReal value) noexcept
            : Pattern(start, end), value(std::move(value)) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct BoolLiteralPattern final : Pattern {
    bool value;

    BoolLiteralPattern(Location start, Location end, bool value) noexcept
            : Pattern(start, end), value(value) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct StringLiteralPattern final : Pattern {
    std::string value;

    StringLiteralPattern(Location start, Location end, std::string value) noexcept
            : Pattern(start, end), value(std::move(value)) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct NilLiteralPattern final : Pattern {
    NilLiteralPattern(Location start, Location end) noexcept : Pattern(start, end) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct VoidLiteralPattern final : Pattern {
    VoidLiteralPattern(Location start, Location end) noexcept : Pattern(start, end) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct BindingPattern final : Pattern {
    std::string name;

    BindingPattern(Location start, Location end, std::string name) noexcept
        : Pattern(start, end), name(std::move(name)) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct WildcardPattern final : Pattern {
    WildcardPattern(Location start, Location end) noexcept : Pattern(start, end) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct TuplePattern final : Pattern {
    std::vector<Pattern*> patterns;

    TuplePattern(Location start, Location end, std::vector<Pattern*> patterns) noexcept
        : Pattern(start, end), patterns(std::move(patterns)) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct CollectionPattern final : Pattern {
    std::vector<Pattern*> prefix;
    std::vector<Pattern*> suffix;
    bool isExact;

    CollectionPattern(Location start, Location end,
                      std::vector<Pattern*> prefix, std::vector<Pattern*> suffix, bool isExact) noexcept
        : Pattern(start, end), prefix(std::move(prefix)), suffix(std::move(suffix)), isExact(isExact) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};



enum class OverloadableOpKind {
    Add, Sub, Mul, Div, Mod, BitNot, BitAnd, BitOr, BitXor, BitShl, BitShr, LogNot, LogAnd, LogOr,
        Eq, Ne, Lt, Le, Gt, Ge, Range, RangeExcl, Call, Subscript,
    AddAssign, SubAssign, MulAssign, DivAssign, ModAssign, BitAndAssign, BitOrAssign, BitXorAssign, BitShlAssign,
        BitShrAssign
};

} // Spark::FrontEnd
