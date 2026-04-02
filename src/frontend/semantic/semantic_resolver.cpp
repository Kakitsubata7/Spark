#include "semantic_resolver.hpp"

#include <sstream>
#include <variant>

namespace Spark::FrontEnd {

SemanticResolver::SemanticResolver(Diagnostics& diagnostics) : _diagnostics(diagnostics) {
    _unknownType = new StructType{"<Unknown>"};
    _voidType = new StructType{"Void"};
    // TODO
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

void SemanticResolver::visit(LambdaExpr* lambda) {
    assert(lambda != nullptr);
    error(lambda->start, lambda->end, "lambda expression is not supported");
}

void SemanticResolver::visit(IfThenExpr* ifthen) {
    assert(ifthen != nullptr);

}

void SemanticResolver::visit(TryElseExpr* tryelse) {
    assert(tryelse != nullptr);
    error(tryelse->start, tryelse->end, "`try ... else ...` expression is not supported");
}

void SemanticResolver::visit(MatchExpr* match) {
    assert(match != nullptr);
    error(match->start, match->end, "`match` expression is not supported");
}

void SemanticResolver::visit(TryCatchExpr* trycatch) {
    assert(trycatch != nullptr);
    error(trycatch->start, trycatch->end, "`try ... catch ...` expression is not supported");
}

void SemanticResolver::visit(ThrowExpr* t) {
    assert(t != nullptr);
    error(t->start, t->end, "`throw` expression is not supported");
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

    // Sets type
    _currentType = type;

    popEnv();
}

void SemanticResolver::visit(IsExpr* is) {
    assert(is != nullptr);
    error(is->start, is->end, "`is` expression is not supported");
}

void SemanticResolver::visit(AsExpr* as) {
    assert(as != nullptr);
    error(as->start, as->end, "`as` expression is not supported");
}

void SemanticResolver::visit(BinaryExpr* binary) {
    assert(binary != nullptr);

    // Resolves LHS and RHS expressions
    SemanticType* lhsType = resolve(binary->lhs);
    SemanticType* rhsType = resolve(binary->rhs);

    // If the operator is overloadable
    if (auto r = binaryOpToIdent(binary->op)) {
        std::string_view name = r.value();

        // Looks up for free operator overload
        Symbol* opSymbol = currentEnv().lookup(name);
        if (opSymbol == nullptr) {
            // TODO: Looks up for instance operator overload
        }

        // If no operator overload is found, error
        if (false) { // TODO: Update condition
            cannotFindOperatorError(binary->start, binary->end, name, lhsType, rhsType);
            _currentType = unknownType();
            return;
        }

        _currentType = nullptr; // TODO: Set to the return type of the semantic function
        return;
    }

    // The operator is not overloadable
    switch (binary->op) {
        case BinaryExpr::OpKind::StrictEq: {
            // Makes sure LHS and RHS are of the same type
            if (lhsType != rhsType) {
                invalidStrictEqError(binary->start, binary->end, lhsType, rhsType);
                break;
            }


            break;
        }

        case BinaryExpr::OpKind::StrictNe: {
            // Makes sure LHS and RHS are of the same type
            if (lhsType != rhsType) {
                invalidStrictNeError(binary->start, binary->end, lhsType, rhsType);
                break;
            }


            break;
        }

        case BinaryExpr::OpKind::Coalesce:
            break;

        case BinaryExpr::OpKind::FuncType:
            break;

        case BinaryExpr::OpKind::Pipe:
            break;

        default:
            assert(false && "invalid `BinaryExpr::OpKind` value");
            break;
    }

    // Cannot find operator error
    cannotFindOperatorError(binary->start, binary->end, opName, lhsType, rhsType);
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

void SemanticResolver::visit(MemberAccessExpr* maccess) {
    assert(maccess != nullptr);

    // Resolves base
    SemanticType* baseType = resolve(maccess->base);
}

void SemanticResolver::visit(CallExpr* call) {
    assert(call != nullptr);

    // Resolves callee
    SemanticType* calleeType = resolve(call->callee);

    // Resolves args
    std::vector<SemanticType*> argTypes{call->args.size()};
    for (size_t i = 0; i < call->args.size(); ++i) {
        CallArg* arg = call->args[i];

        // Resolves arg
        argTypes[i] = resolve(arg->expr);

        // Named argument is currently not supported
        if (arg->name != nullptr) {
            error(arg->name->start, arg->name->end, "named argument is not supported");
        }
    }

    // Gets the semantic function
    if (SemanticFunc* func = calleeType->getCallable(argTypes); func != nullptr) {

    } else {
        notCallableError(call->start, call->end, calleeType, argTypes);
    }
}

void SemanticResolver::visit(SubscriptExpr* subscript) {
    assert(subscript != nullptr);

    // Resolves base
    SemanticType* baseType = resolve(subscript->base);

    // Resolves indices
    for () {

    }
}

void SemanticResolver::visit(LiteralExpr* literal) {
    assert(literal != nullptr);

    std::visit([&](auto&& value) {
         using T = std::decay_t<decltype(value)>;

         if constexpr (std::is_same_v<T, VoidLiteral>) {
             // Sets type
             _currentType = voidType();
         } else if constexpr (std::is_same_v<T, IntLiteral>) {
             // Sets type
            _currentType = intType();
         } else if constexpr (std::is_same_v<T, RealLiteral>) {
             // Sets type
            _currentType = realType();
         } else if constexpr (std::is_same_v<T, BoolLiteral>) {
             // Sets type
            _currentType = boolType();
         } else if constexpr (std::is_same_v<T, StringLiteral>) {
             // Sets type
            _currentType = stringType();
         } else if constexpr (std::is_same_v<T, NilLiteral>) {
             // Sets type
            _currentType = nilType();
         }
     }, literal->literal);
}

void SemanticResolver::visit(NameExpr* ident) {
    assert(ident != nullptr);

    // Gets the symbol of the name
    std::string_view name = ident->name->value.str();

    // Resolves name and type
    if (Symbol* symbol = currentEnv().lookup(name); symbol != nullptr) {
        _currentType = symbol->type;
    } else {
        cannotFindError(ident->start, ident->end, name);
        _currentType = unknownType();
    }
}

void SemanticResolver::visit(GlobalAccessExpr* gaccess) {
    assert(gaccess != nullptr);
    error(gaccess->start, gaccess->end, "`global` expression is not supported");
}

void SemanticResolver::visit(UpvalueExpr* upvalue) {
    assert(upvalue != nullptr);
    error(upvalue->start, upvalue->end, "`$` operator is not supported");
}

void SemanticResolver::visit(TupleExpr* tuple) {
    assert(tuple != nullptr);
    error(tuple->start, tuple->end, "tuple expression is not supported");
}

void SemanticResolver::visit(CollectionExpr* collection) {
    assert(collection != nullptr);
    error(collection->start, collection->end, "collection expression is not supported");
}

void SemanticResolver::visit(TypeofExpr* t) {
    assert(t != nullptr);
    error(t->start, t->end, "`typeof` expression is not supported");
}

void SemanticResolver::visit(VarDefStmt* vardef) {
    assert(vardef != nullptr);

    // Declares name into the current environment
    declare(vardef->pattern, currentEnv(), SymbolKind::Var, isReassignable(vardef->mod));

    // Sets type
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

    // Sets type
    _currentType = voidType();
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

    // Sets type
    _currentType = voidType();
}

void SemanticResolver::visit(AssignStmt* assign) {
    assert(assign != nullptr);

    if (assign->lhs->as<>()) {

    }
}

void SemanticResolver::visit(IfStmt* ifstmt) {
    assert(ifstmt != nullptr);

    // Resolves condition and makes sure it's a boolean
    SemanticType* condType = resolve(ifstmt->condition);
    if (!condType->is(boolType())) {
        unexpectedTypeError(ifstmt->condition->start, ifstmt->condition->end, boolType(), condType);
    }

    // Resolves then body
    resolve(ifstmt->thenBody);

    // Resolves else body
    if (ifstmt->elseBody != nullptr) {
        resolve(ifstmt->elseBody);
    }

    // Sets type
    _currentType = voidType();
}

void SemanticResolver::visit(WhileStmt* w) {
    assert(w != nullptr);

    // Resolves condition and makes sure it's a boolean
    SemanticType* condType = resolve(w->condition);
    if (!condType->is(boolType())) {
        unexpectedTypeError(w->condition->start, w->condition->end, boolType(), condType);
    }

    // Resolves body
    resolve(w->body);

    // Sets type
    _currentType = voidType();
}

void SemanticResolver::visit(DoWhileStmt* dw) {
    assert(dw != nullptr);
    error(dw->start, dw->end, "`do ... while` statement is not supported");
}

void SemanticResolver::visit(ForStmt* f) {
    assert(f != nullptr);
    error(f->start, f->end, "`for` statement is not supported");
}

void SemanticResolver::visit(BreakStmt* b) {
    assert(b != nullptr);
}

void SemanticResolver::visit(ContinueStmt* c) {
    assert(c != nullptr);
}

void SemanticResolver::visit(ReturnStmt* ret) {
    assert(ret != nullptr);

    // Resolves returned expression
    if (ret->expr != nullptr) {
        resolve(ret->expr);
    }

    // Sets type
    _currentType = voidType();
}

void SemanticResolver::visit(ModuleStmt* moddef) {
    assert(moddef != nullptr);
    error(moddef->start, moddef->end, "`module` statement is not supported");
}

void SemanticResolver::visit(ExportStmt* e) {
    assert(e != nullptr);
    error(e->start, e->end, "`export` statement is not supported");
}

void SemanticResolver::visit(ImportStmt* i) {
    assert(i != nullptr);
    error(i->start, i->end, "`import` statement is not supported");
}

void SemanticResolver::visit(ImportAllStmt* i) {
    assert(i != nullptr);
    error(i->start, i->end, "`import` statement is not supported");
}

void SemanticResolver::visit(UndefineStmt* undef) {
    assert(undef != nullptr);
    error(undef->start, undef->end, "`undefine` statement is not supported");
}

SemanticType* SemanticResolver::resolve(Node* node) {
    assert(node != nullptr);

    node->accept(*this);
    return _currentType;
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

    NameDeclarator declarator{_symbolTable, env, kind, isReassignable, _diagnostics};
    node->accept(declarator);
}

void SemanticResolver::declare(Pattern* pattern, Env& env, SymbolKind kind, bool isReassignable) {
    assert(pattern != nullptr);

    NameDeclarator declarator{_symbolTable, env, kind, isReassignable, _diagnostics};
    pattern->accept(declarator);
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

std::optional<std::string_view> SemanticResolver::binaryOpToIdent(BinaryExpr::OpKind op) noexcept {
    switch (op) {
        case BinaryExpr::OpKind::Add:
            return "operator+";

        case BinaryExpr::OpKind::Sub:
            return "operator-";

        case BinaryExpr::OpKind::Mul:
            return "operator*";

        case BinaryExpr::OpKind::Div:
            return "operator/";

        case BinaryExpr::OpKind::Mod:
            return "operator%";

        case BinaryExpr::OpKind::Lt:
            return "operator<";

        case BinaryExpr::OpKind::Le:
            return "operator<=";

        case BinaryExpr::OpKind::Gt:
            return "operator>";

        case BinaryExpr::OpKind::Ge:
            return "operator>=";

        case BinaryExpr::OpKind::Eq:
            return "operator==";

        case BinaryExpr::OpKind::Ne:
            return "operator!=";

        case BinaryExpr::OpKind::BitAnd:
            return "operator&";

        case BinaryExpr::OpKind::BitOr:
            return "operator|";

        case BinaryExpr::OpKind::BitXor:
            return "operator^";

        case BinaryExpr::OpKind::BitShl:
            return "operator<<";

        case BinaryExpr::OpKind::BitShr:
            return "operator>>";

        case BinaryExpr::OpKind::LogAnd:
            return "operator&&";

        case BinaryExpr::OpKind::LogOr:
            return "operator||";

        case BinaryExpr::OpKind::Range:
            return "operator...";

        case BinaryExpr::OpKind::RangeExcl:
            return "operator..<";

        default:
            return std::nullopt;
    }
}

std::optional<std::string_view> SemanticResolver::prefixOpToIdent(PrefixExpr::OpKind op) noexcept {
    switch (op) {
        case PrefixExpr::OpKind::Pos:
            return "operator(+)";

        case PrefixExpr::OpKind::Neg:
            return "operator(-)";

        case PrefixExpr::OpKind::BitNot:
            return "operator~";

        case PrefixExpr::OpKind::LogNot:
            return "operator!";

        default:
            return std::nullopt;
    }
}

void SemanticResolver::error(Location start, Location end, std::string message, std::vector<Diagnostic> subs) {
    _diagnostics.add(Diagnostic::error(start, end, std::move(message), std::move(subs)));
}

void SemanticResolver::cannotFindError(Location start, Location end, std::string_view name) {
    std::ostringstream msg;
    msg << "cannot find name: `" << name << "`";
    error(start, end, msg.str());
}

void SemanticResolver::cannotFindOperatorError(Location start,
                                               Location end,
                                               std::string_view opName,
                                               const SemanticType* lhsType,
                                               const SemanticType* rhsType) {
    assert(lhsType != nullptr);
    assert(rhsType != nullptr);

    std::ostringstream msg;
    msg << "cannot find operator: `" << opName
        << "` between types `" << lhsType->name() << "` and `" << rhsType->name() << "`";
    error(start, end, msg.str());
}

void SemanticResolver::unexpectedTypeError(Location start,
                                           Location end,
                                           const SemanticType* expected,
                                           const SemanticType* actual) {
    assert(expected != nullptr);
    assert(actual != nullptr);

    std::ostringstream msg;
    msg << "unexpected type: `" << expected->name() << "`, expecting `" << actual->name() << "`";
    error(start, end, msg.str());
}

void SemanticResolver::notCallableError(Location start,
                                        Location end,
                                        const SemanticType* type,
                                        const std::vector<SemanticType*>& paramTypes) {
    assert(type != nullptr);

    std::ostringstream msg;
    msg << "type `" << type->name() << "` is not callable with parameter types: ";
    for (auto it = paramTypes.begin(); it != paramTypes.end(); ++it) {
        msg << "`" << (*it)->name() << "`";
        if (std::next(it) != paramTypes.end()) {
            msg << ", " << *it;
        }
    }
    error(start, end, msg.str());
}

void SemanticResolver::invalidStrictEqError(Location start,
                                            Location end,
                                            const SemanticType* lhsType,
                                            const SemanticType* rhsType) {
    assert(lhsType != nullptr);
    assert(rhsType != nullptr);

    std::ostringstream msg;
    msg << "cannot compare values of different types using `===`: `"
        << lhsType->name() << "` and `" << rhsType->name() << "`";
    error(start, end, msg.str());
}

void SemanticResolver::invalidStrictNeError(Location start,
                                            Location end,
                                            const SemanticType* lhsType,
                                            const SemanticType* rhsType) {
    assert(lhsType != nullptr);
    assert(rhsType != nullptr);

    std::ostringstream msg;
    msg << "cannot compare values of different types using `!==`: `"
        << lhsType->name() << "` and `" << rhsType->name() << "`";
    error(start, end, msg.str());
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

        // Bind name to symbol in the environment
        _env.set(name, symbol);
    } else {
        // Redeclaration error
        redeclareError(node->start, node->end, name, symbol->start(), symbol->end());
    }
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
