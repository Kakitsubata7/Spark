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

struct IntLiteral {
    BigInt value;

    explicit IntLiteral(BigInt value) noexcept : value(std::move(value)) { }

    bool operator==(const IntLiteral& rhs) const noexcept { return value == rhs.value; }
    bool operator!=(const IntLiteral& rhs) const noexcept { return !(*this == rhs); }
};

struct RealLiteral {
    BigReal value;

    explicit RealLiteral(BigReal value) noexcept : value(std::move(value)) { }

    bool operator==(const RealLiteral& rhs) const noexcept { return value == rhs.value; }
    bool operator!=(const RealLiteral& rhs) const noexcept { return !(*this == rhs); }
};

struct BoolLiteral {
    bool value;

    explicit BoolLiteral(bool value) noexcept : value(value) { }

    bool operator==(const BoolLiteral rhs) const noexcept { return value == rhs.value; }
    bool operator!=(const BoolLiteral rhs) const noexcept { return !(*this == rhs); }
};

struct StringLiteral {
    std::string value;

    explicit StringLiteral(std::string value) noexcept : value(std::move(value)) { }

    bool operator==(const StringLiteral& rhs) const noexcept { return value == rhs.value; }
    bool operator!=(const StringLiteral& rhs) const noexcept { return !(*this == rhs); }
};

struct NilLiteral {
    NilLiteral() noexcept = default;

    bool operator==(const NilLiteral) const noexcept { return true; }
    bool operator!=(const NilLiteral) const noexcept { return false; }
};

struct VoidLiteral {
    VoidLiteral() noexcept = default;

    bool operator==(const VoidLiteral) const noexcept { return true; }
    bool operator!=(const VoidLiteral) const noexcept { return false; }
};

using Literal = std::variant<VoidLiteral,
                             IntLiteral,
                             RealLiteral,
                             BoolLiteral,
                             StringLiteral,
                             NilLiteral>;



struct IdentifierName {
    std::string name;

    explicit IdentifierName(std::string name) noexcept : name(std::move(name)) { }

    bool operator==(const IdentifierName& rhs) const noexcept { return name == rhs.name; }
    bool operator!=(const IdentifierName& rhs) const noexcept { return !(*this == rhs); }
};

struct DiscardName {
    DiscardName() noexcept = default;

    bool operator==(const DiscardName) const noexcept { return true; }
    bool operator!=(const DiscardName) const noexcept { return false; }
};

struct ConstructorName {
    ConstructorName() noexcept = default;

    bool operator==(const ConstructorName) const noexcept { return true; }
    bool operator!=(const ConstructorName) const noexcept { return false; }
};

struct DestructorName {
    DestructorName() noexcept = default;

    bool operator==(const DestructorName) const noexcept { return true; }
    bool operator!=(const DestructorName) const noexcept { return false; }
};

struct OperatorName {
    enum class OpKind {
        Add, Sub, Mul, Div, Mod, BitNot, BitAnd, BitOr, BitXor, BitShl, BitShr, LogNot,
            LogAnd, LogOr, Eq, Ne, Lt, Le, Gt, Ge, Range, RangeExcl,
        Pos, Neg,
        Call, Subscript,
        AddAssign, SubAssign, MulAssign, DivAssign, ModAssign, BitAndAssign, BitOrAssign,
            BitXorAssign, BitShlAssign, BitShrAssign
    };

    OpKind op;

    explicit OperatorName(OpKind op) noexcept : op(op) { }

    bool operator==(const OperatorName rhs) const noexcept { return op == rhs.op; }
    bool operator!=(const OperatorName rhs) const noexcept { return !(*this == rhs); }
};

struct SelfName {
    SelfName() noexcept = default;

    bool operator==(const SelfName) const noexcept { return true; }
    bool operator!=(const SelfName) const noexcept { return false; }
};

using Name = std::variant<DiscardName,
                          IdentifierName,
                          ConstructorName,
                          DestructorName,
                          OperatorName,
                          SelfName>;



class NodeVisitor;

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
        : Node(start, end), kind(kind), isImmutable(isImmutable),
          optionality(optionality) { }

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

struct PathSeg final : Node {
    Name name;
    std::vector<Expr*> generics;

