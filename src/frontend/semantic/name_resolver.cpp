#include "name_resolver.hpp"

#include <sstream>

namespace Spark::FrontEnd {

static Diagnostic redeclareError(Location start, Location end, const Symbol* symbol) {
    std::ostringstream msg;
    msg << "redeclaration of `" << symbol->name() << "`";

    std::ostringstream note;
    note << "previously declared as ";
    switch (symbol->kind) {
        case SymbolKind::Var:
            note << "variable";
            break;
        case SymbolKind::Func:
            note << "function";
            break;
        case SymbolKind::Type:
            note << "type";
            break;
        case SymbolKind::Module:
            note << "module";
            break;
    }
    note << " here";

    return Diagnostic::error(start, end, msg.str(), {
        Diagnostic::note(symbol->start(), symbol->end(), note.str())
    });
}



void PatternDeclarator::declare(Pattern* pattern,
                                SymbolTable& symbolTable,
                                Env& env,
                                SymbolKind kind,
                                bool isReassignable,
                                Diagnostics& diagnostics) {
    PatternDeclarator declarator{symbolTable, env, kind, isReassignable, diagnostics};
    pattern->accept(declarator);
}

void PatternDeclarator::visit(BindingPattern* pattern) {
    // Checks if the name already exists in the environment
    if (Symbol* symbol = _env.get(pattern->name->value.str()); symbol != nullptr) {
        diagnostic(redeclareError(pattern->name->start, pattern->name->end, symbol));
        return;
    }

    // Creates symbol and declares name
    _env.set(pattern->name->value.str(), _symbolTable.make(Symbol{
        .kind = _kind,
        .isReassignable = _isReassignable,
        .node = pattern->name
    }));
}

void PatternDeclarator::visit(TuplePattern* pattern) {
    for (Pattern* p : pattern->patterns) {
        p->accept(*this);
    }
}

void PatternDeclarator::visit(CollectionPattern* pattern) {
    for (Pattern* p : pattern->prefix) {
        p->accept(*this);
    }
    for (Pattern* p : pattern->suffix) {
        p->accept(*this);
    }
}

void PatternDeclarator::visit(RecordPattern* pattern) {
    for (RecordPatternField* field : pattern->fields) {
        field->pattern->accept(*this);
    }
}



void Declarator::declare(Node* node, SymbolTable& symbolTable, Env& env, Diagnostics& diagnostics) {
    if (node == nullptr) {
        return;
    }

    Declarator declarator{symbolTable, env, diagnostics};
    node->accept(declarator);
}

void Declarator::visit(MatchCase* c) {
    PatternDeclarator::declare(c->pattern, _symbolTable, _env, SymbolKind::Var, false, _diagnostics);
}

void Declarator::visit(VarDefStmt* vardef) {
    PatternDeclarator::declare(vardef->pattern, _symbolTable, _env, SymbolKind::Var,
        vardef->mod->kind == VarModifier::VarKind::Let, _diagnostics);
}

void Declarator::visit(FnDefStmt* fndef) {
    // Checks if the name already exists in the environment
    if (Symbol* symbol = _env.get(fndef->name->value.str()); symbol != nullptr) {
        diagnostic(redeclareError(fndef->name->start, fndef->name->end, symbol));
        return;
    }

    // Creates symbol and declares name
    _env.set(fndef->name->value.str(), _symbolTable.make(Symbol{
        .kind = SymbolKind::Func,
        .isReassignable = false,
        .node = fndef->name
    }));
}

void Declarator::visit(TypeDefStmt* tdef) {
    // Checks if the name already exists in the environment
    if (Symbol* symbol = _env.get(tdef->name->value.str()); symbol != nullptr) {
        diagnostic(redeclareError(tdef->name->start, tdef->name->end, symbol));
        return;
    }

    // Creates symbol and declares name
    _env.set(tdef->name->value.str(), _symbolTable.make(Symbol{
        .kind = SymbolKind::Type,
        .isReassignable = false,
        .node = tdef->name
    }));
}

void Declarator::visit(ModuleStmt* mdef) {
    Name* name = mdef->path->segs[0]->name;
    Symbol* symbol = _env.get(name->value.str());
    if (symbol != nullptr) {
        // Checks if the name already exists in the environment with a different declaration kind
        if (symbol->kind != SymbolKind::Module) {
            diagnostic(redeclareError(name->start, name->end, symbol));
            return;
        }
    } else {
        // Create symbol
        symbol = _symbolTable.make(Symbol{
            .kind = SymbolKind::Module,
            .isReassignable = false,
            .node = name
        });
    }

    // Declares name with the symbol
    _env.set(name->value.str(), symbol);

    // TODO: Creates (or retrieve) module environment(s) and link to the symbol
}



void GlobalNameResolver::resolve(const AST& ast, SymbolTable& symbolTable, Env& globalEnv, Diagnostics& diagnostics) {
    if (ast.root == nullptr) {
        return;
    }

    GlobalNameResolver resolver{symbolTable, globalEnv, diagnostics};
    if (auto* block = ast.root->as<BlockExpr>()) {
        for (Node* node : block->nodes) {
            node->accept(resolver);
        }
    } else {
        ast.root->accept(resolver);
    }
}

void GlobalNameResolver::visit(ExportStmt* e) {
    Declarator::declare(e->stmt, _symbolTable, _globalEnv, _diagnostics);
}



void NameResolver::resolve(const AST& ast, const Env& globalEnv, Diagnostics& diagnostics) {
    if (ast.root == nullptr) {
        return;
    }

    NameResolver resolver{globalEnv, diagnostics};
    ast.root->accept(resolver);
}

void NameResolver::visit(MemberAccessExpr* expr) {
    // Resolves base expression
    expr->base->accept(*this);


}

} // Spark::FrontEnd
