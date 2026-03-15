#include "type_resolver.hpp"

namespace Spark::FrontEnd {

void TypeResolver::visit(LiteralExpr* expr) {
    assert(expr != nullptr);

    const Literal& literal = expr->literal;
    if (std::holds_alternative<VoidLiteral>(literal)) {

    } else if (std::holds_alternative<IntLiteral>(literal)) {

    } else if (std::holds_alternative<RealLiteral>(literal)) {

    } else if (std::holds_alternative<BoolLiteral>(literal)) {

    } else if (std::holds_alternative<StringLiteral>(literal)) {

    } else if (std::holds_alternative<NilLiteral>(literal)) {

    }
}

void TypeResolver::visit(VarDefStmt* vardef) {
    assert(vardef != nullptr);

    // _nodeTypeMap.set(vardef, void);
}

void TypeResolver::visit(FnDefStmt* fndef) {
    assert(fndef != nullptr);

    // _nodeTypeMap.set(fndef, void);
}

void TypeResolver::visit(TypeDefStmt* tdef) {
    assert(tdef != nullptr);

    // _nodeTypeMap.set(tdef, void);
}

} // Spark::FrontEnd
