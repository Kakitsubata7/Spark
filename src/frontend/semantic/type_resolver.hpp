#pragma once

#include "semantic_type.hpp"
#include "symbol.hpp"
#include "frontend/ast/node_visitor.hpp"
#include "utils/diagnostic.hpp"

namespace Spark::FrontEnd {

class TypeResolver : public NodeVisitor {
private:
    NodeSymbolMap& _nodeSymbolMap;

    SemanticTypeTable& _typeTable;
    NodeTypeMap& _nodeTypeMap;

    Diagnostics& _diagnostics;

public:
    TypeResolver(NodeSymbolMap& nodeSymbolMap,
                 SemanticTypeTable& typeTable,
                 NodeTypeMap& nodeTypeMap,
                 Diagnostics& diagnostics) noexcept :
        _nodeSymbolMap(nodeSymbolMap), _typeTable(typeTable), _nodeTypeMap(nodeTypeMap), _diagnostics(diagnostics) { }

    void visit(LiteralExpr* expr) override;

    void visit(VarDefStmt* vardef) override;
    void visit(FnDefStmt* fndef) override;
    void visit(TypeDefStmt* tdef) override;
};

} // Spark::FrontEnd
