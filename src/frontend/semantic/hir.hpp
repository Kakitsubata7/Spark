#pragma once

#include "utils/bigint.hpp"
#include "utils/bigreal.hpp"

namespace Spark::FrontEnd {

struct HIRNode {
    virtual ~HIRNode() = default;
};

struct HIRExpr : HIRNode {

};

struct HIRBlock : HIRExpr {
    std::vector<HIRNode*> nodes;

    explicit HIRBlock(std::vector<HIRNode*> nodes) : nodes(std::move(nodes)) { }
};

struct HIRCall : HIRExpr {

};

struct HIRFieldAccess : HIRExpr {

};

struct HIRIntLiteral : HIRExpr {
    BigInt value;

    explicit HIRIntLiteral(BigInt value) : value(std::move(value)) { }
};

struct HIRRealLiteral : HIRExpr {
    BigReal value;

    explicit HIRRealLiteral(BigReal value) : value(std::move(value)) { }
};

struct HIRStringLiteral : HIRExpr {
    std::string value;

    explicit HIRStringLiteral(std::string value) : value(std::move(value)) { }
};

struct HIRBoolLiteral : HIRExpr {
    bool value;

    explicit HIRBoolLiteral(bool value) : value(value) { }
};

struct HIRNilLiteral : HIRExpr { };

struct HIRVoidLiteral : HIRExpr { };

struct HIRAssign : HIRNode {
    HIRExpr* expr;

    explicit HIRAssign(HIRExpr* expr) : expr(expr) { }
};

class HIR {
private:

};

} // Spark::FrontEnd
