#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "node_visitor.hpp"
#include "utils/bigint.hpp"
#include "utils/bigreal.hpp"
#include "utils/location.hpp"

namespace Spark::FrontEnd {

enum class NodeKind : uint32_t {
    Other   = 0,
    Expr    = 1 << 0,
    Stmt    = 1 << 1,
    Pattern = 1 << 2
};

inline NodeKind operator|(NodeKind lhs, NodeKind rhs) {
    return static_cast<NodeKind>(
        static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs)
    );
}

inline NodeKind operator&(NodeKind lhs, NodeKind rhs) {
    return static_cast<NodeKind>(
        static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs)
    );
}

inline NodeKind& operator|=(NodeKind& lhs, NodeKind rhs) {
    lhs = lhs | rhs;
    return lhs;
}

enum class NodeType {
    None, Terms,
    Operation,
    Identifier, UpvalueIdentifier, Discard,
    IntLiteral, RealLiteral, BoolLiteral, StringLiteral, Nil, Undefined,
    VarDef,
    FnKeyword, ConstructorKeyword, DestructorKeyword, OperatorKeyword,
    TypeDef, TypeDefKeyword,
    Block,
    IfKeyword, ThenKeyword, ElseKeyword,
    MatchKeyword, CaseKeyword,
    While, DoWhile, For, InKeyword, Break, Continue,
    Return,
    Is, Typeof,
    Throw, TryKeyword, Catch,
    Module, Export, From, Import,
    Annotation,
    Semicolon,
};

class NodeVisitor;

struct Node {
    Location start;
    Location end;

    Node(Location start, Location end) noexcept
        : start(start), end(end) { }

    virtual ~Node() = default;

    [[nodiscard]]
    virtual NodeKind kind() const = 0;

    [[nodiscard]]
    virtual NodeType type() const = 0;

    virtual void accept(NodeVisitor& v) = 0;
};

struct Terms final : Node {
    std::vector<Node*> nodes;

    Terms(Location start, Location end, std::vector<Node*> nodes = {}) noexcept
        : Node(start, end), nodes(std::move(nodes)) { }

    [[nodiscard]]
    NodeKind kind() const override { return NodeKind::Other; }

    [[nodiscard]]
    NodeType type() const override { return NodeType::Terms; }

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

enum class OperationKind {
    Add, Sub, Mul, Div, Mod, Tide, And, VBar, Caret, Bang, LogAnd, LogOr, Eq, Ne, Lt, Shl, Gt, Shr, Le, Ge, StrictEq, StrictNe, Range, RangeExcl, Question, Coalesce, Pipe,
    Is,
    Assign, AddAssign, SubAssign, MulAssign, DivAssign, ModAssign, BitAndAssign, BitOrAssign, BitXorAssign, CoalesceAssign,
    Dot, Colon
};

struct Operation final : Node {
    OperationKind op;

    Operation(Location start, Location end, OperationKind op) noexcept
        : Node(start, end), op(op) { }

    [[nodiscard]]
    NodeKind kind() const override { return NodeKind::Other; }

