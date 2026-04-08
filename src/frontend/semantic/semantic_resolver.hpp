#pragma once

#include <optional>
#include <string_view>

#include "env.hpp"
#include "frontend/ast.hpp"
#include "semantic_func.hpp"
#include "semantic_type.hpp"
#include "symbol.hpp"
#include "utils/diagnostic.hpp"

namespace Spark::FrontEnd {

class SemanticResolver : public NodeVisitor {
private:
    SymbolTable _symbolTable;
    FuncTable _funcTable;
    TypeTable _typeTable;

    Env _globalEnv;
    std::vector<Env> _envStack;

    Diagnostics& _diagnostics;

    bool _inLoop = false;
    bool _inFunc = false;

    SemanticType* _resultType = nullptr;

    SemanticType* _unknownType;
    StructType* _voidType;
    StructType* _intType;
    StructType* _realType;
    StructType* _boolType;
    ClassType* _stringType;
    StructType* _nilType;
    TypeType* _typeType;

    [[nodiscard]]
    SemanticType* unknownType() const noexcept { return _unknownType; }

    [[nodiscard]]
    StructType* voidType() const noexcept { return _voidType; }

    [[nodiscard]]
    StructType* intType() const noexcept { return _intType; }

    [[nodiscard]]
    StructType* realType() const noexcept { return _realType; }

    [[nodiscard]]
    StructType* boolType() const noexcept { return _boolType; }

    [[nodiscard]]
    ClassType* stringType() const noexcept { return _stringType; }

    [[nodiscard]]
    StructType* nilType() const noexcept { return _nilType; }

    [[nodiscard]]
    TypeType* typeType() const noexcept { return _typeType; }

    // Functions
    std::unordered_map<FnDefStmt*, SemanticFunc*> _fndefMap;
    SemanticType* _currentReturnType = nullptr;

    // Types
    std::unordered_map<TypeDefStmt*, TypeType*> _tdefMap;

public:
    explicit SemanticResolver(Diagnostics& diagnostics);

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

private:
    SemanticType* resolve(Node* node);

    std::vector<SemanticType*> resolveNodes(const std::vector<Node*>& nodes);

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
     * @param name Name to declare.
     * @param env Environment to declare the name in.
     * @param kind Kind of the declared symbol.
     * @param isReassignable Reassignability.
     * @param type Type of the symbol
     * @param start Start location of the name.
     * @param end End location of the name.
     */
    void declare(std::string_view name,
                 Env& env,
                 SymbolKind kind,
                 bool isReassignable,
                 SemanticType* type,
                 Location start,
                 Location end);

    /**
     * Declares function definitions.
     * @param fndefs Function definition statements.
     * @param env Environment to declare into.
     */
    void declareFunctions(const std::vector<FnDefStmt*>& fndefs, Env& env);

    /**
     * Declares type definitions.
     * @param tdefs Type definition statements.
     * @param env Environment to declare into.
     */
    void declareTypes(const std::vector<TypeDefStmt*>& tdefs, Env& env);

    /**
     * Finds the `SemanticFunc` pointer based on symbol name and parameter types starting from the given environment.
     * @param name Symbol name to look for.
     * @param currentEnv Environment to start finding.
     * @param paramTypes Parameter types
     * @return `SemanticFunc` pointer or `nullptr` if not found.
     */
    static SemanticFunc* findFunc(std::string_view name,
                                  const Env& currentEnv,
                                  const std::vector<SemanticType*>& paramTypes);

    /**
     * Finds the `SemanticFunc` pointer of a type's method based on its name.
     * @param name Method name to look for.
     * @param type Type to look the method for.
     * @param paramTypes Parameter types.
     * @return `SemanticFunc` pointer or `nullptr` if not found.
     */
    static SemanticFunc* findFunc(std::string_view name,
                                  const SemanticType* type,
                                  const std::vector<SemanticType*>& paramTypes);

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
     * Constructs a `std::string` for the name of a `MonoFuncType` instance.
     * @param paramTypes Parameter types.
     * @param returnType Return type.
     * @return Constructed name.
     */
    static std::string createMonoFuncTypeName(const std::vector<SemanticType*>& paramTypes,
                                              const SemanticType* returnType);

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
     * @param type Type.
     */
    void cannotFindOperatorError(Location start,
                                 Location end,
                                 std::string_view opName,
                                 const SemanticType* type);

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

    /**
     * Adds a "cannot find operator" operator resolution error to the diagnostics.
     * @param start Start location of the error.
     * @param end End location of the error.
     * @param opName Operator name that couldn't be found.
     * @param types Types.
     */
    void cannotFindOperatorError(Location start,
                                 Location end,
                                 std::string_view opName,
                                 const std::vector<SemanticType*>& types);

    void redeclareError(Location start,
                        Location end,
                        std::string_view name,
                        Location prevStart,
                        Location prevEnd);

    void unexpectedTypeError(Location start, Location end, const SemanticType* expected, const SemanticType* actual);

    void notCallableError(Location start,
                          Location end,
                          const SemanticType* type,
                          const std::vector<SemanticType*>& paramTypes);

    void notIndexableError(Location start,
                           Location end,
                           const SemanticType* type,
                           const std::vector<SemanticType*>& paramTypes);

    void invalidStrictEqError(Location start, Location end, const SemanticType* lhsType, const SemanticType* rhsType);

    void invalidStrictNeError(Location start, Location end, const SemanticType* lhsType, const SemanticType* rhsType);

    void invalidStructInheritanceError(Location start, Location end, const SemanticType* type);

    void typeHasNoMemberError(Location start, Location end, const SemanticType* type, std::string_view member);

    void redeclareOfFuncWithTheSameSigError(Location start,
                                            Location end,
                                            std::string_view name,
                                            const std::vector<SemanticType*>& paramTypes);

    void notReassignableError(Location start, Location end, std::string_view name);

    void exprNotReassignableError(Location start, Location end);
};

} // Spark::FrontEnd
