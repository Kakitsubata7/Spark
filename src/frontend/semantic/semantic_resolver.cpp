#include "semantic_resolver.hpp"

#include <sstream>
#include <variant>

namespace Spark::FrontEnd {

SemanticResolver::SemanticResolver(Diagnostics& diagnostics) : _diagnostics(diagnostics) {
}

void SemanticResolver::visit(FnParam* param) {
    assert(param != nullptr);

    // Declares the parameter name
    declare(param->pattern, currentEnv(), SymbolKind::Var, isReassignable(param->mod));

    // Resolves type and default value (if present)
    if (param->type != nullptr) {
        param->type->accept(*this);
    }
    if (param->def) {
        param->def->accept(*this);
    }
}

void SemanticResolver::visit(IfThenExpr* ifthen) {
    assert(ifthen != nullptr);

    // Resolves condition
    SemanticType* condType = resolve(ifthen->condition);

    // Type checks condition
    if (condType->is(boolType())) {

    } else if (auto* ctor = boolType()->getImplicitConstructor(condType); ctor != nullptr) {

    } else {
        // TODO: Type mismatch error
    }

    // Resolves branches
    SemanticType* thenType = resolve(ifthen->thenExpr);
    SemanticType* elseType = resolve(ifthen->elseExpr);

    //
    _currentType = unionType(thenType, elseType);
}

void SemanticResolver::visit(BlockExpr* block) {
    assert(block != nullptr);

    pushEnv();

    SemanticType* type = voidType();

    // Declare hoisted names
    for (auto it = block->nodes.begin(); it != block->nodes.end(); ++it) {
        Node* node = *it;
        if (isHoistedDeclarative(node)) {
            SemanticType* t = resolve(node);
            if (std::next(it) == block->nodes.end()) {
                type = t;
            }
        }
    }

    // Resolve the rest
    for (auto it = block->nodes.begin(); it != block->nodes.end(); ++it) {
        Node* node = *it;
        if (!isHoistedDeclarative(node)) {
            SemanticType* t = resolve(node);
            if (std::next(it) == block->nodes.end()) {
                type = t;
            }
        }
    }

    // Sets the type of the block
    _currentType = type;

    popEnv();
}

void SemanticResolver::visit(BinaryExpr* binary) {
    assert(binary != nullptr);

    SemanticType* lhsType = resolve(binary->lhs);
    SemanticType* rhsType = resolve(binary->rhs);

    switch (binary->op) {
        case BinaryExpr::OpKind::Add:
            break;

        case BinaryExpr::OpKind::Sub:
            break;

        case BinaryExpr::OpKind::Mul:
            break;

        case BinaryExpr::OpKind::Div:
            break;

        case BinaryExpr::OpKind::Mod:
            break;

        case BinaryExpr::OpKind::Lt:
            break;

        case BinaryExpr::OpKind::Le:
            break;

        case BinaryExpr::OpKind::Gt:
            break;

        case BinaryExpr::OpKind::Ge:
            break;

        case BinaryExpr::OpKind::Eq:
            break;

        case BinaryExpr::OpKind::Ne:
            break;

        case BinaryExpr::OpKind::StrictEq: {
            if (lhsType != rhsType) {
                // TODO: Type mismatch error
            }
            break;
        }

        case BinaryExpr::OpKind::StrictNe: {
            if (lhsType != rhsType) {
                // TODO: Type mismatch error
            }
            break;
        }

        case BinaryExpr::OpKind::BitAnd:
            break;

        case BinaryExpr::OpKind::BitOr:
            break;

        case BinaryExpr::OpKind::BitXor:
            break;

        case BinaryExpr::OpKind::BitShl:
            break;

        case BinaryExpr::OpKind::BitShr:
            break;

        case BinaryExpr::OpKind::LogAnd:
            break;

        case BinaryExpr::OpKind::LogOr:
            break;

        case BinaryExpr::OpKind::Coalesce:
            break;

        case BinaryExpr::OpKind::Range:
            break;

        case BinaryExpr::OpKind::RangeExcl:
            break;

        case BinaryExpr::OpKind::FuncType:
            break;

        case BinaryExpr::OpKind::Pipe:
            break;

        default:
            assert(false && "invalid `BinaryExpr::OpKind` value");
            break;
    }
}

void SemanticResolver::visit(PrefixExpr* prefix) {
    assert(prefix != nullptr);

    // Resolves expr
    SemanticType* type = resolve(prefix->expr);

    switch (prefix->op) {
        case PrefixExpr::OpKind::Pos:
            break;

        case PrefixExpr::OpKind::Neg:
            break;

        case PrefixExpr::OpKind::BitNot:
            break;

        case PrefixExpr::OpKind::LogNot:
            break;

        case PrefixExpr::OpKind::Immut:
            break;
    }
}

void SemanticResolver::visit(PostfixExpr* postfix) {
    assert(postfix != nullptr);

    // Resolves expr
    SemanticType* type = resolve(postfix->expr);

    switch (postfix->op) {
        case PostfixExpr::OpKind::Optional:
            break;

        case PostfixExpr::OpKind::NonNull:
            break;

        case PostfixExpr::OpKind::ForceUnwrap:
            break;
    }
}

void SemanticResolver::visit(LiteralExpr* literal) {
    assert(literal != nullptr);

    std::visit([&](auto&& value) {
         using T = std::decay_t<decltype(value)>;

         if constexpr (std::is_same_v<T, VoidLiteral>) {
             // Resolves type
             _currentType = voidType();
         } else if constexpr (std::is_same_v<T, IntLiteral>) {
             // Resolves type
            _currentType = intType();
         } else if constexpr (std::is_same_v<T, RealLiteral>) {
             // Resolves type
            _currentType = realType();
         } else if constexpr (std::is_same_v<T, BoolLiteral>) {
             // Resolves type
            _currentType = boolType();
         } else if constexpr (std::is_same_v<T, StringLiteral>) {
             // Resolves type
            _currentType = stringType();
         } else if constexpr (std::is_same_v<T, NilLiteral>) {
             // Resolves type
            _currentType = nilType();
         }
     }, literal->literal);
}

void SemanticResolver::visit(NameExpr* ident) {
    assert(ident != nullptr);

    // Gets the symbol of the name
    std::string_view name = ident->name->value.str();

    // Resolves name and type
    if (Symbol* symbol = lookup(name); symbol != nullptr) {
        _currentType = symbol->type;
    } else {
        _currentType = unknownType();
        cannotFindError(ident->start, ident->end, name);
    }
}

void SemanticResolver::visit(VarDefStmt* vardef) {
    assert(vardef != nullptr);

    // Declares name into the current environment
    declare(vardef->pattern, currentEnv(), SymbolKind::Var, isReassignable(vardef->mod));

    // Resolves type
    _currentType = voidType();
}

void SemanticResolver::visit(FnDefStmt* fndef) {
    assert(fndef != nullptr);

    // Declares function name
    declare(fndef->name, currentEnv(), SymbolKind::Func, false);

    // Function body
    pushEnv();

    // Declares parameters
    for (FnParam* param : fndef->params) {
        param->accept(*this);
    }

    // Resolves body
    fndef->body->accept(*this);

    popEnv();
}

void SemanticResolver::visit(TypeDefStmt* tdef) {
    assert(tdef != nullptr);

    // Declares type name
    declare(tdef->name, currentEnv(), SymbolKind::Type, false);

    // Resolves base type list
    for (Expr* base : tdef->bases) {
        base->accept(*this);
    }

    // Resolves type body
    tdef->body->accept(*this);
}

void SemanticResolver::visit(IfStmt* ifstmt) {
    assert(ifstmt != nullptr);
    ifstmt->condition->accept(*this);
    ifstmt->thenBody->accept(*this);
    if (ifstmt->elseBody != nullptr) {
        ifstmt->elseBody->accept(*this);
    }
}

void SemanticResolver::visit(WhileStmt* w) {
    assert(w != nullptr);
    w->condition->accept(*this);
    w->body->accept(*this);
}

void SemanticResolver::visit(DoWhileStmt* dowhile) {
    assert(dowhile != nullptr);
    dowhile->body->accept(*this);
    dowhile->condition->accept(*this);
}

void SemanticResolver::visit(ReturnStmt* ret) {
    assert(ret != nullptr);
    if (ret->expr != nullptr) {
        ret->expr->accept(*this);
    }
}

void SemanticResolver::resolve(LiteralExpr* literal, std::string& src) {
    assert(literal != nullptr);

    std::visit([&](auto&& value) {
        using T = std::decay_t<decltype(value)>;

        if constexpr (std::is_same_v<T, VoidLiteral>) {
            // Resolves type
            resolveType(literal, getGlobalDeclaredType("Void"));

            // Set C source
            src = _emitter.voidLiteral();
        } else if constexpr (std::is_same_v<T, IntLiteral>) {
            // Resolves type
            resolveType(literal, getGlobalDeclaredType("Int"));

            // Set C source
            src = _emitter.intLiteral(value.value);
        } else if constexpr (std::is_same_v<T, RealLiteral>) {
            // Resolves type
            resolveType(literal, getGlobalDeclaredType("Real"));

            // Set C source
            src = _emitter.realLiteral(value.value);
        } else if constexpr (std::is_same_v<T, BoolLiteral>) {
            // Resolves type
            resolveType(literal, getGlobalDeclaredType("Bool"));

            // Set C source
            src = _emitter.boolLiteral(value.value);
        } else if constexpr (std::is_same_v<T, StringLiteral>) {
            // Resolves type
            resolveType(literal, getGlobalDeclaredType("String"));

            // Set C source
            src = _emitter.stringLiteral(value.value);
        } else if constexpr (std::is_same_v<T, NilLiteral>) {
            // Resolves type
            resolveType(literal, getGlobalDeclaredType("Nil"));

            // Set C source
            src = _emitter.nilLiteral();
        }
    }, literal->literal);
}

Env& SemanticResolver::currentEnv() noexcept {
    return _envStack.empty() ? _globalEnv : _envStack.back();
}

void SemanticResolver::pushEnv() {
    _envStack.emplace_back(currentEnv());
}

void SemanticResolver::popEnv() {
    assert(!_envStack.empty());
    _envStack.pop_back();
}

void SemanticResolver::declare(Name* node, Env& env, SymbolKind kind, bool isReassignable) {
    assert(node != nullptr);

    NameDeclarator declarator{_symbolTable, _nodeSymbolMap, env, kind, isReassignable, _diagnostics};
    node->accept(declarator);
}

void SemanticResolver::declare(Pattern* pattern, Env& env, SymbolKind kind, bool isReassignable) {
    assert(pattern != nullptr);

    NameDeclarator declarator{_symbolTable, _nodeSymbolMap, env, kind, isReassignable, _diagnostics};
    pattern->accept(declarator);
}

SemanticType* SemanticResolver::getDeclaredType(Node* node) {
    assert(node != nullptr);


}

SemanticType* SemanticResolver::getGlobalDeclaredType(std::string_view name) const {
    Symbol* symbol = _globalEnv.get(name);
    if (symbol == nullptr) {
        return nullptr;
    }
    auto it = _symbolDeclaredTypeMap.find(symbol);
    return it == _symbolDeclaredTypeMap.end() ? nullptr : it->second;
}

bool SemanticResolver::isReassignable(VarModifier::VarKind kind) noexcept {
    return kind == VarModifier::VarKind::Let || kind == VarModifier::VarKind::Ref;
}

bool SemanticResolver::isReassignable(const VarModifier* varmod) noexcept {
    assert(varmod != nullptr);
    return isReassignable(varmod->kind);
}

bool SemanticResolver::isHoistedDeclarative(const Node* node) noexcept {
    assert(node != nullptr);

    if (node->is<FnDefStmt>() || node->is<TypeDefStmt>()) {
        return true;
    }
    return false;
}

void SemanticResolver::cannotFindError(Location start, Location end, std::string_view name) {
    std::ostringstream msg;
    msg << "cannot find name: `" << name << "`";
    _diagnostics.add(Diagnostic::error(start, end, msg.str()));
}



void NameDeclarator::visit(Name* node) {
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
        redeclareError(node->start, node->end, name, symbol->start(), symbol->end());
    }

