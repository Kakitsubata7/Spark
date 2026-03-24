#pragma once

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

public:
    SemanticFunc(std::vector<SemanticType*> paramTypes,
                 std::vector<SemanticType*> returnTypes) noexcept
        : _paramTypes(std::move(paramTypes)), _returnTypes(std::move(returnTypes)) { }

    [[nodiscard]]
    const std::vector<SemanticType*>& paramTypes() const noexcept { return _paramTypes; }

    [[nodiscard]]
    const std::vector<SemanticType*>& returnTypes() const noexcept { return _returnTypes; }
};

} // Spark::FrontEnd
