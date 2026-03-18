#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include "frontend/ast/node.hpp"

namespace Spark::FrontEnd {

class SemanticFunc;

/**
 * Represents a type during semantic passes.
 */
class SemanticType {
public:
    virtual ~SemanticType() = default;
};

/**
 * Represents a record type's field during semantic passes.
 */
class RecordField {
private:
    std::string name;
    SemanticType* _type;

public:
    explicit RecordField(SemanticType* type) noexcept : _type(type) { }

    [[nodiscard]]
    SemanticType* type() const noexcept { return _type; }
};

class RecordType : public SemanticType {
private:
    std::vector<SemanticType*> _traits;

    std::vector<std::pair<std::string, RecordField>> _fields;

    std::unordered_map<std::string, SemanticFunc*> _methods;

protected:
    explicit RecordType(std::vector<SemanticType*> traits = {}) noexcept
        : _traits(std::move(traits)) { }

public:
    [[nodiscard]]
    const std::vector<SemanticType*>& traits() const noexcept { return _traits; }

    [[nodiscard]]
    const std::unordered_map<std::string, SemanticFunc*>& methods() const noexcept { return _methods; }
};

class StructType final : public RecordType {
public:
    explicit StructType(std::vector<SemanticType*> traits) noexcept
        : RecordType(std::move(traits)) { }
};

class ClassType final : public RecordType {
private:
    SemanticType* _base;

public:
    explicit ClassType(SemanticType* base, std::vector<SemanticType*> traits = {}) noexcept
        : RecordType(std::move(traits)), _base(base) { }

    [[nodiscard]]
    SemanticType* base() const noexcept { return _base; }
};

class FuncType final : public SemanticType {
private:
    SemanticFunc* _func;

public:
    explicit FuncType(SemanticFunc* func) noexcept
        : _func(func) { }

    [[nodiscard]]
    SemanticFunc* func() const noexcept { return _func; }
};

/**
 * Represents a data structure that manages `SemanticType` instances and provides factory methods for creating
 * `SemanticType` instances.
 * Instantiated `SemanticType` instances will be deallocated when the table is destructed.
 */
class TypeTable {
private:
    std::vector<std::unique_ptr<SemanticType>> _types;

public:
    /**
     * Instantiates a `SemanticType` instance with the given object.
     * @param type `SemanticType` instance to instantiate.
     * @return Instantiated `SemanticType` instance.
     */
    SemanticType* make(SemanticType type);

    /**
     * Instantiates a function type.
     * @param func `SemanticFunc`
     * @return Instantiated function `SemanticType` instance.
     */
    SemanticType* makeFuncType(SemanticFunc* func, const std::vector<SemanticType*>& captures = {});

    /**
     * Instantiates a struct type.
     * @param traits Traits the struct type will implement (none by default).
     * @return Instantiated struct `SemanticType` instance.
     */
    SemanticType* makeStructType(std::vector<SemanticType*> traits = {});

    /**
     * Instantiates a class type.
     * @param base Base class type (none by default).
     * @param traits Traits the class type will implement (none by default).
     * @return Instantiated class `SemanticType` instance.
     */
    SemanticType* makeClassType(SemanticType* base = nullptr, std::vector<SemanticType*> traits = {});

    /**
     * Instantiates a tuple type with the given types.
     * @param types Types to form a tuple type.
     * @return Instantiated tuple `SemanticType` instance.
     */
    SemanticType* makeTupleType(const std::vector<SemanticType*>& types);

    /**
     * Instantiates a union type with the given types.
     * @param types Types to form a union type.
     * @return Instantiated union `SemanticType` instance.
     */
    SemanticType* makeUnionType(const std::vector<SemanticType*>& types);
};

/**
 * Represents a data structure that maps an AST node to a type.
 */
class NodeTypeMap {
private:
    std::unordered_map<const Node*, SemanticType*> _map;

public:
    SemanticType* get(const Node* node) const {
        auto it = _map.find(node);
        return it == _map.end() ? nullptr : it->second;
    }

    void set(const Node* node, SemanticType* type) {
        assert(node != nullptr);

        if (type == nullptr) {
            _map.erase(node);
        } else {
            _map[node] = type;
        }
    }
};

} // Spark::FrontEnd