    // Bind symbol to the `Name` node
    _nodeSymbolMap.set(node, symbol);
}

void NameDeclarator::visit(BindingPattern* pattern) {
    assert(pattern != nullptr);
    pattern->name->accept(*this);
}

void NameDeclarator::visit(TuplePattern* pattern) {
    assert(pattern != nullptr);
    for (Pattern* p : pattern->patterns) {
        p->accept(*this);
    }
}

void NameDeclarator::visit(CollectionPattern* pattern) {
    assert(pattern != nullptr);
    for (Pattern* p : pattern->prefix) {
        p->accept(*this);
    }
    for (Pattern* p : pattern->suffix) {
        p->accept(*this);
    }
}

void NameDeclarator::visit(RecordPattern* pattern) {
    assert(pattern != nullptr);
    for (RecordPatternField* field : pattern->fields) {
        field->pattern->accept(*this);
    }
}

void NameDeclarator::redeclareError(Location start,
                                        Location end,
                                        std::string_view name,
                                        Location prevStart,
                                        Location prevEnd) noexcept {
    std::ostringstream msg;
    msg << "redeclaration of name: `" << name << "`";
    _diagnostics.add(Diagnostic::error(start, end, msg.str(), {
        Diagnostic::note(prevStart, prevEnd, "previously declared here")
    }));
}

} // Spark::FrontEnd
