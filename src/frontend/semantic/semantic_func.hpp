#pragma once

#include <memory>
#include <vector>

#include "symbol.hpp"

namespace Spark::FrontEnd {

class SemanticType;

/**
 * Represents a function during semantic passes.
 */
class SemanticFunc {
private:
    std::vector<SemanticType*> _paramTypes;
    std::vector<SemanticType*> _returnTypes;

    std::vector<Symbol*> _captures;

public:
    SemanticFunc(std::vector<SemanticType*> paramTypes,
                 std::vector<SemanticType*> returnTypes,
                 std::vector<Symbol*> captures) noexcept
        : _paramTypes(std::move(paramTypes)), _returnTypes(std::move(returnTypes)), _captures(std::move(captures)) { }

    /**
     * Checks whether the function is callable with the given parameter and return types.
     * @param paramTypes Parameter types (in order).
     * @param returnTypes Return types (in order).
     * @return `true` if the function is callable with the given parameter and return types, `false` otherwise.
     */
    [[nodiscard]]
    bool isCallableWith(const std::vector<SemanticType*>& paramTypes,
                        const std::vector<SemanticType*>& returnTypes) const noexcept;
};

/**
 * Represents a data structure that manages `SemanticFunc` instances and provide factory methods for creating
 * `SemanticFunc` instances.
 */
class FuncTable {
private:
    std::vector<std::unique_ptr<SemanticFunc>> _funcs;

public:
    SemanticFunc* make(SemanticFunc func);
};

} // Spark::FrontEnd
