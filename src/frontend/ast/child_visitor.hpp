#pragma once

#include <vector>

#include "node_visitor.hpp"

namespace Spark::FrontEnd {

class ChildVisitor : public NodeVisitor {
public:
    std::vector<Node*>& out;

    explicit ChildVisitor(std::vector<Node*>& out) noexcept : out(out) { }

    void visit(FnParam& node) override;
    void visit(FnCapture& node) override;
    void visit(FnCaptureClause& node) override;
    void visit(FnReturn& node) override;

    void visit(PathSeg& node) override;
    void visit(Path& node) override;

    void visit(LambdaExpr& node) override;
    void visit(IfThenExpr& node) override;
    void visit(TryElseExpr& node) override;
    void visit(MatchCase& node) override;
    void visit(MatchExpr& node) override;
    void visit(CatchClause& node) override;
    void visit(TryCatchExpr& node) override;
    void visit(ThrowExpr& node) override;
    void visit(BlockExpr& node) override;
    void visit(IsExpr& node) override;
    void visit(AsExpr& node) override;
    void visit(BinaryExpr& node) override;
    void visit(PrefixExpr& node) override;
    void visit(PostfixExpr& node) override;
    void visit(MemberAccessExpr& node) override;
    void visit(CallArg& node) override;
    void visit(CallExpr& node) override;
    void visit(SubscriptExpr& node) override;
    void visit(NameExpr& node) override;
    void visit(GlobalAccessExpr& node) override;
    void visit(UpvalueExpr& node) override;
    void visit(TupleExpr& node) override;
    void visit(CollectionExpr& node) override;
    void visit(TypeofExpr& node) override;

    void visit(VarDefStmt& node) override;
    void visit(FnDefStmt& node) override;
    void visit(TypeDefStmt& node) override;
    void visit(CaseDefStmt& node) override;
    void visit(AssignStmt& node) override;
    void visit(IfStmt& node) override;
    void visit(WhileStmt& node) override;
    void visit(DoWhileStmt& node) override;
    void visit(ForStmt& node) override;
    void visit(ReturnStmt& node) override;
    void visit(ModuleStmt& node) override;
    void visit(ExportStmt& node) override;
    void visit(ImportItem& node) override;
    void visit(ImportStmt& node) override;
    void visit(ImportAllStmt& node) override;
    void visit(UndefineStmt& node) override;

    void visit(BindingPattern& node) override;
    void visit(TuplePattern& node) override;
    void visit(CollectionPattern& node) override;
    void visit(RecordPatternField& node) override;
    void visit(RecordPattern& node) override;

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
