#include "semantic_type.hpp"

#include <algorithm>

#include <boost/container_hash/hash.hpp>

#include "semantic_func.hpp"

namespace Spark::FrontEnd {

SemanticFunc* MonoFuncType::getFunc(const std::vector<SemanticType*>& paramTypes) const noexcept {
    return _func->isCallableWith(paramTypes) ? _func : nullptr;
}

SemanticFunc* OverloadedFuncType::getFunc(const std::vector<SemanticType*>& paramTypes) const noexcept {
    for (MonoFuncType* mf : _funcTypes) {
        if (mf->func()->isCallableWith(paramTypes)) {
            return mf->func();
        }
    }
    return nullptr;
}

FuncType* TypeMethod::type() const noexcept {
    assert(dynamic_cast<FuncType*>(_symbol->type) != nullptr);
    return static_cast<FuncType*>(_symbol->type);
}

OverloadedFuncType::OverloadedFuncType(TypeId id, std::string name, const std::vector<MonoFuncType*>& funcTypes)
    : FuncType(id, std::move(name)), _funcTypes(funcTypes) {
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
    MonoFuncType* t = makeType<MonoFuncType>(getNextId(), std::move(name), func);
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
    OverloadedFuncType* t = makeType<OverloadedFuncType>(getNextId(), std::move(name), std::move(canonizedFuncs));
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

    return makeType<TypeType>(id, std::move(name), declaredType);
}

TraitType* TypeTable::makeTraitType(std::string name,
                                    std::vector<TraitType*> traits,
                                    std::vector<TypeMethod> methods) {
    return makeType<TraitType>(getNextId(), std::move(name), std::move(traits), std::move(methods));
}

StructType* TypeTable::makeStructType(std::string name,
                                      std::vector<TraitType*> traits,
                                      std::vector<RecordField> fields,
                                      std::vector<TypeMethod> methods) {
    return makeType<StructType>(getNextId(), std::move(name), std::move(traits), std::move(fields),
                                std::move(methods));
}

ClassType* TypeTable::makeClassType(std::string name,
                                    ClassType* base,
                                    std::vector<TraitType*> traits,
                                    std::vector<RecordField> fields,
                                    std::vector<TypeMethod> methods) {
    return makeType<ClassType>(getNextId(), std::move(name), base, std::move(traits),
                               std::move(fields), std::move(methods));
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
