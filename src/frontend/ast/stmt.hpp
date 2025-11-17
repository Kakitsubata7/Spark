#pragma once

#include <optional>
#include <vector>

#include "expr.hpp"
#include "node.hpp"
#include "node_visitor.hpp"
#include "var.hpp"

namespace Spark::FrontEnd::AST {

struct Stmt : Node { };

struct BlockStmt final : Stmt {
    std::vector<Stmt> stmts;

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct VarDeclStmt final : Stmt {
    VarDecl var;
    Expr* expr = nullptr;

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct AssignStmt final : Stmt {
    Expr* target = nullptr;
    Expr* expr = nullptr;

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
        Pattern* pattern;
        Expr* guard;
        BlockStmt* body;
    };

    Expr* scrutinee = nullptr;
    std::optional<VarDecl> asVar = std::nullopt;
    std::vector<CaseClause> cases;

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct WhileStmt final : Stmt {
    Expr* condition = nullptr;
    BlockStmt* body = nullptr;

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

struct TryCatchStmt final : Stmt {
    struct CatchClause {
        Pattern* pattern = nullptr;
        std::optional<VarDecl> asVar = std::nullopt;
        BlockStmt* body = nullptr;
    };

    BlockStmt* tryBody = nullptr;
    std::vector<CatchClause> catchClauses;

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

} // Spark::FrontEnd::AST
