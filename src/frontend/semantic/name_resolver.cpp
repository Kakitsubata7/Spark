#include "name_resolver.hpp"

#include "binder.hpp"

namespace Spark::FrontEnd {

NameResolveResult NameResolver::resolve(const AST& ast,
                                        SymbolTable& symTable,
                                        NodeSymbolMap& nodeSymMap,
                                        const Env& globalEnv) {
    if (ast.root == nullptr) {
        return {};
    }

    Diagnostics diagnostics;
    NameResolveVisitor v(symTable, nodeSymMap, diagnostics);
    if (BlockExpr* block = ast.root->as<BlockExpr>()) {
        v.resolveBlock(block, globalEnv);
    } else {
        v.pushEnv(globalEnv);
        ast.root->accept(v);
        v.popEnv();
    }
    return NameResolveResult{.diagnostics = std::move(diagnostics)};
}



void NameResolveVisitor::visit(Node* node) {
    for (Node* child : node->getChildren()) {
        child->accept(*this);
    }
}

void NameResolveVisitor::visit(Name* name) {
    auto [s, isVisible] = lookup(name->value);
    if (s == nullptr) {
        report(Diagnostic::cannotFindError(name->start, name->end, name->value.str()));
        return;
    }
    if (!isVisible) {
        report(Diagnostic::useBeforeDeclError(name->start, name->end, s));
        return;
    }
    _nodeSymMap.set(name, s);
}

void NameResolveVisitor::visit(LambdaExpr* lambda) {

}

void NameResolveVisitor::visit(VarDefStmt* vardef) {
    // Mark declared names as visible
    for (Name* name : PatternBindingCollector::collect(vardef->pattern)) {
        currentEnv().setVisible(name->value, true);
    }

    // Resolve type
    if (vardef->type != nullptr) {
        vardef->type->accept(*this);
    }

    // Resolve rhs
    if (vardef->rhs != nullptr) {
        vardef->rhs->accept(*this);
    }
}

void NameResolveVisitor::visit(FnDefStmt* fndef) {

}

void NameResolveVisitor::visit(TypeDefStmt* tdef) {

}

void NameResolveVisitor::visit(ModuleStmt* moddef) {

}

void NameResolveVisitor::visit(BlockExpr* block) {
    resolveBlock(block, Env{});
}

void NameResolveVisitor::resolveBlock(BlockExpr* block, Env env) {
    pushEnv(std::move(env));

    // Declare names, marking non-hoisted names as invisible
    for (Node* node : block->nodes) {
        if (!_nodeSymMap.hasSymbol(node)) {
            if (node->is<VarDefStmt>() || node->is<FnDefStmt>() || node->is<TypeDefStmt>() || node->is<ModuleStmt>()) {
                DeclBinder::bind(node, currentEnv(), _symTable, _nodeSymMap, !node->is<VarDefStmt>(),
                    _diagnostics);
            }
        }
    }

    // Resolution
    for (Node* node : block->nodes) {
        node->accept(*this);
    }

    popEnv();
}

std::pair<const Symbol*, bool> NameResolveVisitor::lookup(InternedNameValue name) const noexcept {
    for (auto it = _envStack.rbegin(); it != _envStack.rend(); ++it) {
        const Env& env = *it;
        if (const Symbol* s = env.find(name); s != nullptr) {
            return std::make_pair(s, env.isVisible(name));
        }
    }
    return std::make_pair(nullptr, false);
}

} // Spark::FrontEnd
