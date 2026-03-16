#include "semantic_type.hpp"

namespace Spark::FrontEnd {

SemanticType* SemanticTypeTable::make(SemanticType type) {
    return _types.emplace_back(std::make_unique<SemanticType>(std::move(type))).get();
}

} // Spark::FrontEnd
