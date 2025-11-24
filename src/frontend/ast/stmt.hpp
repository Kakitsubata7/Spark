#pragma once

#include <optional>
#include <vector>

#include "expr.hpp"
#include "node.hpp"
#include "node_visitor.hpp"
#include "utils.hpp"

namespace Spark::FrontEnd {

struct Stmt : Node {
    Stmt(Location start, Location end) : Node(start, end) { }
};

struct BlockStmt final : Stmt {
    std::vector<Stmt*> stmts;

    BlockStmt(Location start, Location end, std::vector<Stmt*> stmts = {}) noexcept
        : Stmt(start, end), stmts(std::move(stmts)) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct VarDeclStmt final : Stmt {
    VarDecl var;
    Expr* expr = nullptr;

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct FnDeclStmt final : Stmt {
    Name name;
    std::vector<ArgDecl> args;
    std::optional<Path> returnTypePath = std::nullopt;
    BlockStmt* body = nullptr;

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct LambdaDeclStmt final : Stmt {
    Name name;
    std::vector<ArgDecl> args;
    std::optional<Path> returnTypePath = std::nullopt;
    std::vector<Node*> body;

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct AssignStmt final : Stmt {
    AssignType rator = AssignType::None;
    Expr* target = nullptr;
    Expr* expr = nullptr;

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct CallStmt final : Stmt {
    Expr* func = nullptr;
    std::vector<Expr*> args;

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct SubscriptStmt final : Stmt {
    Expr* expr = nullptr;
    Expr* index = nullptr;

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct IfStmt final : Stmt {
    struct IfClause {
        Expr* condition;
        BlockStmt* body;
    };

    std::vector<IfClause> ifClauses;
    BlockStmt* elseBody;

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct MatchStmt final : Stmt {
    struct CaseClause {
        Pattern* pattern = nullptr;
        Expr* guard = nullptr;
        BlockStmt* body = nullptr;
    };

    Expr* scrutinee = nullptr;
    std::optional<VarDecl> asVar = std::nullopt;
    std::vector<CaseClause> cases;

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct WhileStmt final : Stmt {
    Expr* condition;
    BlockStmt* body;

    WhileStmt(Location start, Location end, Expr* condition, BlockStmt* body) noexcept
        : Stmt(start, end), condition(condition), body(body) { }

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct ForStmt final : Stmt {
    VarDecl loopVar;
    Expr* iterator = nullptr;
    BlockStmt* body = nullptr;

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct BreakStmt final : Stmt {
    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct ContinueStmt final : Stmt {
    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct ThrowStmt final : Stmt {
    Expr* thrown = nullptr;

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct CatchClause {
    Pattern* pattern = nullptr;
    std::optional<VarDecl> asVar = std::nullopt;
    BlockStmt* body = nullptr;
};

struct TryStmt final : Stmt {
    VarDecl var;
    Expr* expr = nullptr;
    BlockStmt* body = nullptr;

    std::vector<CatchClause> catchClauses;

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct TryCatchStmt final : Stmt {
    BlockStmt* tryBody = nullptr;
    std::vector<CatchClause> catchClauses;

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

} // Spark::FrontEnd
