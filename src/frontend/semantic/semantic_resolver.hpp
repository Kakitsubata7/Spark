#pragma once

#include <string_view>

#include "c_emitter.hpp"
#include "env.hpp"
#include "frontend/ast.hpp"
#include "semantic_type.hpp"
#include "symbol.hpp"
#include "utils/diagnostic.hpp"

namespace Spark::FrontEnd {

/**
 * Represents a visitor that declares names and resolves name usages, creating environment mappings for each scope.
 */
class SemanticResolver : public NodeVisitor {
private:
    SymbolTable& _symbolTable;
    NodeSymbolMap& _nodeSymbolMap;

    TypeTable& _typeTable;

    Env& _globalEnv;
    std::vector<Env> _envStack;

    CEmitter& _emitter;

    Diagnostics& _diagnostics;

public:
    SemanticResolver(SymbolTable& symbolTable,
                     NodeSymbolMap& nodeSymbolMap,
                     TypeTable& typeTable,
                     Env& globalEnv,
                     CEmitter& emitter,
                     Diagnostics& diagnostics)
        : _symbolTable(symbolTable), _nodeSymbolMap(nodeSymbolMap), _typeTable(typeTable), _globalEnv(globalEnv),
          _emitter(emitter), _diagnostics(diagnostics) { }

    void visit(Name* node) override;
    void visit(FnParam* param) override;

    void visit(IfThenExpr* ifthen) override;
    void visit(BlockExpr* block) override;
    void visit(BinaryExpr* binary) override;
    void visit(PrefixExpr* prefix) override;
    void visit(PostfixExpr* postfix) override;
    void visit(LiteralExpr* literal) override;
    void visit(NameExpr* ident) override;

    void visit(VarDefStmt* vardef) override;
    void visit(FnDefStmt* fndef) override;
    void visit(TypeDefStmt* tdef) override;
    void visit(IfStmt* ifstmt) override;
    void visit(WhileStmt* w) override;
    void visit(DoWhileStmt* dowhile) override;
    void visit(ReturnStmt* ret) override;

private:
    void resolve(LiteralExpr* literal, std::string& src);

    /**
     * Gets the reference to the current environment. If no lexical environment is pushed, this will be the global
     * environment.
     * @return Reference to the current environment.
     */
    Env& currentEnv() noexcept;

    /**
     * Creates and pushes a new lexical environment. It will be the new current environment.
     */
    void pushEnv();

    /**
     * Pops the current environment. The global environment cannot be popped.
     */
    void popEnv();

    /**
     * Creates symbol and declares a name to the given environment by specifying its symbol kind and reassignability.
     * If the name already exists, no new symbol will be created, but the name will be mapped to the existing symbol.
     * @param node `Name` node to declare.
     * @param env Environment to declare the name in.
     * @param kind Symbol kind of the symbol to declare.
     * @param isReassignable Whether the symbol to declare is reassignable or not.
     */
    void declare(Name* node, Env& env, SymbolKind kind, bool isReassignable);

    /**
     * Creates symbol(s) and declares name(s) to the given environment by specifying their symbol kind(s) and
     * reassignability.
     * If a name already exists, no new symbol will be created, but the name will be mapped to the existing symbol.
     * @param pattern `Pattern` node containing names to declare.
     * @param env Environment to declare the name(s) in.
     * @param kind Symbol kind of the symbol(s) to declare.
     * @param isReassignable Whether the symbol(s) to declare are reassignable or not.
     */
    void declare(Pattern* pattern, Env& env, SymbolKind kind, bool isReassignable);

    /**
     * Gets the type that the name declares in the global environment.
     * @param name Name to get.
     * @return Pointer to the `SemanticType` instance declared by the name, `nullptr` if not found.
     */
    [[nodiscard]]
    SemanticType* getGlobalDeclaredType(std::string_view name) const;

    void resolveType(Node* node, SemanticType* type);

    /**
     * Checks whether a var kind represents reassignable or not.
     * @param kind Kind to check.
     * @return `true` if represents reassignable, `false` otherwise.
     */
    static bool isReassignable(VarModifier::VarKind kind) noexcept;

    /**
     * Checks whether a `VarModifier` node represents reassignable or not.
     * @param varmod `VarModifier` node to check.
     * @return `true` if represents reassignable, `false` otherwise.
     */
    static bool isReassignable(const VarModifier* varmod) noexcept;

    /**
     * Checks whether an AST node is a hoisted declarative node.
     * @param node Node to check.
     * @return `true` if the node is a hoisted declarative node, `false` otherwise.
     */
    static bool isHoistedDeclarative(const Node* node) noexcept;

    /**
     * Adds a "cannot find name" name resolution error to the diagnostics.
     * @param start Start location of the error.
     * @param end End location of the error.
     * @param name Name that couldn't be found.
     */
    void cannotFindError(Location start, Location end, std::string_view name);
};



/**
 * Represents a visitor that creates symbol(s) for a name or a pattern and binds them to the given environment.
 */
class NameDeclarator : public NodeVisitor {
private:
    SymbolTable& _symbolTable;
    NodeSymbolMap& _nodeSymbolMap;

    Env& _env;

    SymbolKind _kind;
    bool _isReassignable;

    Diagnostics& _diagnostics;

public:
    NameDeclarator(SymbolTable& symbolTable,
                   NodeSymbolMap& nodeSymbolMap,
                   Env& env,
                   SymbolKind kind,
                   bool isReassignable,
                   Diagnostics& diagnostics) noexcept
        : _symbolTable(symbolTable), _nodeSymbolMap(nodeSymbolMap), _env(env), _kind(kind),
          _isReassignable(isReassignable), _diagnostics(diagnostics) { }

    void visit(Name* node) override;
    void visit(BindingPattern* pattern) override;
    void visit(TuplePattern* pattern) override;
    void visit(CollectionPattern* pattern) override;
    void visit(RecordPattern* pattern) override;

private:
    void redeclareError(Location start,
                        Location end,
                        std::string_view name,
                        Location prevStart,
                        Location prevEnd) noexcept;
};



/**
 * Represents an AST node visitor that resolves an expression to its declared type.
 */
class DeclaredTypeResolver : public NodeVisitor {
private:
    TypeTable& _typeTable;
    NodeTypeMap& _nodeTypeMap;

    const Env& _env;

    SemanticType* _result = nullptr;

public:
    DeclaredTypeResolver(TypeTable& typeTable, NodeTypeMap& nodeTypeMap, const Env& env) noexcept
        : _typeTable(typeTable), _nodeTypeMap(nodeTypeMap), _env(env) { }

    [[nodiscard]]
    SemanticType* result() const noexcept { return _result; }

    void visit(NameExpr* ident) override;
    void visit(BinaryExpr* binary) override;
};

} // Spark::FrontEnd
