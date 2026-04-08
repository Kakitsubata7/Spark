#include "semantic_resolver.hpp"

#include <sstream>
#include <variant>

namespace Spark::FrontEnd {

SemanticResolver::SemanticResolver(Diagnostics& diagnostics) : _diagnostics(diagnostics) {
    _unknownType = _typeTable.makeStructType("<unknown>");
    _voidType = _typeTable.makeStructType("Void");
    _intType = _typeTable.makeStructType("Int");
    _realType = _typeTable.makeStructType("Real");
    _boolType = _typeTable.makeStructType("Bool");
    _stringType = _typeTable.makeClassType("String");
    _nilType = _typeTable.makeStructType("Nil");
    _typeType = _typeTable.makeTypeType("Type", _unknownType);
}

void SemanticResolver::visit(LambdaExpr* lambda) {
    assert(lambda != nullptr);
    error(lambda->start, lambda->end, "lambda expression is not supported");
}

void SemanticResolver::visit(IfThenExpr* ifthen) {
    assert(ifthen != nullptr);
    error(ifthen->start, ifthen->end, "`if ... then ...` expression is not supported");
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

    SemanticType* resultType = voidType();

    // Collect function definitions
    std::vector<FnDefStmt*> fndefs;
    for (Node* node : block->nodes) {
        if (FnDefStmt* fndef = node->as<FnDefStmt>()) {
            fndefs.push_back(fndef);
        }
    }

    pushEnv();

    // Resolves function definitions
    declareFunctions(fndefs);

    popEnv();

    // Sets result type
    _resultType = resultType;
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
        if (SemanticFunc* func = findFunc(name, currentEnv(), {lhsType, rhsType})) {


            // Sets result type
            _resultType = func->sig().returnType();
            return;
        }

        // If no free operator overload was found, looking up for instance operator overload in LHS type
        if (SemanticFunc* func = findFunc(name, lhsType, {rhsType})) {


            // Sets result type
            _resultType = func->sig().returnType();
            return;
        }

        // If no operator overload is found, error
        cannotFindOperatorError(binary->start, binary->end, name, lhsType, rhsType);
        _resultType = unknownType();
        return;
    }

    // The operator is not overloadable
    switch (binary->op) {
        case BinaryExpr::OpKind::StrictEq: {
            error(binary->start, binary->end, "binary `===` operator is not supported");
            break;
        }

        case BinaryExpr::OpKind::StrictNe: {
            error(binary->start, binary->end, "binary `!==` operator is not supported");
            _resultType = unknownType();
            break;
        }

        case BinaryExpr::OpKind::Coalesce:
            error(binary->start, binary->end, "binary `??` operator is not supported");
            _resultType = unknownType();
            break;

        case BinaryExpr::OpKind::FuncType:
            error(binary->start, binary->end, "binary `->` operator is not supported");
            _resultType = unknownType();
            break;

        case BinaryExpr::OpKind::Pipe:
            error(binary->start, binary->end, "binary `|>` operator is not supported");
            _resultType = unknownType();
            break;

        default:
            assert(false && "invalid `BinaryExpr::OpKind` value");
            break;
    }
}

void SemanticResolver::visit(PrefixExpr* prefix) {
    assert(prefix != nullptr);

    // Resolves expr
    SemanticType* exprType = resolve(prefix->expr);

    // If the operator is overloadable
    if (auto r = prefixOpToIdent(prefix->op)) {
        std::string_view name = r.value();

        // Looks up for free operator overload
        if (SemanticFunc* func = findFunc(name, currentEnv(), {exprType})) {


            // Sets result type
            _resultType = func->sig().returnType();
            return;
        }

        // If no operator overload is found, error
        cannotFindOperatorError(prefix->start, prefix->end, name, exprType);
        _resultType = unknownType();
    } else {
        assert(false && "invalid `PrefixExpr::OpKind` value");
    }
}

void SemanticResolver::visit(PostfixExpr* postfix) {
    assert(postfix != nullptr);

    switch (postfix->op) {
        case PostfixExpr::OpKind::Optional:
            error(postfix->start, postfix->end, "postfix `?` operator is not supported");
            _resultType = unknownType();
            break;

        case PostfixExpr::OpKind::NonNull:
            error(postfix->start, postfix->end, "postfix `?!` operator is not supported");
            _resultType = unknownType();
            break;

        case PostfixExpr::OpKind::ForceUnwrap:
            error(postfix->start, postfix->end, "postfix `!` operator is not supported");
            _resultType = unknownType();
            break;

        default:
            assert(false && "invalid `PostfixExpr::OpKind` value");
            break;
    }
}

