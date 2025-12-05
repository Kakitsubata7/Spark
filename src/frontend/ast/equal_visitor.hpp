#pragma once

#include <typeinfo>

#include "node.hpp"
#include "node_visitor.hpp"

namespace Spark::FrontEnd {

class EqualVisitor final : public NodeVisitor {
private:
    bool _result;
    const Node& _rhs;

public:
    [[nodiscard]]
    constexpr bool result() const noexcept { return _result; }

    explicit EqualVisitor(const Node& rhs) noexcept : _result(false), _rhs(rhs) { }

    void visit(Node& lhs) override;

    void visit(Expr& lhs) override;
    void visit(IntLiteralExpr& lhs) override;
    void visit(RealLiteralExpr& lhs) override;
    void visit(BoolLiteralExpr& lhs) override;
    void visit(StringLiteralExpr& lhs) override;
    void visit(NilLiteralExpr& lhs) override;
    void visit(VarExpr& lhs) override;
    void visit(UpvalueVarExpr& lhs) override;

    void visit(Stmt& lhs) override;
};

} // Spark::FrontEnd
