#include "semantic_func.hpp"

namespace Spark::FrontEnd {

bool SemanticFunc::isCallableWith(const std::vector<SemanticType*>& paramTypes,
                                  const std::vector<SemanticType*>& returnTypes) const noexcept {
    return false; // TODO
}

} // Spark::FrontEnd
