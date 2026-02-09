#pragma once

#include "env.hpp"
#include "frontend/ast.hpp"
#include "symbol.hpp"
#include "utils/diagnostic.hpp"

namespace Spark::FrontEnd {

struct GlobalResolveResult {
    Env globalEnv;
    Diagnostics diagnostics;
};

class GlobalResolver {
public:
    static GlobalResolveResult resolve(const AST& ast, SymbolTable& symTable, NodeSymbolMap& nodeSymMap);
};

} // Spark::FrontEnd