    [[nodiscard]]
    NodeType type() const override { return NodeType::Operation; }

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct Identifier final : Node {
    std::string lexeme;

    Identifier(Location start, Location end, std::string lexeme) noexcept
        : Node(start, end), lexeme(std::move(lexeme)) { }

    [[nodiscard]]
    NodeKind kind() const override { return NodeKind::Expr; }

    [[nodiscard]]
    NodeType type() const override { return NodeType::Identifier; }

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct UpvalueIdentifier final : Node {
    std::string lexeme;

    UpvalueIdentifier(Location start, Location end, std::string lexeme) noexcept
        : Node(start, end), lexeme(std::move(lexeme)) { }

    [[nodiscard]]
    NodeKind kind() const override { return NodeKind::Expr; }

    [[nodiscard]]
    NodeType type() const override { return NodeType::UpvalueIdentifier; }

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct Discard final : Node {
    Discard(Location start, Location end) noexcept : Node(start, end) { }

    [[nodiscard]]
    NodeKind kind() const override { return NodeKind::Expr | NodeKind::Pattern; }

    [[nodiscard]]
    NodeType type() const override { return NodeType::Discard; }

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct IntLiteral final : Node {
    BigInt value;

    IntLiteral(Location start, Location end, BigInt value) noexcept
        : Node(start, end), value(std::move(value)) { }

    [[nodiscard]]
    NodeKind kind() const override { return NodeKind::Expr | NodeKind::Pattern; }

    [[nodiscard]]
    NodeType type() const override { return NodeType::IntLiteral; }

    void accept(NodeVisitor& v) override {
        v.visit(*this);
    }
};

struct RealLiteral final : Node {
    BigReal value;

    RealLiteral(Location start, Location end, BigReal value) noexcept
        : Node(start, end), value(std::move(value)) { }

    [[nodiscard]]
    NodeKind kind() const override { return NodeKind::Expr | NodeKind::Pattern; }

    [[nodiscard]]
    NodeType type() const override { return NodeType::RealLiteral; }

    void accept(NodeVisitor& v) override {
        v.visit(*this);
    }
};

struct BoolLiteral final : Node {
    bool value;

    BoolLiteral(Location start, Location end, bool value) noexcept
        : Node(start, end), value(value) { }

    [[nodiscard]]
    NodeKind kind() const override { return NodeKind::Expr | NodeKind::Pattern; }

    [[nodiscard]]
    NodeType type() const override { return NodeType::BoolLiteral; }

    void accept(NodeVisitor& v) override {
        v.visit(*this);
    }
};

struct StringLiteral final : Node {
    std::string value;

    StringLiteral(Location start, Location end, std::string value) noexcept
        : Node(start, end), value(std::move(value)) { }

    [[nodiscard]]
    NodeKind kind() const override { return NodeKind::Expr | NodeKind::Pattern; }

    [[nodiscard]]
    NodeType type() const override { return NodeType::StringLiteral; }

    void accept(NodeVisitor& v) override {
        v.visit(*this);
    }
};

struct Nil final : Node {
    Nil(Location start, Location end) noexcept
        : Node(start, end) { }

    [[nodiscard]]
    NodeKind kind() const override { return NodeKind::Expr | NodeKind::Pattern; }

    [[nodiscard]]
    NodeType type() const override { return NodeType::Nil; }

    void accept(NodeVisitor& v) override {
        v.visit(*this);
    }
};

struct Undefined final : Node {
    Undefined(Location start, Location end) noexcept
        : Node(start, end) { }

    [[nodiscard]]
    NodeKind kind() const override { return NodeKind::Expr; }

    [[nodiscard]]
    NodeType type() const override { return NodeType::Undefined; }

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

enum class VarDefKind {
    Let, Const, Ref, Cref
};

struct VarDef final : Node {
    VarDefKind varKind;
    bool isImmutable;
    bool isOptional;
    Node* pattern;
    Node* expr;

    VarDef(Location start, Location end,
           VarDefKind varKind, bool isImmutable, bool isOptional,
           Node* pattern, Node* expr) noexcept
        : Node(start, end),
          varKind(varKind), isImmutable(isImmutable), isOptional(isOptional),
          pattern(pattern), expr(expr) { }

    [[nodiscard]]
    NodeKind kind() const override { return NodeKind::Stmt; }

    [[nodiscard]]
    NodeType type() const override { return NodeType::VarDef; }

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

enum class TypeDefKind {
    Struct, Class, Enum, EnumClass, EnumStruct, Trait, Alias, Extension
};

struct TypeDef final : Node {
    TypeDefKind typeKind;
    Block* block;

    TypeDef(Location start, Location end, TypeDefKind typeKind, Block* block) noexcept
        : Node(start, end), typeKind(typeKind), block(block) { }

    [[nodiscard]]
    NodeKind kind() const override { return NodeKind::Stmt; }

    [[nodiscard]]
    NodeType type() const override { return NodeType::TypeDef; }

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct TypeDefKeyword final : Node {
    TypeDefKind typeKind;

    TypeDefKeyword(Location start, Location end, TypeDefKind typeKind) noexcept
            : Node(start, end), typeKind(typeKind) { }

    [[nodiscard]]
    NodeKind kind() const override { return NodeKind::Other; }

    [[nodiscard]]
    NodeType type() const override { return NodeType::TypeDefKeyword; }

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct Block final : Node {
    Terms* terms;

    Block(Location start, Location end, Terms* terms) noexcept
        : Node(start, end), terms(std::move(terms)) { }

    [[nodiscard]]
    NodeKind kind() const override { return NodeKind::Stmt; }

    [[nodiscard]]
    NodeType type() const override { return NodeType::Block; }

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct IfKeyword final : Node {
    IfKeyword(Location start, Location end) noexcept : Node(start, end) { }

    [[nodiscard]]
    NodeKind kind() const override { return NodeKind::Other; }

    [[nodiscard]]
    NodeType type() const override { return NodeType::IfKeyword; }

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct ThenKeyword final : Node {
    ThenKeyword(Location start, Location end) noexcept : Node(start, end) { }

    [[nodiscard]]
    NodeKind kind() const override { return NodeKind::Other; }

    [[nodiscard]]
    NodeType type() const override { return NodeType::ThenKeyword; }

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct ElseKeyword final : Node {
    ElseKeyword(Location start, Location end) noexcept : Node(start, end) { }

    [[nodiscard]]
    NodeKind kind() const override { return NodeKind::Other; }

    [[nodiscard]]
    NodeType type() const override { return NodeType::ElseKeyword; }

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct MatchKeyword final : Node {
    MatchKeyword(Location start, Location end) noexcept : Node(start, end) { }

    [[nodiscard]]
    NodeKind kind() const override { return NodeKind::Other; }

    [[nodiscard]]
    NodeType type() const override { return NodeType::MatchKeyword; }

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct CaseKeyword final : Node {
    CaseKeyword(Location start, Location end) noexcept : Node(start, end) { }

    [[nodiscard]]
    NodeKind kind() const override { return NodeKind::Other; }

    [[nodiscard]]
    NodeType type() const override { return NodeType::CaseKeyword; }

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct While final : Node {
    Node* condition;
    Block* block;

    While(Location start, Location end, Node* condition, Block* block) noexcept
        : Node(start, end), condition(condition), block(block) { }

    [[nodiscard]]
    NodeKind kind() const override { return NodeKind::Stmt; }

    [[nodiscard]]
    NodeType type() const override { return NodeType::While; }

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct DoWhile final : Node {
    Block* block;
    Node* condition;

    DoWhile(Location start, Location end, Block* block, Node* condition) noexcept
        : Node(start, end), block(block), condition(condition) { }

    [[nodiscard]]
    NodeKind kind() const override { return NodeKind::Other; }

    [[nodiscard]]
    NodeType type() const override { return NodeType::DoWhile; }

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct For final : Node {
    Node* iterator;
    Node* range;
    Block* block;

    For(Location start, Location end, Node* iterator, Node* range, Block* block) noexcept
        : Node(start, end), iterator(iterator), range(range), block(block) { }

    [[nodiscard]]
    NodeKind kind() const override { return NodeKind::Stmt; }

    [[nodiscard]]
    NodeType type() const override { return NodeType::For; }

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct InKeyword final : Node {
    InKeyword(Location start, Location end) noexcept : Node(start, end) { }

    [[nodiscard]]
    NodeKind kind() const override { return NodeKind::Other; }

    [[nodiscard]]
    NodeType type() const override { return NodeType::InKeyword; }

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct Break final : Node {
    Break(Location start, Location end) noexcept
         : Node(start, end) { }

    [[nodiscard]]
    NodeKind kind() const override { return NodeKind::Stmt; }

    [[nodiscard]]
    NodeType type() const override { return NodeType::Break; }

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct Continue final : Node {
    Continue(Location start, Location end) noexcept
         : Node(start, end) { }

    [[nodiscard]]
    NodeKind kind() const override { return NodeKind::Stmt; }

    [[nodiscard]]
    NodeType type() const override { return NodeType::Continue; }

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct Return final : Node {
    Node* expr;

    Return(Location start, Location end, Node* expr = nullptr) noexcept
        : Node(start, end), expr(expr) { }

    [[nodiscard]]
    NodeKind kind() const override { return NodeKind::Expr; }

    [[nodiscard]]
    NodeType type() const override { return NodeType::Return; }

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct Is final : Node {
    Node* lhs;
    Node* rhs;

    Is(Location start, Location end, Node* lhs, Node* rhs) noexcept
        : Node(start, end), lhs(lhs), rhs(rhs) { }

    [[nodiscard]]
    NodeKind kind() const override { return NodeKind::Expr; }

    [[nodiscard]]
    NodeType type() const override { return NodeType::Is; }

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct Typeof final : Node {
    Node* rand;

    Typeof(Location start, Location end, Node* rand) noexcept
        : Node(start, end), rand(rand) { }

    [[nodiscard]]
    NodeKind kind() const override { return NodeKind::Expr; }

    [[nodiscard]]
    NodeType type() const override { return NodeType::Typeof; }

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct Throw final : Node {
    Node* expr;

    Throw(Location start, Location end, Node* expr = nullptr) noexcept
        : Node(start, end), expr(expr) { }

    [[nodiscard]]
    NodeKind kind() const override { return NodeKind::Expr; }

    [[nodiscard]]
    NodeType type() const override { return NodeType::Throw; }

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct TryKeyword final : Node {
    TryKeyword(Location start, Location end) noexcept : Node(start, end) { }

    [[nodiscard]]
    NodeKind kind() const override { return NodeKind::Other; }

    [[nodiscard]]
    NodeType type() const override { return NodeType::TryKeyword; }

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct Catch final : Node {
    Node* pattern;
    Block* block;

    Catch(Location start, Location end, Block* block, Node* pattern = nullptr) noexcept
        : Node(start, end), pattern(pattern), block(block) { }

    [[nodiscard]]
    NodeKind kind() const override { return NodeKind::Stmt; }

    [[nodiscard]]
    NodeType type() const override { return NodeType::Catch; }

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct Module final : Node {
    Block* block;

    Module(Location start, Location end, Block* block) noexcept
        : Node(start, end), block(block) { }

    [[nodiscard]]
    NodeKind kind() const override { return NodeKind::Stmt; }

    [[nodiscard]]
    NodeType type() const override { return NodeType::Module; }

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct Export final : Node {
    Node* stmt;

    Export(Location start, Location end, Node* stmt) noexcept
        : Node(start, end), stmt(stmt) { }

    [[nodiscard]]
    NodeKind kind() const override { return NodeKind::Stmt; }

    [[nodiscard]]
    NodeType type() const override { return NodeType::Export; }

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct From final : Node {
    Node* term;

    From(Location start, Location end, Node* module) noexcept
        : Node(start, end), term(module) { }

    [[nodiscard]]
    NodeKind kind() const override { return NodeKind::Stmt; }

    [[nodiscard]]
    NodeType type() const override { return NodeType::From; }

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct Import final : Node {
    Node* term;

    Import(Location start, Location end, Node* term) noexcept
        : Node(start, end), term(term) { }

    [[nodiscard]]
    NodeKind kind() const override { return NodeKind::Stmt; }

    [[nodiscard]]
    NodeType type() const override { return NodeType::Import; }

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct Annotation final : Node {
    Node* node;

    Annotation(Location start, Location end, Node* node) noexcept
        : Node(start, end), node(node) { }

    [[nodiscard]]
    NodeKind kind() const override { return NodeKind::Other; }

    [[nodiscard]]
    NodeType type() const override { return NodeType::Annotation; }

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct Semicolon final : Node {
    Semicolon(Location start, Location end) noexcept : Node(start, end) { }

    [[nodiscard]]
    NodeKind kind() const override { return NodeKind::Other; }

    [[nodiscard]]
    NodeType type() const override { return NodeType::Semicolon; }

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

} // Spark::FrontEnd
