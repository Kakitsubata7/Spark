#pragma once

#include "utils/bigint.hpp"
#include "utils/bigreal.hpp"

namespace Spark::FrontEnd {

struct HIRNode {
    virtual ~HIRNode() = default;
};

struct HIRExpr : HIRNode {

};

struct HIRBlock final : HIRExpr {
    std::vector<HIRNode*> nodes;

    explicit HIRBlock(std::vector<HIRNode*> nodes) : nodes(std::move(nodes)) { }
};

struct HIRCall final : HIRExpr {
    HIRExpr* callee;
    std::vector<HIRExpr*> args;

    HIRCall(HIRExpr* callee, std::vector<HIRExpr*> args) : callee(callee), args(std::move(args)) { }
};

struct HIRFieldAccess final : HIRExpr {

};

struct HIRIntLiteral final : HIRExpr {
    BigInt value;

    explicit HIRIntLiteral(BigInt value) : value(std::move(value)) { }
};

struct HIRRealLiteral final : HIRExpr {
    BigReal value;

    explicit HIRRealLiteral(BigReal value) : value(std::move(value)) { }
};

struct HIRStringLiteral final : HIRExpr {
    std::string value;

    explicit HIRStringLiteral(std::string value) : value(std::move(value)) { }
};

struct HIRBoolLiteral final : HIRExpr {
    bool value;

    explicit HIRBoolLiteral(bool value) : value(value) { }
};

struct HIRNilLiteral final : HIRExpr { };

struct HIRVoidLiteral final : HIRExpr { };

struct HIRAssign final : HIRNode {
    HIRExpr* expr;

    explicit HIRAssign(HIRExpr* expr) : expr(expr) { }
};

struct HIRIf final : HIRExpr {
    HIRExpr* condition;
    HIRExpr* then;
    HIRExpr* els;

    HIRIf(HIRExpr* cond, HIRExpr* then, HIRExpr* els) noexcept
        : condition(cond), then(then), els(els) { }
};

class HIR {
private:

};

} // Spark::FrontEnd
