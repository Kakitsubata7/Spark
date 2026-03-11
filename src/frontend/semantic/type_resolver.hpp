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
                 Diagnostics& diagnostics) noexcept :
        _nodeSymbolMap(nodeSymbolMap), _typeTable(typeTable), _diagnostics(diagnostics) { }
};

} // Spark::FrontEnd