void SemanticResolver::visit(MemberAccessExpr* maccess) {
    assert(maccess != nullptr);

    // Resolves base
    SemanticType* baseType = resolve(maccess->base);

    // Get member name
    std::string_view member = maccess->member->value.str();

    // Find member
    if (const RecordType* r = baseType->as<RecordType>()) {
        // Try fields
        const RecordField* targetField = nullptr;
        for (const RecordField& field : r->fields()) {
            if (field.name() == member) {
                targetField = &field;
                break;
            }
        }

        // Found field
        if (targetField != nullptr) {
            _resultType = targetField->type();
            return;
        }

        // Try methods
        const TypeMethod* targetMethod = nullptr;
        for (const TypeMethod& method : r->methods()) {
            if (method.name() == member) {
                targetMethod = &method;
                break;
            }
        }

        // Found method
        if (targetMethod != nullptr) {
            _resultType = targetMethod->type();
            return;
        }
    }

    // Member not found
    typeHasNoMemberError(maccess->member->start, maccess->member->end, baseType, member);
    _resultType = unknownType();
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
    if (SemanticFunc* func = findFunc("operator()", currentEnv(), argTypes)) {
        // Found free definition

        // Sets result type
        _resultType = func->returnType();
    } else if (SemanticFunc* func = findFunc("operator()", calleeType, argTypes)) {
        // Found instance definition

        // Sets result type
        _resultType = func->returnType();
    } else {
        // Cannot find function
        notIndexableError(call->start, call->end, calleeType, argTypes);
        _resultType = unknownType();
    }
}

void SemanticResolver::visit(SubscriptExpr* subscript) {
    assert(subscript != nullptr);

    // Resolves base
    SemanticType* baseType = resolve(subscript->base);

    // Resolves indices
    std::vector<SemanticType*> indexTypes{subscript->indices.size()};
    for (size_t i = 0; i < subscript->indices.size(); ++i) {
        Expr* index = subscript->indices[i];

        // Resolve index
        indexTypes[i] = resolve(index);
    }

    // Gets the semantic function
    if (SemanticFunc* func = findFunc("operator[]", currentEnv(), indexTypes)) {
        // Found free definition

        // Sets result type
        _resultType = func->returnType();
    } else if (SemanticFunc* func = findFunc("operator[]", baseType, indexTypes)) {
        // Found instance definition

        // Sets result type
        _resultType = func->returnType();
    } else {
        // Cannot find function
        notIndexableError(subscript->start, subscript->end, baseType, indexTypes);
        _resultType = unknownType();
    }
}

void SemanticResolver::visit(LiteralExpr* literal) {
    assert(literal != nullptr);

    std::visit([&](auto&& value) {
         using T = std::decay_t<decltype(value)>;

         if constexpr (std::is_same_v<T, VoidLiteral>) {
             // Sets result type
             _resultType = voidType();
         } else if constexpr (std::is_same_v<T, IntLiteral>) {
             // Sets result type
            _resultType = intType();
         } else if constexpr (std::is_same_v<T, RealLiteral>) {
             // Sets result type
            _resultType = realType();
         } else if constexpr (std::is_same_v<T, BoolLiteral>) {
             // Sets result type
            _resultType = boolType();
         } else if constexpr (std::is_same_v<T, StringLiteral>) {
             // Sets result type
            _resultType = stringType();
         } else if constexpr (std::is_same_v<T, NilLiteral>) {
             // Sets result type
            _resultType = nilType();
         }
     }, literal->literal);
}

