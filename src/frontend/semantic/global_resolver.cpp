#include "global_resolver.hpp"

#include "binder.hpp"

namespace Spark::FrontEnd {

GlobalResolveResult GlobalResolver::resolve(const AST& ast, SymbolTable& symTable, NodeSymbolMap& nodeSymMap) {
    if (ast.root == nullptr) {
        return {};
    }

    // Appends nodes to process
    std::vector<Node*> nodes;
    if (ast.root->is<BlockExpr>()) {
        ast.root->getChildren(nodes);
    } else {
        nodes.push_back(ast.root);
    }

    // Declare exported symbols in the top-level
    Env globalEnv;
    Diagnostics diagnostics;
    for (const Node* node : nodes) {
        if (const ExportStmt* ex = node->as<ExportStmt>(); ex != nullptr) {
            Stmt* stmt = ex->stmt;
            Binder::bind(stmt, globalEnv, symTable, nodeSymMap, true, diagnostics);
        }
    }

    return GlobalResolveResult{.globalEnv = std::move(globalEnv), .diagnostics = std::move(diagnostics)};
}

} // Spark::FrontEnd
