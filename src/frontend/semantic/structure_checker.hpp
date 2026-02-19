#pragma once

#include "frontend/ast.hpp"
#include "utils/diagnostic.hpp"

namespace Spark::FrontEnd {

class StructureChecker : public NodeVisitor {
private:
    struct Context {
        bool inLoop;
        bool inFunc;
        bool inType;
        bool inModule;
    };

    Context _ctx{};

    Diagnostics& _diagnostics;

    explicit StructureChecker(Diagnostics& diagnostics) noexcept : _diagnostics(diagnostics) { }

public:
    static Diagnostics check(const AST& ast);

    void visit(Expr* expr) override;
    void visit(Stmt* stmt) override;
    void visit(BlockExpr* block) override;
    void visit(VarDefStmt* vardef) override;

    void visit(WhileStmt* w) override;
    void visit(ForStmt* f) override;
    void visit(FnDefStmt* fndef) override;
    void visit(LambdaExpr* lambda) override;
    void visit(TypeDefStmt* tdef) override;
    void visit(ModuleStmt* moddef) override;

    void visit(BreakStmt* b) override;
    void visit(ContinueStmt* c) override;
    void visit(ReturnStmt* r) override;
    void visit(ExportStmt* e) override;
    void visit(UndefineStmt* undef) override;

private:
    Context replaceCtx(Context ctx) {
        Context old = _ctx;
        _ctx = ctx;
        return old;
    }

    [[nodiscard]]
    constexpr bool inLoop() const noexcept { return _ctx.inLoop; }

    [[nodiscard]]
    constexpr bool inFunc() const noexcept { return _ctx.inFunc; }

    [[nodiscard]]
    constexpr bool inType() const noexcept { return _ctx.inType; }

    [[nodiscard]]
    constexpr bool inModule() const noexcept { return _ctx.inModule; }

    void visitLoop(BlockExpr* body);
    void visitFn(Expr* body);

    void error(Location start, Location end, std::string message, std::vector<Diagnostic> subs = {}) {
        _diagnostics.add(Diagnostic::error(start, end, std::move(message), std::move(subs)));
    }
};

} // Spark::FrontEnd
