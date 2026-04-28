#include "semantic_func.hpp"

#include <boost/functional/hash.hpp>

#include "semantic_type.hpp"

namespace Spark::FrontEnd {

bool FuncSignature::operator==(const FuncSignature& other) const noexcept {
    if (this == &other) {
        return true;
    }

    // Check parameter types
    if (_paramTypes.size() != other._paramTypes.size()) {
        return false;
    }
    for (size_t i = 0; i < _paramTypes.size(); i++) {
        if (!_paramTypes[i]->isIdentical(other._paramTypes[i])) {
            return false;
        }
    }

    // Check return type
    return _returnType->isIdentical(other._returnType);
}

bool FuncSignature::operator!=(const FuncSignature& other) const noexcept {
    return !(*this == other);
}

size_t FuncSignatureHash::operator()(const FuncSignature& sig) const {
    size_t seed = 0;
    for (auto* t : sig.paramTypes()) {
        boost::hash_combine(seed, t->id());
    }
    boost::hash_combine(seed, sig.returnType()->id());
    return seed;
}

bool SemanticFunc::isCallableWith(const std::vector<SemanticType*>& paramTypes) const noexcept {
    if (_sig.paramTypes().size() != paramTypes.size()) {
        return false;
    }
    for (size_t i = 0; i < paramTypes.size(); ++i) {
        SemanticType* expected = _sig.paramTypes()[i];
        SemanticType* actual = paramTypes[i];
        if (!expected->isIdentical(actual)) {
            return false;
        }
    }
    return true;
}

} // Spark::FrontEnd