void SemanticResolver::visit(NameExpr* ident) {
    assert(ident != nullptr);

    // Gets the symbol of the name
    std::string_view name = ident->name->value.str();

    // Resolves name and type
    if (Symbol* symbol = currentEnv().lookup(name); symbol != nullptr) {
        _resultType = symbol->type;
    } else {
        cannotFindError(ident->start, ident->end, name);
        _resultType = unknownType();
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

    // Checks whether the kind is valid
    if (vardef->mod->kind == VarModifier::VarKind::None) {
        error(vardef->mod->start, vardef->mod->end, "invalid variable declaration kind");
        _resultType = voidType();
        return;
    }

    // Reference declaration is not supported
    if (vardef->mod->kind == VarModifier::VarKind::Ref || vardef->mod->kind == VarModifier::VarKind::Cref) {
        error(vardef->mod->start, vardef->mod->end, "reference declaration is not supported");
        _resultType = voidType();
        return;
    }

    // Makes sure RHS is not empty
    SemanticType* rhsType;
    if (vardef->rhs != nullptr) {
        rhsType = resolve(vardef->rhs);
    } else {
        error(vardef->start, vardef->end, "variable must be initialized at declaration");
        rhsType = unknownType();
    }

    // Gets variable type
    SemanticType* varType;
    if (vardef->type != nullptr) {
        if (TypeType* t = resolve(vardef->type)->as<TypeType>()) {
            varType = t->declaredType();
        } else {
            unexpectedTypeError(vardef->type->start, vardef->type->end, typeType(), t);
            varType = unknownType();
        }
    } else {
        // Infers type based on RHS type
        varType = rhsType;
    }

    // Declares name into the current environment
    declare(vardef->pattern, currentEnv(), SymbolKind::Var, isReassignable(vardef->mod));

    // Sets result type
    _resultType = voidType();
}

void SemanticResolver::visit(FnDefStmt* fndef) {
    assert(fndef != nullptr);

    // Resolves parameters
    std::vector<SemanticType*> paramTypes{fndef->params.size()};
    for (size_t i = 0; i < fndef->params.size(); ++i) {
        FnParam* param = fndef->params[i];

        // Makes sure type expression produces a type
        SemanticType* type = resolve(param->type);
        if (const TypeType* t = type->as<TypeType>()) {
            paramTypes[i] = t->declaredType();
        } else {
            unexpectedTypeError(param->type->start, param->type->end, typeType(), t);
            paramTypes[i] = unknownType();
        }
    }

    // Resolves return type expression
    SemanticType* returnType;
    if (fndef->returns.empty()) {
        returnType = voidType();
    } else {
        FnReturn* front = fndef->returns.front();

        // Multiple returns is not supported
        if (fndef->returns.size() > 1) {
            error(front->start, front->end, "function with multiple returns is not supported");
        }

        // Return by reference is not supported
        if (front->kind == FnReturn::RetKind::ByRef || front->kind == FnReturn::RetKind::ByCref) {
            error(front->start, front->end, "return by reference is not supported");
            returnType = voidType();
        }
        // Return by value
        else if (front->kind == FnReturn::RetKind::ByValue) {
            // Makes sure type expression produces a type
            SemanticType* t = resolve(front->type);
            if (const TypeType* tt = t->as<TypeType>()) {
                returnType = tt->declaredType();
            } else {
                unexpectedTypeError(front->type->start, front->type->end, typeType(), t);
                returnType = unknownType();
            }
        } else {
            assert(false && "invalid `FnReturn::RetKind` value");
        }
    }

    // Constructs semantic function
    SemanticFunc* func = _funcTable.make(std::move(paramTypes), returnType);

    // Constructs semantic type
    MonoFuncType* type = _typeTable.makeMonoFuncType(createMonoFuncTypeName(paramTypes, returnType), func);

    // Declares function
    declare(fndef->name, currentEnv(), SymbolKind::Func, false);

    // Enter function scope
    pushEnv();

    // Declares parameters
    for (size_t i = 0; i < fndef->params.size(); ++i) {
        FnParam* param = fndef->params[i];
        SemanticType* paramType = paramTypes[i];
        declare(param->pattern, currentEnv(), SymbolKind::Var, isReassignable(param->mod));
    }

    // Exit function scope
    popEnv();

    // Sets type
    _resultType = voidType();
}

void SemanticResolver::visit(TypeDefStmt* tdef) {
    assert(tdef != nullptr);

    // Makes sure declared type is a struct
    if (tdef->kind != TypeDefStmt::TypeKind::Struct) {
        error(tdef->start, tdef->end, "only `struct` type declaration is supported");
        _resultType = voidType();
        return;
    }

    // Resolves base type list
    std::vector<SemanticType*> baseTypes{tdef->bases.size()};
    for (size_t i = 0; i < baseTypes.size(); ++i) {
        baseTypes[i] = resolve(tdef->bases[i]);
    }

    // Get traits
    std::vector<TraitType*> traits;
    traits.reserve(baseTypes.size());
    for (SemanticType* baseType : baseTypes) {
        if (TypeType* t = baseType->as<TypeType>()) {
            if (TraitType* trait = t->declaredType()->as<TraitType>()) {
                traits.push_back(trait);
            } else {
                invalidStructInheritanceError(t->start(), t->end(), t->declaredType());
            }
        } else {
            unexpectedTypeError(baseType->start(), baseType->end(), typeType(), t);
        }
    }

    // TODO: Resolves type body
    pushEnv();
    for (Node* node : tdef->body->nodes) {
        if (VarDefStmt* vardef = node->as<VarDefStmt>()) {

        } else if (FnDefStmt* fndef = node->as<FnDefStmt>()) {

        } else {
            error(node->start, node->end, "invalid statement inside type declaration body");
        }
    }
    popEnv();

    // Constructs the semantic type object
    SemanticType* type = _typeTable.makeStructType();

    // Declares type name
    declare(tdef->name, currentEnv(), SymbolKind::Type, false);

    // Sets result type
    _resultType = voidType();
}

void SemanticResolver::visit(AssignStmt* assign) {
    assert(assign != nullptr);

    // Only `operator=` is supported
    if (assign->op != AssignStmt::OpKind::Assign) {
        error(assign->start, assign->end, "only `=` assignment operator is supported");
        _resultType = voidType();
        return;
    }

    // Makes sure LHS is an identifier
    NameExpr* lhs = assign->lhs->as<NameExpr>();
    if (lhs == nullptr) {
        error(assign->lhs->start, assign->lhs->end, "invalid LHS of assignment statement");
        _resultType = voidType();
        return;
    }

    // Makes sure RHS is an expression
    Expr* rhs = assign->rhs->as<Expr>();
    if (rhs == nullptr) {
        error(assign->rhs->start, assign->rhs->end, "invalid RHS of assignment statement");
        _resultType = voidType();
        return;
    }

    // Resolves LHS identifier
    SemanticType* lhsType = resolve(lhs);

    // Resolves RHS expression
    SemanticType* rhsType = resolve(rhs);

    // Make sure LHS type and RHS type match
    if (!lhsType->isIdentical(rhsType)) {
        unexpectedTypeError(assign->start, assign->end, lhsType, rhsType);
        _resultType = voidType();
        return;
    }

    // Sets result type
    _resultType = voidType();
}

void SemanticResolver::visit(IfStmt* ifstmt) {
    assert(ifstmt != nullptr);

    // Resolves condition and makes sure it's a boolean
    SemanticType* condType = resolve(ifstmt->condition);
    if (!condType->isIdentical(boolType())) {
        unexpectedTypeError(ifstmt->condition->start, ifstmt->condition->end, boolType(), condType);
    }

    // Resolves then body
    resolve(ifstmt->thenBody);

    // Resolves else body
    if (ifstmt->elseBody != nullptr) {
        resolve(ifstmt->elseBody);
    }

    // Sets type
    _resultType = voidType();
}

void SemanticResolver::visit(WhileStmt* w) {
    assert(w != nullptr);

    // Resolves condition and makes sure it's a boolean
    SemanticType* condType = resolve(w->condition);
    if (!condType->isIdentical(boolType())) {
        unexpectedTypeError(w->condition->start, w->condition->end, boolType(), condType);
    }

    // Resolves body
    bool wasInLoop = _inLoop;
    _inLoop = true;
    resolve(w->body);
    _inLoop = wasInLoop;

    // Sets type
    _resultType = voidType();
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

    // Makes sure in a loop
    if (!_inLoop) {
        error(b->start, b->end, "`break` is only allowed inside a loop");
    }

    // Sets result type
    _resultType = voidType();
}

void SemanticResolver::visit(ContinueStmt* c) {
    assert(c != nullptr);

    // Makes sure in a loop
    if (!_inLoop) {
        error(c->start, c->end, "`continue` is only allowed inside a loop");
    }

    // Sets result type
    _resultType = voidType();
}

void SemanticResolver::visit(ReturnStmt* ret) {
    assert(ret != nullptr);

    // Makes sure in a function
    if (!_inFunc) {
        error(ret->start, ret->end, "`return` is only allowed inside a function");
    }

    // Resolves returned expression
    if (ret->expr != nullptr) {
        resolve(ret->expr);
    }

    // Sets type
    _resultType = voidType();
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
    return _resultType;
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

void SemanticResolver::declare(std::string_view name,
                               Env& env,
                               SymbolKind kind,
                               bool isReassignable,
                               SemanticType* type,
                               Location start,
                               Location end) {
    assert(type != nullptr);

    // Checks if the name is already declared locally in the environment
    Symbol* symbol = env.get(name);
    if (symbol == nullptr) {
        // Create symbol
        symbol = _symbolTable.make(Symbol{
            .kind = kind,
            .isReassignable = isReassignable,
            .type = type,
            .name = std::string{name},
            .start = start,
            .end = end,
        });

        // Bind name to symbol in the environment
        env.set(name, symbol);
    } else {
        // Redeclaration error
        redeclareError(start, end, name, symbol->start, symbol->end);
    }
}

void SemanticResolver::declareFunctions(const std::vector<FnDefStmt*>& fndefs, Env& env) {
    for (FnDefStmt* fndef : fndefs) {
        assert(fndef != nullptr);
    }

    for (FnDefStmt* fndef : fndefs) {
        // Gets function name
        std::string_view name = fndef->name->value.str();

        // Resolves parameters
        std::vector<SemanticType*> paramTypes{fndef->params.size()};
        for (size_t i = 0; i < fndef->params.size(); ++i) {
            FnParam* param = fndef->params[i];

            // Makes sure type expression produces a type
            SemanticType* type = resolve(param->type);
            if (const TypeType* t = type->as<TypeType>()) {
                paramTypes[i] = t->declaredType();
            } else {
                unexpectedTypeError(param->type->start, param->type->end, typeType(), t);
                paramTypes[i] = unknownType();
            }
        }

        // Resolves return type expression
        SemanticType* returnType;
        if (fndef->returns.empty()) {
            returnType = voidType();
        } else {
            FnReturn* front = fndef->returns.front();

            // Multiple returns is not supported
            if (fndef->returns.size() > 1) {
                error(front->start, front->end, "function with multiple returns is not supported");
            }

            // Return by reference is not supported
            if (front->kind == FnReturn::RetKind::ByRef || front->kind == FnReturn::RetKind::ByCref) {
                error(front->start, front->end, "return by reference is not supported");
                returnType = voidType();
            }
            // Return by value
            else if (front->kind == FnReturn::RetKind::ByValue) {
                // Makes sure type expression produces a type
                SemanticType* t = resolve(front->type);
                if (const TypeType* tt = t->as<TypeType>()) {
                    returnType = tt->declaredType();
                } else {
                    unexpectedTypeError(front->type->start, front->type->end, typeType(), t);
                    returnType = unknownType();
                }
            } else {
                assert(false && "invalid `FnReturn::RetKind` value");
            }
        }

        // Constructs semantic function
        SemanticFunc* func = _funcTable.make(std::move(paramTypes), returnType);

        // Constructs semantic type
        MonoFuncType* type = _typeTable.makeMonoFuncType(createMonoFuncTypeName(paramTypes, returnType), func);

        // Checks if the name is already declared
        if (Symbol* symbol = env.get(name)) {
            // Checks if the name was declared with a different kind
            if (symbol->kind != SymbolKind::Func) {
                redeclareError(fndef->start, fndef->end, name, symbol->start, symbol->end);
                continue;
            }

            // TODO: HERE!
            if (symbol->type->as<MonoFuncType>()) {

            }
            else if (symbol->type->as<OverloadedFuncType>()) {

            }

            continue;
        }

        // Declares function
        declare(name, env, SymbolKind::Func, false, type, fndef->name->start, fndef->name->end);
    }
}

SemanticFunc* SemanticResolver::findFunc(std::string_view name,
                                         const Env& currentEnv,
                                         const std::vector<SemanticType*>& paramTypes) {
    if (Symbol* opSymbol = currentEnv.lookup(name); opSymbol != nullptr) {
        assert(opSymbol->kind == SymbolKind::Func);

        if (const MonoFuncType* t = opSymbol->type->as<MonoFuncType>()) {
            if (t->func()->isCallableWith(paramTypes)) {
                return t->func();
            }
        } else if (const OverloadedFuncType* t = opSymbol->type->as<OverloadedFuncType>()) {
            for (MonoFuncType* funcType : t->funcTypes()) {
                if (funcType->func()->isCallableWith(paramTypes)) {
                    return funcType->func();
                }
            }
        } else {
            assert(false && "function symbol found with non-function type");
        }
    }
    return nullptr;
}

SemanticFunc* SemanticResolver::findFunc(std::string_view name,
                                         const SemanticType* type,
                                         const std::vector<SemanticType*>& paramTypes) {
    assert(type != nullptr);

    if (const RecordType* r = type->as<RecordType>()) {
        for (const TypeMethod& method : r->methods()) {
            if (method.name() == name) {
                method.type()->as<>()
            }
        }
    }

    return nullptr;
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

std::string SemanticResolver::createMonoFuncTypeName(const std::vector<SemanticType*>& paramTypes,
                                                     const SemanticType* returnType) {
    for (SemanticType* paramType : paramTypes) {
        assert(paramType != nullptr);
    }
    assert(returnType != nullptr);

    std::ostringstream oss;
    if (paramTypes.empty()) {
        oss << "()";
    } else if (paramTypes.size() == 1) {
        oss << paramTypes.front()->name();
    } else {
        oss << "(";
        for (size_t i = 0; i < paramTypes.size(); ++i) {
            SemanticType* paramType = paramTypes[i];
            oss << paramType->name();
            if (i != paramTypes.size() - 1) {
                oss << ", ";
            }
        }
        oss << ")";
    }
    oss << " -> " << returnType->name();
    return oss.str();
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
                                                   const SemanticType* type) {
    assert(type != nullptr);

    std::ostringstream msg;
    msg << "cannot find operator: `" << opName << "` for type `" << type->name() << "`";
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

void SemanticResolver::cannotFindOperatorError(Location start,
                                               Location end,
                                               std::string_view opName,
                                               const std::vector<SemanticType*>& types) {
    for (SemanticType* type : types) {
        assert(type != nullptr);
    }

    std::ostringstream msg;
    msg << "cannot find operator: `" << opName << "`";
    if (!types.empty()) {
        msg << " for types: ";
        for (size_t i = 0; i < types.size(); ++i) {
            SemanticType* type = types[i];
            msg << type->name();
            if (i != types.size() - 1) {
                msg << ", ";
            }
        }
    }
    error(start, end, msg.str());
}

void SemanticResolver::redeclareError(Location start,
                                      Location end,
                                      std::string_view name,
                                      Location prevStart,
                                      Location prevEnd) {
    std::ostringstream msg;
    msg << "redeclaration of name: `" << name << "`";
    error(start, end, msg.str(), {
        Diagnostic::note(prevStart, prevEnd, "previously declared here")
    });
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
    for (SemanticType* paramType : paramTypes) {
        assert(paramType != nullptr);
    }

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

void SemanticResolver::notIndexableError(Location start,
                                         Location end,
                                         const SemanticType* type,
                                         const std::vector<SemanticType*>& indexTypes) {
    assert(type != nullptr);
    for (SemanticType* indexType : indexTypes) {
        assert(indexType != nullptr);
    }

    std::ostringstream msg;
    msg << "type `" << type->name() << "` is not indexable with index types: ";
    for (auto it = indexTypes.begin(); it != indexTypes.end(); ++it) {
        msg << "`" << (*it)->name() << "`";
        if (std::next(it) != indexTypes.end()) {
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

void SemanticResolver::invalidStructInheritanceError(Location start, Location end, const SemanticType* type) {
    assert(type != nullptr);

    std::ostringstream msg;
    msg << "invalid struct inheritance from non-trait type `" << type->name() << "`";
    error(start, end, msg.str());
}

void SemanticResolver::typeHasNoMemberError(Location start,
                                            Location end,
                                            const SemanticType* type,
                                            std::string_view member) {
    assert(type != nullptr);

    std::ostringstream msg;
    msg << "type `" << type->name() << "` has no member `" << member << "`";
    error(start, end, msg.str());
}

} // Spark::FrontEnd
