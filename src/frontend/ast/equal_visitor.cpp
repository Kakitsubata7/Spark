#include "equal_visitor.hpp"

#include <typeinfo>

#include "node.hpp"
#include "expr.hpp"
#include "stmt.hpp"

namespace Spark::FrontEnd {

static bool typeidEquals(const Node& lhs, const Node& rhs) {
    return typeid(lhs) == typeid(rhs);
}

void EqualVisitor::visit(Node& lhs) {
    if ((_result = typeidEquals(lhs, _rhs))) {
        _result = lhs.start == _rhs.start && lhs.end == _rhs.end;
    }
}

void EqualVisitor::visit(Expr& lhs) {
    visit(static_cast<Node&>(lhs));
    if (_result) {
        const auto& rhs = static_cast<const Expr&>(_rhs);
        _result = lhs.type == rhs.type;
    }
}

void EqualVisitor::visit(IntLiteralExpr& lhs) {
    visit(static_cast<Expr&>(lhs));
    if (_result) {
        const auto& rhs = static_cast<const IntLiteralExpr&>(_rhs);
        _result = lhs.value == rhs.value;
    }
}

void EqualVisitor::visit(RealLiteralExpr& lhs) {
    visit(static_cast<Expr&>(lhs));
    if (_result) {
        const auto& rhs = static_cast<const RealLiteralExpr&>(_rhs);
        _result = lhs.value == rhs.value;
    }
}

void EqualVisitor::visit(BoolLiteralExpr& lhs) {
    visit(static_cast<Expr&>(lhs));
    if (_result) {
        const auto& rhs = static_cast<const BoolLiteralExpr&>(_rhs);
        _result = lhs.value == rhs.value;
    }
}

void EqualVisitor::visit(StringLiteralExpr& lhs) {
    visit(static_cast<Expr&>(lhs));
    if (_result) {
        const auto& rhs = static_cast<const StringLiteralExpr&>(_rhs);
        _result = lhs.value == rhs.value;
    }
}

void EqualVisitor::visit(NilLiteralExpr& lhs) {
    visit(static_cast<Expr&>(lhs));
}

void EqualVisitor::visit(VarExpr& lhs) {
    visit(static_cast<Expr&>(lhs));
    if (_result) {
        const auto& rhs = static_cast<const VarExpr&>(_rhs);
        _result = lhs.identifier == rhs.identifier;
    }
}

void EqualVisitor::visit(UpvalueVarExpr& lhs) {
    visit(static_cast<Expr&>(lhs));
    if (_result) {
        const auto& rhs = static_cast<const UpvalueVarExpr&>(_rhs);
        _result = lhs.identifier == rhs.identifier;
    }
}

void EqualVisitor::visit(Stmt& lhs) {
    visit(static_cast<Node&>(lhs));
}

} // Spark::FrontEnd
