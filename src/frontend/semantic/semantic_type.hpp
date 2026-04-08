#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include "symbol.hpp"

namespace Spark::FrontEnd {

class SemanticFunc;

using TypeId = uint64_t;

/**
 * Represents a type during semantic passes.
 */
class SemanticType {
private:
    TypeId _id;

    std::string _name;
    Location _start;
    Location _end;

protected:
    explicit SemanticType(std::string name, TypeId id) noexcept
        : _name(std::move(name)), _id(id) { }

public:
    virtual ~SemanticType() = default;

    [[nodiscard]]
    TypeId id() const noexcept { return _id; }

    [[nodiscard]]
    std::string_view name() const noexcept { return _name; }

    [[nodiscard]]
    Location start() const noexcept { return _start; }

    [[nodiscard]]
    Location end() const noexcept { return _end; }

    /**
     * Checks whether two `SemanticType` instances describe the same type.
     * @param other Other type to check.
     * @return `true` if the two instances describe the same type, `false` otherwise.
     */
    [[nodiscard]]
    bool isIdentical(const SemanticType& other) const noexcept {
        return _id == other._id;
    }

    /**
     * Checks whether two `SemanticType` instances describe the same type.
     * @param other Pointer to the other type to check.
     * @return `true` if the two instances describe the same type, `false` otherwise.
     */
    [[nodiscard]]
    bool isIdentical(const SemanticType* other) const noexcept {
        assert(other != nullptr);
        return isIdentical(*other);
    }

    template <typename T>
    [[nodiscard]]
    bool is() const {
        return dynamic_cast<const T*>(this) != nullptr;
    }

    template <typename T>
    [[nodiscard]]
    T* as() {
        return dynamic_cast<T*>(this);
    }

    template <typename T>
    [[nodiscard]]
    const T* as() const {
        return dynamic_cast<const T*>(this);
    }

    /**
     * Sorts and de-duplicates types based on their IDs.
     * @tparam T `SemanticType` or its subtype.
     * @param vec Vector of types to canonize.
     */
    template <typename T>
    static void canonizeTypeVec(std::vector<T*>& vec) {
        static_assert(std::is_base_of_v<SemanticType, T>);

        std::sort(vec.begin(), vec.end(),
            [](const T* a, const T* b) {
                return a->id() < b->id();
            }
        );

        vec.erase(std::unique(vec.begin(), vec.end(),
            [](const T* a, const T* b) {
                return a->isIdentical(b);
            }),
            vec.end()
        );
    }
};

/**
 * Represents a semantic function type.
 */
class FuncType : public SemanticType {
protected:
    explicit FuncType(std::string name, TypeId id) noexcept
        : SemanticType(std::move(name), id) { }

public:
    [[nodiscard]]
    virtual SemanticFunc* getFunc(const std::vector<SemanticType*>& paramTypes) const noexcept = 0;
};

/**
 * Represents a semantic function type without overloads.
 */
class MonoFuncType final : public FuncType {
private:
    SemanticFunc* _func;

public:
    MonoFuncType(std::string name, TypeId id, SemanticFunc* func) noexcept
        : FuncType(std::move(name), id), _func(func) { }

    [[nodiscard]]
    SemanticFunc* func() const noexcept { return _func; }

    [[nodiscard]]
    SemanticFunc* getFunc(const std::vector<SemanticType*>& paramTypes) const noexcept override;
};

/**
 * Represents a semantic function type with overloads.
 */
class OverloadedFuncType final : public FuncType {
private:
    /**
     * Invariant: types are sorted.
     */
    std::vector<MonoFuncType*> _funcTypes;

public:
    OverloadedFuncType(std::string name, TypeId id, std::vector<MonoFuncType*>&& funcTypes) noexcept
        : FuncType(std::move(name), id), _funcTypes(std::move(funcTypes)) { }

    OverloadedFuncType(std::string name, TypeId id, const std::vector<MonoFuncType*>& funcTypes);

    [[nodiscard]]
    const std::vector<MonoFuncType*>& funcTypes() const noexcept { return _funcTypes; }

    [[nodiscard]]
    SemanticFunc* getFunc(const std::vector<SemanticType*>& paramTypes) const noexcept override;
};

/**
 * Represents `Type`'s semantic type.
 */
class TypeType final : public SemanticType {
private:
    SemanticType* _declaredType;

public:
    TypeType(std::string name, TypeId id, SemanticType* declaredType) noexcept
        : SemanticType(std::move(name), id), _declaredType(declaredType) { }

    [[nodiscard]]
    SemanticType* declaredType() const noexcept { return _declaredType; }
};

/**
 * Represents the method of a semantic type.
 */
class TypeMethod {
private:
    Symbol* _symbol;

public:
    explicit TypeMethod(Symbol* symbol) noexcept;

    [[nodiscard]]
    std::string_view name() const noexcept { return _symbol->name; }

    [[nodiscard]]
    FuncType* type() const noexcept {
        assert(dynamic_cast<FuncType*>(_symbol->type) != nullptr);
        return static_cast<FuncType*>(_symbol->type);
    }
};

/**
 * Represents a semantic trait type.
 */
class TraitType : public SemanticType {
private:
    std::vector<TraitType*> _traits;

