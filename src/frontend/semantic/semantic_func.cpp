#include "semantic_func.hpp"

#include "semantic_type.hpp"

namespace Spark::FrontEnd {

bool SemanticFunc::isCallableWith(const std::vector<SemanticType*>& paramTypes,
                                  const std::vector<SemanticType*>& returnTypes) const noexcept {
    return false; // TODO
}

SemanticFunc* FuncTable::make(SemanticFunc func) {
    return _funcs.emplace_back(std::make_unique<SemanticFunc>(std::move(func))).get();
}
} // Spark::FrontEnd
