#pragma once

#include <vector>

#include "node.hpp"
#include "node_visitor.hpp"

namespace Spark::FrontEnd {

class ChildVisitor : public NodeVisitor {
public:
    std::vector<Node*>& out;

    explicit ChildVisitor(std::vector<Node*>& out) noexcept : out(out) { }

    void visit(FnParam& node) override {
        add(node.mod);
        add(node.pattern);
        add(node.type);
        add(node.def);
    }

    void visit(FnCapture& node) override {
        add(node.mod);
        add(node.pattern);
    }

    void visit(FnCaptureClause& node) override {
        addVec(node.captures);
        add(node.restMod);
    }

    void visit(FnReturn& node) override {
        add(node.type);
    }

    void visit(PathSeg& node) override {
        addVec(node.generics);
    }

    void visit(Path& node) override {
        addVec(node.segs);
    }

    void visit(LambdaExpr& node) override {
        addVec(node.params);
        add(node.captureClause);
        addVec(node.returns);
        add(node.throwExpr);
        add(node.body);
    }

    void visit(IfThenExpr& node) override {
        add(node.condition);
        add(node.thenExpr);
        add(node.elseExpr);
    }

    void visit(TryElseExpr& node) override {
        add(node.tryExpr);
        add(node.elseExpr);
    }

    void visit(MatchCase& node) override {
        add(node.pattern);
        add(node.guard);
        add(node.body);
    }

    void visit(MatchExpr& node) override {
        add(node.scrutinee);
        addVec(node.cases);
    }

    void visit(CatchClause& node) override {
        add(node.pattern);
        add(node.guard);
        add(node.body);
    }

    void visit(TryCatchExpr& node) override {
        add(node.expr);
        addVec(node.catches);
    }

    void visit(ThrowExpr& node) override {
        add(node.expr);
    }

    void visit(BlockExpr& node) override {
        addVec(node.nodes);
    }

    void visit(IsExpr& node) override {
        add(node.expr);
        add(node.type);
    }

    void visit(AsExpr& node) override {
        add(node.expr);
        add(node.pattern);
        add(node.type);
    }

    void visit(BinaryExpr& node) override {
        add(node.lhs);
        add(node.rhs);
    }

    void visit(PrefixExpr& node) override {
        add(node.expr);
    }

    void visit(PostfixExpr& node) override {
        add(node.expr);
    }

    void visit(MemberAccessExpr& node) override {
        add(node.base);
    }

    void visit(CallArg& node) override {
        add(node.expr);
    }

    void visit(CallExpr& node) override {
        add(node.callee);
        addVec(node.args);
    }

    void visit(SubscriptExpr& node) override {
        add(node.base);
        addVec(node.indices);
    }

    void visit(TupleExpr& node) override {
        addVec(node.exprs);
    }

    void visit(CollectionExpr& node) override {
        addVec(node.exprs);
    }

    void visit(TypeofExpr& node) override {
        add(node.expr);
    }

    void visit(VarDefStmt& node) override {
        add(node.mod);
        add(node.pattern);
        add(node.type);
        add(node.rhs);
    }

    void visit(FnDefStmt& node) override {
        addVec(node.params);
        add(node.captureClause);
        addVec(node.returns);
        add(node.throwExpr);
        add(node.body);
    }

    void visit(TypeDefStmt& node) override {
        addVec(node.bases);
        add(node.body);
    }

    void visit(CaseDefStmt& node) override {
        add(node.val);
    }

    void visit(AssignStmt& node) override {
        add(node.lhs);
        add(node.rhs);
    }

    void visit(IfStmt& node) override {
        add(node.condition);
        add(node.thenBody);
        add(node.elseBody);
    }

    void visit(WhileStmt& node) override {
        add(node.condition);
        add(node.body);
    }

    void visit(DoWhileStmt& node) override {
        add(node.body);
        add(node.condition);
    }

    void visit(ForStmt& node) override {
        add(node.iterator);
        add(node.range);
        add(node.body);
    }

    void visit(ReturnStmt& node) override {
        add(node.expr);
    }

    void visit(ModuleStmt& node) override {
        add(node.path);
        add(node.body);
    }

    void visit(ExportStmt& node) override {
        add(node.stmt);
    }

    void visit(ImportItem& node) override {
        add(node.path);
    }

    void visit(ImportStmt& node) override {
        add(node.from);
        addVec(node.imports);
    }

    void visit(ImportAllStmt& node) override {
        add(node.from);
    }

    void visit(UndefineStmt& node) override {
        add(node.path);
    }

    void visit(TuplePattern& node) override {
        addVec(node.patterns);
    }

    void visit(CollectionPattern& node) override {
        addVec(node.prefix);
        addVec(node.suffix);
    }

    void visit(RecordPatternField& node) override {
        add(node.pattern);
    }

    void visit(RecordPattern& node) override {
        addVec(node.fields);
    }

private:
    void add(Node* node) const noexcept {
        if (node != nullptr) {
            out.push_back(node);
        }
    }

    template <typename T, typename = std::enable_if_t<std::is_base_of_v<Node, T>>>
    void addVec(const std::vector<T*>& nodes) const noexcept {
        for (T* node : nodes) {
            add(node);
        }
    }
};

} // Spark::FrontEnd
