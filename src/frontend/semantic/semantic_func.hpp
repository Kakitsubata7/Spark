#pragma once

#include <vector>

#include "symbol.hpp"

namespace Spark::FrontEnd {

class SemanticType;

/**
 * Represents a semantic function's signature
 */
class FuncSignature {
private:
    std::vector<SemanticType*> _paramTypes;
    SemanticType* _returnType;

public:
    FuncSignature(std::vector<SemanticType*> paramTypes, SemanticType* returnType) noexcept
        : _paramTypes(std::move(paramTypes)), _returnType(returnType) { }

    [[nodiscard]]
    const std::vector<SemanticType*>& paramTypes() const noexcept { return _paramTypes; }

    [[nodiscard]]
    SemanticType* returnType() const noexcept { return _returnType; }

    bool operator==(const FuncSignature& other) const noexcept;
    bool operator!=(const FuncSignature& other) const noexcept;
};

/**
 * Represents the hasher for `FuncSignature`.
 */
struct FuncSignatureHash {
    size_t operator()(const FuncSignature& sig) const;
};

/**
 * Represents a function during semantic passes.
 */
class SemanticFunc {
private:
    FuncSignature _sig;

public:
    explicit SemanticFunc(FuncSignature sig) noexcept : _sig(std::move(sig)) { }

    SemanticFunc(std::vector<SemanticType*> paramTypes, SemanticType* returnType) noexcept
        : _sig(std::move(paramTypes), returnType) { }

    [[nodiscard]]
    const FuncSignature& sig() const noexcept { return _sig; }

    [[nodiscard]]
    const std::vector<SemanticType*>& paramTypes() const noexcept { return _sig.paramTypes(); }

    [[nodiscard]]
    SemanticType* returnType() const noexcept { return _sig.returnType(); }

    [[nodiscard]]
    bool isCallableWith(const std::vector<SemanticType*>& paramTypes) const noexcept;
};

/**
 * Represents a factory that manages `SemanticFunc` instances.
 */
class FuncTable {
private:
    std::vector<std::unique_ptr<SemanticFunc>> _funcs;

public:
    SemanticFunc* make(FuncSignature sig) {
        return _funcs.emplace_back(std::make_unique<SemanticFunc>(std::move(sig))).get();
    }

    SemanticFunc* make(std::vector<SemanticType*> paramTypes, SemanticType* returnType) {
        return _funcs.emplace_back(std::make_unique<SemanticFunc>(std::move(paramTypes), returnType)).get();
    }
};

} // Spark::FrontEnd

template <>
struct std::hash<Spark::FrontEnd::FuncSignature> {
    std::size_t operator()(const Spark::FrontEnd::FuncSignature& sig) const noexcept {
        return Spark::FrontEnd::FuncSignatureHash{}(sig);
    }
};
