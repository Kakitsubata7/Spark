#pragma once

#include <optional>
#include <utility>
#include <vector>

#include "env.hpp"
#include "frontend/ast.hpp"
#include "symbol_table.hpp"
#include "utils/diagnostic.hpp"

namespace Spark::FrontEnd {

/**
 * Represents the result of name resolution.
 */
struct NameResolveResult {
    std::vector<Diagnostic> diagnostics;

    NameResolveResult() = default;
    explicit NameResolveResult(std::vector<Diagnostic> diagnostics) noexcept
        : diagnostics(std::move(diagnostics)) { }
};

/**
 * Represents lexical symbol resolution.
 */
class NameResolver {
private:
    /**
     * Represents a visitor that resolves all symbols.
     */
    class ResolveVisitor : public NodeVisitor {
    private:
        SymbolTable& _symTable;

        std::vector<Env> _envStack;

        const std::optional<std::string>& _filename;

    public:
        NameResolveResult result;

        explicit ResolveVisitor(SymbolTable& symTable,
                                const std::optional<std::string>& filename = std::nullopt) noexcept
            : _symTable(symTable), _filename(filename) { }

        void visit(Node& node) override;
        void visit(Name& name) override;
        void visit(VarDefStmt& vardef) override;
        void visit(FnDefStmt& fndef) override;
        void visit(TypeDefStmt& tdef) override;
        void visit(BlockExpr& block) override;

    private:
        void pushEnv() { _envStack.emplace_back(); }
        void popEnv() { _envStack.pop_back(); }
        Env& currentEnv() { return _envStack.back(); }

        Symbol* lookup(InternedNameValue name) const noexcept;

        void diagnostic(Diagnostic diagnostic) noexcept {
            result.diagnostics.push_back(std::move(diagnostic));
        }

        static bool isHoisted(const Node* node) noexcept {
            return node->is<FnDefStmt>() || node->is<TypeDefStmt>() || node->is<ModuleStmt>();
        }

        static bool isReassignable(const VarModifier* mod) noexcept {
            VarModifier::VarKind kind = mod->kind;
            return kind == VarModifier::VarKind::Let || kind == VarModifier::VarKind::Ref;
        }

        static bool isReference(const VarModifier* mod) noexcept {
            VarModifier::VarKind kind = mod->kind;
            return kind == VarModifier::VarKind::Ref || kind == VarModifier::VarKind::Cref;
        }
    };

    /**
     * Represents a visitor that traverses a pattern and binds symbols to an environment.
     */
    class PatternBinder : public NodeVisitor {
    private:
        Env& _env;
        SymbolTable& _symTable;

        bool _isReassignable;
        bool _isReference;

        const std::optional<std::string>& _filename;

    public:
        std::vector<Diagnostic> diagnostics;

        PatternBinder(Env& env,
                      SymbolTable& symTable,
                      bool isReassignable,
                      bool isReference,
                      const std::optional<std::string>& filename = std::nullopt) noexcept
            : _env(env), _symTable(symTable), _isReassignable(isReassignable), _isReference(isReference),
              _filename(filename) { }

        void visit(BindingPattern& p) override;
        void visit(TuplePattern& p) override;
        void visit(CollectionPattern& p) override;
        void visit(RecordPattern& p) override;

        void diagnostic(Diagnostic diagnostic) noexcept {
            diagnostics.push_back(std::move(diagnostic));
        }
    };

public:
    static NameResolveResult resolve(const AST& ast, SymbolTable& symTable);
};

} // Spark::FrontEnd
