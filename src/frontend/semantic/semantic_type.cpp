#include "semantic_type.hpp"

#include <algorithm>

#include <boost/container_hash/hash.hpp>

#include "semantic_func.hpp"

namespace Spark::FrontEnd {

OverloadedFuncType::OverloadedFuncType(std::string name, TypeId id, const std::vector<MonoFuncType*>& funcTypes)
    : SemanticType(std::move(name), id), _funcTypes(funcTypes) {
    canonizeTypeVec(_funcTypes);
}

TypeMethod::TypeMethod(Symbol* symbol) noexcept : _symbol(symbol) {
    assert(symbol != nullptr);
    assert(symbol->type->is<FuncType>());
}

bool TypeTable::OverloadedFuncKey::operator==(const OverloadedFuncKey& other) const noexcept {
    if (_funcTypes.size() != other._funcTypes.size()) {
        return false;
    }
    for (size_t i = 0; i < _funcTypes.size(); ++i) {
        if (!_funcTypes[i]->isIdentical(other._funcTypes[i])) {
            return false;
        }
    }
    return true;
}

size_t TypeTable::OverloadedFuncKeyHash::operator()(const OverloadedFuncKey& key) const {
    size_t seed = 0;
    for (auto* t : key.funcTypes()) {
        boost::hash_combine(seed, t->id());
    }
    return seed;
}

MonoFuncType* TypeTable::makeMonoFuncType(std::string name, SemanticFunc* func) {
    // Check for existing function type with the same signature
    if (auto it = _funcTypeMap.find(func->sig()); it != _funcTypeMap.end()) {
        return it->second;
    }

    // Create and store the function type
    MonoFuncType* t = makeType<MonoFuncType>(std::move(name), getNextId(), func);
    _funcTypeMap.insert({func->sig(), t});
    return t;
}

OverloadedFuncType* TypeTable::makeOverloadedFuncType(std::string name, const std::vector<MonoFuncType*>& funcTypes) {
    // Canonize function types vector
    std::vector<MonoFuncType*> canonizedFuncs = funcTypes; // Copies
    SemanticType::canonizeTypeVec(canonizedFuncs);

    // Check for existing overloaded function type with the same set of function types
    OverloadedFuncKey key{canonizedFuncs}; // Copies
    auto [it, inserted] =
        _overloadedFuncTypeMap.emplace(std::move(key), nullptr);
    if (!inserted) {
        return it->second;
    }

    // Create and store the overloaded function type
    OverloadedFuncType* t = makeType<OverloadedFuncType>(std::move(name), getNextId(), std::move(canonizedFuncs));
    it->second = t;
    return t;
}

TypeType* TypeTable::makeTypeType(std::string name, SemanticType* declaredType) {
    // Get `Type` type ID
    TypeId id;
    if (_hasTypeType) {
        id = _typeTypeId;
    } else {
        id = getNextId();
        _hasTypeType = true;
    }

    return makeType<TypeType>(std::move(name), id, declaredType);
}

TraitType* TypeTable::makeTraitType(std::string name,
                                    std::vector<TraitType*> traits,
                                    std::vector<TypeMethod> methods) {
    return makeType<TraitType>(std::move(name), getNextId(), std::move(traits), std::move(methods));
}

StructType* TypeTable::makeStructType(std::string name,
                                      std::vector<TraitType*> traits,
                                      std::vector<RecordField> fields,
                                      std::vector<TypeMethod> methods) {
    return makeType<StructType>(std::move(name), getNextId(), std::move(traits), std::move(fields), std::move(methods));
}

ClassType* TypeTable::makeClassType(std::string name,
                                    ClassType* base,
                                    std::vector<TraitType*> traits,
                                    std::vector<RecordField> fields,
                                    std::vector<TypeMethod> methods) {
    return makeType<ClassType>(std::move(name), getNextId(), base, std::move(traits), std::move(fields),
                               std::move(methods));
}

TypeId TypeTable::getNextId() noexcept {
    TypeId id = _nextId;
    ++_nextId;
    if (_nextId < id) {
        assert(false && "`TypeTable` runs out of type ID range");
    }
    return id;
}

} // Spark::FrontEnd
