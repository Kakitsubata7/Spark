#include "name_resolver.hpp"

#include <sstream>

namespace Spark::FrontEnd {

void PatternDeclarator::visit(Name* node) {
    assert(node != nullptr);

    // Checks if the name is already declared locally in the environment
    std::string_view name = node->value.str();
    Symbol* symbol = _env.get(name);
    if (symbol == nullptr) {
        // Create symbol
        symbol = _symbolTable.make(Symbol{
            .kind = _kind,
            .isReassignable = _isReassignable,
            .node = node
        });

        // Bind symbol to environment
        _env.set(name, symbol);
    } else {
        // Redeclaration error, reuse the previously declared symbol
        redeclareError(node, symbol->node);
    }

    // Bind symbol to the `Name` node
    _nodeSymbolMap.set(node, symbol);
}

void PatternDeclarator::visit(BindingPattern* pattern) {
    assert(pattern != nullptr);
    pattern->name->accept(*this);
}

void PatternDeclarator::visit(TuplePattern* pattern) {
    assert(pattern != nullptr);
    for (Pattern* p : pattern->patterns) {
        p->accept(*this);
    }
}

void PatternDeclarator::visit(CollectionPattern* pattern) {
    assert(pattern != nullptr);
    for (Pattern* p : pattern->prefix) {
        p->accept(*this);
    }
    for (Pattern* p : pattern->suffix) {
        p->accept(*this);
    }
}

void PatternDeclarator::visit(RecordPattern* pattern) {
    assert(pattern != nullptr);
    for (RecordPatternField* field : pattern->fields) {
        field->pattern->accept(*this);
    }
}

void PatternDeclarator::redeclareError(const Name* node, const Name* prevDeclareNode) noexcept {
    assert(node != nullptr);
    assert(prevDeclareNode != nullptr);

    std::ostringstream msg;
    std::string_view name = node->value.str();
    msg << "redeclaration of name: `" << name << "`";
    _diagnostics.add(Diagnostic::error(node->start, node->end, msg.str(), {
        Diagnostic::note(prevDeclareNode->start, node->end, "previously declared here")
    }));
}



void NameDeclarator::visit(VarDefStmt* vardef) {
    assert(vardef != nullptr);
    bool isReassignable = vardef->mod->kind == VarModifier::VarKind::Let;
    PatternDeclarator declarator{_symbolTable, _nodeSymbolMap, _env, SymbolKind::Var, isReassignable, _diagnostics};
    vardef->pattern->accept(declarator);
}

void NameDeclarator::visit(FnDefStmt* fndef) {
    assert(fndef != nullptr);
    PatternDeclarator declarator{_symbolTable, _nodeSymbolMap, _env, SymbolKind::Func, false, _diagnostics};
    fndef->name->accept(declarator);
}



void NameResolver::visit(NameExpr* ident) {
    std::string_view name = ident->name->value.str();
    if (Symbol* symbol = _env.find(name); symbol != nullptr) {
        _nodeSymbolMap.set(ident, symbol);
        _nodeSymbolMap.set(ident->name, symbol);
        return;
    }
    cannotFindError(ident->start, ident->end, name);
}

void NameResolver::cannotFindError(Location start, Location end, std::string_view name) {
    std::ostringstream msg;
    msg << "cannot find name: `" << name << "`";
    _diagnostics.add(Diagnostic::error(start, end, msg.str()));
}



void SemanticResolver::visit(BlockExpr* block) {
    pushEnv();

    // Create name declarator
    NameDeclarator nameDeclarator{};

    // Declare hoisted names
    for (Node* node : block->nodes) {
        if (isHoistedDeclarative(node)) {
            node->accept(nameDeclarator);
        }
    }

    // Resolves names
    NameResolver nameResolver{};

    popEnv();
}

void SemanticResolver::resolveTopLevel(Node* node) {
    assert(node != nullptr);

    std::vector<Node*> nodes;
    if (auto* block = node->as<BlockExpr>()) {
        nodes = block->nodes;
    } else {
        nodes.push_back(node);
    }
}

bool SemanticResolver::isHoistedDeclarative(const Node* node) noexcept {
    assert(node != nullptr);

    if (node->is<FnDefStmt>()) {
        return true;
    }
    return false;
}
} // Spark::FrontEnd
