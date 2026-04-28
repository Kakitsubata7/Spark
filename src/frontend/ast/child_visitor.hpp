#pragma once

#include <vector>

#include "node_visitor.hpp"

namespace Spark::FrontEnd {

class ChildVisitor : public NodeVisitor {
public:
    std::vector<Node*>& out;

    explicit ChildVisitor(std::vector<Node*>& out) noexcept : out(out) { }

    void visit(FnParam* param) override;
    void visit(FnCapture* cap) override;
    void visit(FnCaptureClause* clause) override;
    void visit(FnReturn* ret) override;

    void visit(PathSeg* seg) override;
    void visit(Path* path) override;

    void visit(LambdaExpr* expr) override;
    void visit(IfThenExpr* expr) override;
    void visit(TryElseExpr* expr) override;
    void visit(MatchCase* c) override;
    void visit(MatchExpr* expr) override;
    void visit(CatchClause* c) override;
    void visit(TryCatchExpr* expr) override;
    void visit(ThrowExpr* expr) override;
    void visit(BlockExpr* expr) override;
    void visit(IsExpr* expr) override;
    void visit(AsExpr* expr) override;
    void visit(BinaryExpr* expr) override;
    void visit(PrefixExpr* expr) override;
    void visit(PostfixExpr* expr) override;
    void visit(MemberAccessExpr* expr) override;
    void visit(CallArg* arg) override;
    void visit(CallExpr* expr) override;
    void visit(SubscriptExpr* expr) override;
    void visit(NameExpr* expr) override;
    void visit(GlobalAccessExpr* expr) override;
    void visit(UpvalueExpr* expr) override;
    void visit(TupleExpr* expr) override;
    void visit(CollectionExpr* expr) override;
    void visit(TypeofExpr* expr) override;

    void visit(VarDefStmt* stmt) override;
    void visit(FnDefStmt* stmt) override;
    void visit(TypeDefStmt* stmt) override;
    void visit(CaseDefStmt* stmt) override;
    void visit(AssignStmt* stmt) override;
    void visit(IfStmt* stmt) override;
    void visit(WhileStmt* stmt) override;
    void visit(DoWhileStmt* stmt) override;
    void visit(ForStmt* stmt) override;
    void visit(ReturnStmt* stmt) override;
    void visit(ModuleStmt* stmt) override;
    void visit(ExportStmt* stmt) override;
    void visit(ImportItem* stmt) override;
    void visit(ImportStmt* stmt) override;
    void visit(ImportAllStmt* stmt) override;
    void visit(UndefineStmt* stmt) override;

    void visit(BindingPattern* pattern) override;
    void visit(TuplePattern* pattern) override;
    void visit(CollectionPattern* pattern) override;
    void visit(RecordPatternField* pattern) override;
    void visit(RecordPattern* pattern) override;

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
