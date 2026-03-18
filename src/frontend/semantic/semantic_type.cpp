#include "semantic_type.hpp"

namespace Spark::FrontEnd {

SemanticType* TypeTable::make(SemanticType type) {
    return _types.emplace_back(std::make_unique<SemanticType>(std::move(type))).get();
}

SemanticType* TypeTable::makeStructType(std::vector<SemanticType*> traits) {

}

SemanticType* TypeTable::makeTupleType(const std::vector<SemanticType*>& types) {

}

SemanticType* TypeTable::makeUnionType(const std::vector<SemanticType*>& types) {

}

} // Spark::FrontEnd
