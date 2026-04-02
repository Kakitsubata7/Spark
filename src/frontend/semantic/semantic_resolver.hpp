#pragma once

#include <optional>
#include <string_view>

#include "env.hpp"
#include "frontend/ast.hpp"
#include "semantic_type.hpp"
#include "symbol.hpp"
#include "utils/diagnostic.hpp"

namespace Spark::FrontEnd {

class SemanticResolver : public NodeVisitor {
private:
    SymbolTable _symbolTable;
    TypeTable _typeTable;

    Env _globalEnv;
    std::vector<Env> _envStack;

    Diagnostics& _diagnostics;

    SemanticType* _currentType = nullptr;

    SemanticType* _unknownType;
    SemanticType* _voidType;
    SemanticType* _intType;
    SemanticType* _realType;
    SemanticType* _boolType;
    SemanticType* _stringType;
    SemanticType* _nilType;

    [[nodiscard]]
    SemanticType* unknownType() const noexcept { return _unknownType; }

    [[nodiscard]]
    SemanticType* voidType() const noexcept { return _voidType; }

    [[nodiscard]]
    SemanticType* intType() const noexcept { return _intType; }

    [[nodiscard]]
    SemanticType* realType() const noexcept { return _realType; }

    [[nodiscard]]
    SemanticType* boolType() const noexcept { return _boolType; }

    [[nodiscard]]
    SemanticType* stringType() const noexcept { return _stringType; }

    [[nodiscard]]
    SemanticType* nilType() const noexcept { return _nilType; }

public:
    explicit SemanticResolver(Diagnostics& diagnostics);

    void visit(FnParam* param) override;

    void visit(LambdaExpr* lambda) override;
    void visit(IfThenExpr* ifthen) override;
    void visit(TryElseExpr* tryelse) override;
    void visit(MatchExpr* match) override;
    void visit(TryCatchExpr* trycatch) override;
    void visit(ThrowExpr* t) override;
    void visit(BlockExpr* block) override;
    void visit(IsExpr* is) override;
    void visit(AsExpr* as) override;
    void visit(BinaryExpr* binary) override;
    void visit(PrefixExpr* prefix) override;
    void visit(PostfixExpr* postfix) override;
    void visit(MemberAccessExpr* maccess) override;
    void visit(CallExpr* call) override;
    void visit(SubscriptExpr* subscript) override;
    void visit(LiteralExpr* literal) override;
    void visit(NameExpr* ident) override;
    void visit(GlobalAccessExpr* gaccess) override;
    void visit(UpvalueExpr* upvalue) override;
    void visit(TupleExpr* tuple) override;
    void visit(CollectionExpr* collection) override;
    void visit(TypeofExpr* t) override;

    void visit(VarDefStmt* vardef) override;
    void visit(FnDefStmt* fndef) override;
    void visit(TypeDefStmt* tdef) override;
    void visit(AssignStmt* assign) override;
    void visit(IfStmt* ifstmt) override;
    void visit(WhileStmt* w) override;
    void visit(DoWhileStmt* dw) override;
    void visit(ForStmt* f) override;
    void visit(BreakStmt* b) override;
    void visit(ContinueStmt* c) override;
    void visit(ReturnStmt* ret) override;
    void visit(ModuleStmt* moddef) override;
    void visit(ExportStmt* e) override;
    void visit(ImportStmt* i) override;
    void visit(ImportAllStmt* i) override;
    void visit(UndefineStmt* undef) override;

    SemanticType* registerType();

private:
    SemanticType* resolve(Node* node);

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
     * Gets the special identifier string the overloadable binary operator corresponds to.
     * @param op Binary operator kind.
     * @return Identifier string the binary operator corresponds to or `std::nullopt`.
     */
    static std::optional<std::string_view> binaryOpToIdent(BinaryExpr::OpKind op) noexcept;

    /**
     * Gets the special identifier string the overloadable prefix operator corresponds to.
     * @param op Prefix operator kind.
     * @return Identifier string the prefix operator corresponds to or `std::nullopt`.
     */
    static std::optional<std::string_view> prefixOpToIdent(PrefixExpr::OpKind op) noexcept;

    /**
     * Adds an error to the diagnostics.
     * @param start Start location of the error.
     * @param end End of location of the error.
     * @param message Error message.
     * @param subs Sub diagnostics of the error (default to none).
     */
    void error(Location start, Location end, std::string message, std::vector<Diagnostic> subs = {});

    /**
     * Adds a "cannot find name" name resolution error to the diagnostics.
     * @param start Start location of the error.
     * @param end End location of the error.
     * @param name Name that couldn't be found.
     */
    void cannotFindError(Location start, Location end, std::string_view name);

    /**
     * Adds a "cannot find operator" operator resolution error to the diagnostics.
     * @param start Start location of the error.
     * @param end End location of the error.
     * @param opName Operator name that couldn't be found.
     * @param lhsType LHS type.
     * @param rhsType RHS type.
     */
    void cannotFindOperatorError(Location start,
                                 Location end,
                                 std::string_view opName,
                                 const SemanticType* lhsType,
                                 const SemanticType* rhsType);

    void unexpectedTypeError(Location start, Location end, const SemanticType* expected, const SemanticType* actual);

    void notCallableError(Location start,
                          Location end,
                          const SemanticType* type,
                          const std::vector<SemanticType*>& paramTypes);

    void invalidStrictEqError(Location start, Location end, const SemanticType* lhsType, const SemanticType* rhsType);

    void invalidStrictNeError(Location start, Location end, const SemanticType* lhsType, const SemanticType* rhsType);
};



/**
 * Represents a visitor that creates symbol(s) for a name or a pattern and binds them to the given environment.
 */
class NameDeclarator : public NodeVisitor {
private:
    SymbolTable& _symbolTable;

    Env& _env;

    SymbolKind _kind;
    bool _isReassignable;

    Diagnostics& _diagnostics;

public:
    NameDeclarator(SymbolTable& symbolTable,
                   Env& env,
                   SymbolKind kind,
                   bool isReassignable,
                   Diagnostics& diagnostics) noexcept
        : _symbolTable(symbolTable), _env(env), _kind(kind), _isReassignable(isReassignable),
          _diagnostics(diagnostics) { }

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

} // Spark::FrontEnd