    PathSeg(Location start, Location end, Name name, std::vector<Expr*> generics) noexcept
        : Node(start, end), name(std::move(name)), generics(std::move(generics)) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct Path final : Node {
    std::vector<PathSeg*> segs;

    Path(Location start, Location end, std::vector<PathSeg*> segs) noexcept
        : Node(start, end), segs(std::move(segs)) { }

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
    std::vector<Node*> nodes;

    BlockExpr(Location start, Location end, std::vector<Node*> nodes) noexcept
        : Expr(start, end), nodes(std::move(nodes)) { }

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
    Name member;

    MemberAccessExpr(Location start, Location end, Expr* base, Name member) noexcept
        : Expr(start, end), base(base), member(member) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct CallArg final : Node {
    std::optional<Name> name;
    Expr* expr;

    CallArg(Location start, Location end, std::optional<Name> name, Expr* expr) noexcept
        : Node(start, end), name(std::move(name)), expr(expr) { }
    CallArg(Location start, Location end, Expr* expr) noexcept
        : CallArg(start, end, std::nullopt, expr) { }

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

struct LiteralExpr final : Expr {
    Literal literal;

    LiteralExpr(Location start, Location end, Literal literal) noexcept
            : Expr(start, end), literal(std::move(literal)) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct NameExpr final : Expr {
    Name name;

    NameExpr(Location start, Location end, Name name) noexcept
            : Expr(start, end), name(std::move(name)) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct GlobalAccessExpr final : Expr {
    Name name;

    GlobalAccessExpr(Location start, Location end, Name name) noexcept
        : Expr(start, end), name(std::move(name)) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct UpvalueExpr final : Expr {
    uint64_t level;
    Name name;

    UpvalueExpr(Location start, Location end, uint64_t level, Name name) noexcept
        : Expr(start, end), level(level), name(std::move(name)) { }

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

struct FnDefStmt final : Stmt {
    bool isImmutable;
    Name name;
    std::vector<Name> generics;
    std::vector<FnParam*> params;
    /* nullable */ FnCaptureClause* captureClause;
    std::vector<FnReturn*> returns;
    bool isThrowing;
    /* nullable */ Expr* throwExpr;
    Expr* body;

    FnDefStmt(Location start, Location end, bool isImmutable, Name name,
              std::vector<Name> generics,
              std::vector<FnParam*> params, FnCaptureClause* captureClause,
              std::vector<FnReturn*> returns, bool isThrowing, Expr* throwExpr,
              Expr* body) noexcept
        : Stmt(start, end), isImmutable(isImmutable), name(std::move(name)),
          generics(std::move(generics)), params(std::move(params)),
          captureClause(captureClause), returns(std::move(returns)),
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
    Name name;
    std::vector<Name> generics;
    std::vector<Expr*> bases;
    BlockExpr* body;

    TypeDefStmt(Location start, Location end,
                TypeKind kind, bool isImmutable, Name name,
                std::vector<Name> generics, std::vector<Expr*> bases, BlockExpr* body) noexcept
        : Stmt(start, end), kind(kind), isImmutable(isImmutable), name(std::move(name)),
          generics(std::move(generics)), bases(std::move(bases)), body(body) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct CaseDefStmt final : Stmt {
    Name name;
    /* nullable */ Expr* val;

    CaseDefStmt(Location start, Location end, Name name, Expr* val) noexcept
        : Stmt(start, end), name(std::move(name)), val(val) { }
    CaseDefStmt(Location start, Location end, Name name) noexcept
        : CaseDefStmt(start, end, std::move(name), nullptr) { }

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

struct IfStmt final : Stmt {
    Expr* condition;
    BlockExpr* thenBody;
    /* nullable */ BlockExpr* elseBody;

    IfStmt(Location start, Location end, Expr* condition, BlockExpr* thenBody, BlockExpr* elseBody) noexcept
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

struct DoWhileStmt final : Stmt {
    BlockExpr* body;
    Expr* condition;

    DoWhileStmt(Location start, Location end, BlockExpr* body, Expr* condition) noexcept
        : Stmt(start, end), body(body), condition(condition) { }

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
    ReturnStmt(Location start, Location end) noexcept
        : ReturnStmt(start, end, nullptr) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct ModuleStmt final : Stmt {
    Path* path;
    BlockExpr* body;

    ModuleStmt(Location start, Location end, Path* path, BlockExpr* body) noexcept
        : Stmt(start, end), path(path), body(body) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct ExportStmt final : Stmt {
    Stmt* stmt;

    ExportStmt(Location start, Location end, Stmt* stmt) noexcept
        : Stmt(start, end), stmt(stmt) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct ImportItem final : Node {
    Path* path;
    std::optional<Name> as;

    ImportItem(Location start, Location end, Path* path, std::optional<Name> as) noexcept
        : Node(start, end), path(path), as(std::move(as)) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct ImportStmt final : Stmt {
    Path* from;
    std::vector<ImportItem*> imports;

    ImportStmt(Location start, Location end, Path* from, std::vector<ImportItem*> imports) noexcept
        : Stmt(start, end), from(from), imports(std::move(imports)) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct ImportAllStmt final : Stmt {
    Path* from;

    ImportAllStmt(Location start, Location end, Path* from) noexcept
        : Stmt(start, end), from(from) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct UndefineStmt final : Stmt {
    Path* path;

    UndefineStmt(Location start, Location end, Path* path) noexcept
        : Stmt(start, end), path(path) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};



struct LiteralPattern final : Pattern {
    Literal literal;

    LiteralPattern(Location start, Location end, Literal literal) noexcept
            : Pattern(start, end), literal(std::move(literal)) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct BindingPattern final : Pattern {
    Name name;

    BindingPattern(Location start, Location end, Name name) noexcept
        : Pattern(start, end), name(std::move(name)) { }

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

struct RecordPatternField final : Node {
    std::optional<Name> label;
    Pattern* pattern;

    RecordPatternField(Location start, Location end, Name label, Pattern* pattern) noexcept
        : Node(start, end), label(std::move(label)), pattern(pattern) { }
    RecordPatternField(Location start, Location end, Pattern* pattern) noexcept
        : Node(start, end), label(std::nullopt), pattern(pattern) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct RecordPattern final : Pattern {
    std::vector<RecordPatternField*> fields;

    RecordPattern(Location start, Location end,
                  std::vector<RecordPatternField*> fields) noexcept
        : Pattern(start, end), fields(std::move(fields)) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

} // Spark::FrontEnd