    std::vector<TypeMethod> _methods;

public:
    explicit TraitType(std::string name,
                       TypeId id,
                       std::vector<TraitType*> traits = {},
                       std::vector<TypeMethod> methods = {}) noexcept
        : SemanticType(std::move(name), id), _traits(std::move(traits)), _methods(std::move(methods)) { }

    [[nodiscard]]
    const std::vector<TraitType*>& traits() const noexcept { return _traits; }

    [[nodiscard]]
    const std::vector<TypeMethod>& methods() const noexcept { return _methods; }
};

/**
 * Represents the field of a semantic record type.
 */
class RecordField {
private:
    Symbol* _symbol;

public:
    explicit RecordField(Symbol* symbol) noexcept : _symbol(symbol) { }

    [[nodiscard]]
    std::string_view name() const noexcept { return _symbol->name; }

    [[nodiscard]]
    SemanticType* type() const noexcept { return _symbol->type; }
};

/**
 * Represents a semantic type with fields and methods.
 */
class RecordType : public SemanticType {
protected:
    std::vector<TraitType*> _traits;

    std::vector<RecordField> _fields;
    std::vector<TypeMethod> _methods;

    explicit RecordType(std::string name,
                        TypeId id,
                        std::vector<TraitType*> traits = {},
                        std::vector<RecordField> fields = {},
                        std::vector<TypeMethod> methods = {}) noexcept
        : SemanticType(std::move(name), id), _traits(std::move(traits)), _fields(std::move(fields)),
          _methods(std::move(methods)) { }

public:
    [[nodiscard]]
    const std::vector<TraitType*>& traits() const noexcept { return _traits; }

    [[nodiscard]]
    const std::vector<RecordField>& fields() const noexcept { return _fields; }

    [[nodiscard]]
    const std::vector<TypeMethod>& methods() const noexcept { return _methods; }
};

/**
 * Represents a semantic struct type.
 */
class StructType final : public RecordType {
public:
    explicit StructType(std::string name,
                        TypeId id,
                        std::vector<TraitType*> traits = {},
                        std::vector<RecordField> fields = {},
                        std::vector<TypeMethod> methods = {}) noexcept
        : RecordType(std::move(name), id, std::move(traits), std::move(fields), std::move(methods)) { }
};

/**
 * Represents a semantic class type.
 */
class ClassType final : public RecordType {
private:
    ClassType* _base;

public:
    explicit ClassType(std::string name,
                       TypeId id,
                       ClassType* base = nullptr,
                       std::vector<TraitType*> traits = {},
                       std::vector<RecordField> fields = {},
                       std::vector<TypeMethod> methods = {}) noexcept
        : RecordType(std::move(name), id, std::move(traits), std::move(fields), std::move(methods)), _base(base) { }

    [[nodiscard]]
    ClassType* base() const noexcept { return _base; }
};

/**
 * Represents a factory that manages `SemanticType` instances.
 */
class TypeTable {
private:
    std::vector<std::unique_ptr<SemanticType>> _types;

    TypeId _nextId = 0;

    // `Type` type specific
    bool _hasTypeType = false;
    TypeId _typeTypeId = 0;

    // Function type specific
    std::unordered_map<FuncSignature, MonoFuncType*> _funcTypeMap;

    /**
     * Represents the key for `OverloadedFuncType` instance caching map.
     */
    class OverloadedFuncKey {
    private:
        std::vector<MonoFuncType*> _funcTypes;

    public:
        explicit OverloadedFuncKey(std::vector<MonoFuncType*> funcTypes) noexcept
            : _funcTypes(std::move(funcTypes)) { }

        [[nodiscard]]
        const std::vector<MonoFuncType*>& funcTypes() const noexcept { return _funcTypes; }

        bool operator==(const OverloadedFuncKey& other) const noexcept;
    };

    /**
     * Represents the hasher for `OverloadedFuncKey`.
     */
    struct OverloadedFuncKeyHash {
        size_t operator()(const OverloadedFuncKey& key) const;
    };

    std::unordered_map<OverloadedFuncKey, OverloadedFuncType*, OverloadedFuncKeyHash> _overloadedFuncTypeMap;

public:
    MonoFuncType* makeMonoFuncType(std::string name, SemanticFunc* func);

    OverloadedFuncType* makeOverloadedFuncType(std::string name, const std::vector<MonoFuncType*>& funcTypes);

    TypeType* makeTypeType(std::string name, SemanticType* declaredType);

    TraitType* makeTraitType(std::string name,
                             std::vector<TraitType*> traits = {},
                             std::vector<TypeMethod> methods = {});

    StructType* makeStructType(std::string name,
                               std::vector<TraitType*> traits = {},
                               std::vector<RecordField> fields = {},
                               std::vector<TypeMethod> methods = {});

    ClassType* makeClassType(std::string name,
                             ClassType* base = nullptr,
                             std::vector<TraitType*> traits = {},
                             std::vector<RecordField> fields = {},
                             std::vector<TypeMethod> methods = {});

private:
    template <typename T, typename... Args>
    T* makeType(std::string name, TypeId id, Args&&... args) {
        std::unique_ptr<T> up = std::make_unique<T>(std::move(name), id, std::forward<Args>(args)...);
        T* p = up.get();
        _types.emplace_back(std::move(up));
        return p;
    }

    TypeId getNextId() noexcept;
};

} // Spark::FrontEnd
