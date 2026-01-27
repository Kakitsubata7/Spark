#pragma once

#include <utility>
#include <vector>

#include "frontend/ast.hpp"
#include "symbol_table.hpp"
#include "utils/error.hpp"

namespace Spark::FrontEnd {

class NameResolver {
private:
    class Visitor : public NodeVisitor {
    public:
        
    };

public:
    static std::pair<SymbolTable, std::vector<Error>> resolve(const AST& ast);
};

} // Spark::FrontEnd
