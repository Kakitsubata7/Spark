#pragma once

#include <vector>

#include "semantic_type.hpp"

namespace Spark::FrontEnd {

class SemanticFunc {
private:
    std::vector<SemanticType*> _paramTypes;
    std::vector<SemanticType*> _returnTypes;

public:
    SemanticFunc(std::vector<SemanticType*> paramTypes, std::vector<SemanticType*> returnTypes) noexcept
        : _paramTypes(std::move(paramTypes)), _returnTypes(std::move(returnTypes)) { }

    [[nodiscard]]
    bool isCallableWith(const std::vector<SemanticType*>& paramTypes,
                        const std::vector<SemanticType*>& returnTypes) const noexcept;
};

} // Spark::FrontEnd
