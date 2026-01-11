#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "node_visitor.hpp"
#include "utils/bigint.hpp"
#include "utils/bigreal.hpp"
#include "utils/location.hpp"

namespace Spark::FrontEnd {

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

struct Assign final : Node {
    enum class OpKind {
        Assign,
        AddAssign, SubAssign, MulAssign, DivAssign, ModAssign,
        BitAndAssign, BitOrAssign, BitXorAssign, BitShlAssign, BitShrAssign,
        CoalesceAssign
    };

    Node* lhs;
    Node* rhs;
    OpKind op;

    Assign(Location start, Location end, Node* lhs, Node* rhs, OpKind op) noexcept
        : Node(start, end), lhs(lhs), rhs(rhs), op(op) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct TypeDef final : Node {
    enum class TypeKind {
        Struct, Class, Enum, EnumStruct, EnumClass, Trait, Alias, Extension
    };

    TypeKind kind;
    bool isImmutable;
    Node* decl;
    std::vector<Node*> bases;
    Node* body;

    TypeDef(Location start, Location end,
            TypeKind kind, bool isImmutable, Node* decl, std::vector<Node*> bases, Node* body) noexcept
        : Node(start, end), kind(kind), isImmutable(isImmutable), decl(decl), bases(std::move(bases)), body(body) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct IfElse final : Node {
    Node* condition;
    Node* thenBody;
    /* nullable */ Node* elseBody;

    IfElse(Location start, Location end, Node* condition, Node* thenBody, Node* elseBody) noexcept
        : Node(start, end), condition(condition), thenBody(thenBody), elseBody(elseBody) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct While final : Node {
    Node* condition;
    Node* body;

    While(Location start, Location end, Node* condition, Node* body) noexcept
        : Node(start, end), condition(condition), body(body) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct For final : Node {
    Node* iterator;
    Node* range;
    Node* body;

    For(Location start, Location end, Node* iterator, Node* range, Node* body) noexcept
        : Node(start, end), iterator(iterator), range(range), body(body) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct Break final : Node {
    Break(Location start, Location end) noexcept : Node(start, end) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct Continue final : Node {
    Continue(Location start, Location end) noexcept : Node(start, end) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct Return final : Node {
    /* nullable */ Node* expr;

    Return(Location start, Location end, Node* expr) noexcept
        : Node(start, end), expr(expr) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct Module final : Node {
    std::vector<Node*> names;
    Node* body;

    Module(Location start, Location end, std::vector<Node*> names, Node* body) noexcept
        : Node(start, end), names(std::move(names)), body(body) { }

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

struct IfThen final : Node {
    Node* condition;
    Node* thenExpr;
    Node* elseExpr;

    IfThen(Location start, Location end, Node* condition, Node* thenExpr, Node* elseExpr) noexcept
        : Node(start, end), condition(condition), thenExpr(thenExpr), elseExpr(elseExpr) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct TryElse final : Node {
    Node* tryExpr;
    Node* elseExpr;

    TryElse(Location start, Location end, Node* tryExpr, Node* elseExpr) noexcept
        : Node(start, end), tryExpr(tryExpr), elseExpr(elseExpr) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct Match final : Node {
    struct Case {
        /* nullable */ Node* pattern;
        /* nullable */ Node* guard;
        Node* body;

        Case() noexcept = default;
        Case(Node* pattern, Node* guard, Node* body) noexcept : pattern(pattern), guard(guard), body(body) { }
    };

    /* nullable */ Node* binding;
    Node* scrutinee;
    std::vector<Case> cases;

    Match(Location start, Location end, Node* binding, Node* scrutinee, std::vector<Case> cases) noexcept
        : Node(start, end), binding(binding), scrutinee(scrutinee), cases(std::move(cases)) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct TryCatch final : Node {
    struct Catch {
        /* nullable */ Node* pattern;
        /* nullable */ Node* guard;
        Node* body;

        Catch() noexcept = default;
        Catch(Node* pattern, Node* guard, Node* body) noexcept : pattern(pattern), guard(guard), body(body) { }
    };

    Node* expr;
    std::vector<Catch> catches;

    TryCatch(Location start, Location end, Node* expr, std::vector<Catch> catches) noexcept
        : Node(start, end), expr(expr), catches(std::move(catches)) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct Throw final : Node {
    Node* expr;

    Throw(Location start, Location end, Node* expr) noexcept
        : Node(start, end), expr(expr) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct Block final : Node {
    std::vector<Node*> elements;

    Block(Location start, Location end, std::vector<Node*> elements) noexcept
        : Node(start, end), elements(std::move(elements)) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct BinaryExpr final : Node {
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
    Node* lhs;
    Node* rhs;

    BinaryExpr(Location start, Location end, OpKind op, Node* lhs, Node* rhs) noexcept
        : Node(start, end), op(op), lhs(lhs), rhs(rhs) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct PrefixExpr final : Node {
    enum class OpKind {
        Pos, Neg, BitNot, LogNot, Immut
    };

    OpKind op;
    Node* expr;

    PrefixExpr(Location start, Location end, OpKind op, Node* expr) noexcept
        : Node(start, end), op(op), expr(expr) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct PostfixExpr final : Node {
    enum class OpKind {
        Optional, NonNull, ForceUnwrap
    };

    OpKind op;
    Node* expr;

    PostfixExpr(Location start, Location end, OpKind op, Node* expr) noexcept
        : Node(start, end), op(op), expr(expr) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct MemberAccess final : Node {
    Node* member;

    MemberAccess(Location start, Location end, Node* member) noexcept
        : Node(start, end), member(member) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct Subscript final : Node {
    Node* callee;
    std::vector<Node*> indices;

    Subscript(Location start, Location end, Node* callee, std::vector<Node*> indices) noexcept
        : Node(start, end), callee(callee), indices(std::move(indices)) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct IntLiteral final : Node {
    BigInt value;

    IntLiteral(Location start, Location end, BigInt value) noexcept
        : Node(start, end), value(std::move(value)) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct RealLiteral final : Node {
    BigReal value;

    RealLiteral(Location start, Location end, BigReal value) noexcept
        : Node(start, end), value(std::move(value)) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct BoolLiteral final : Node {
    bool value;

    BoolLiteral(Location start, Location end, bool value) noexcept
        : Node(start, end), value(value) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct StringLiteral final : Node {
    std::string value;

    StringLiteral(Location start, Location end, std::string value) noexcept
        : Node(start, end), value(std::move(value)) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct NilLiteral final : Node {
    NilLiteral(Location start, Location end) noexcept : Node(start, end) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct VoidLiteral final : Node {
    VoidLiteral(Location start, Location end) noexcept : Node(start, end) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct Identifier final : Node {
    std::string name;

    Identifier(Location start, Location end, std::string name) noexcept
        : Node(start, end), name(std::move(name)) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct Wildcard final : Node {
    Wildcard(Location start, Location end) noexcept : Node(start, end) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct Upvalue final : Node {
    uint64_t level;
    Node* var;

    Upvalue(Location start, Location end, uint64_t level, Node* var) noexcept
        : Node(start, end), level(level), var(var) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct Global final : Node {
    Global(Location start, Location end) noexcept : Node(start, end) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct Self final : Node {
    Self(Location start, Location end) noexcept : Node(start, end) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct OverloadableOp final : Node {
    enum class OpKind {
        Add, Sub, Mul, Div, Mod, BitNot, BitAnd, BitOr, BitXor, BitShl, BitShr, LogNot, LogAnd, LogOr,
        Eq, Ne, Lt, Le, Gt, Ge, Range, RangeExcl, Call, Subscript,
        AddAssign, SubAssign, MulAssign, DivAssign, ModAssign, BitAndAssign, BitOrAssign, BitXorAssign, BitShlAssign,
            BitShrAssign
    };

    OpKind op;

    OverloadableOp(Location start, Location end, OpKind op) noexcept
        : Node(start, end), op(op) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

struct BindingModifier final : Node {
    enum class BindingKind {
        Let, Const, Ref, Cref
    };

    enum class Optionality {
        None, Optional, OptionalNonNull
    };

    BindingKind kind;
    bool isImmutable;
    Optionality opt;

    BindingModifier(Location start, Location end, BindingKind kind, bool isImmutable, Optionality opt) noexcept
        : Node(start, end), kind(kind), isImmutable(isImmutable), opt(opt) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }

protected:
    [[nodiscard]]
    bool equalsImpl(const Node& rhs) const noexcept override;
};

} // Spark::FrontEnd
