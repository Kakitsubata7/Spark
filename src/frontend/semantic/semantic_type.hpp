#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include "frontend/ast/node.hpp"

namespace Spark::FrontEnd {

enum class TypeKind {
    Struct,
    Class
};

class SemanticType;

class TypeField {
private:
    SemanticType* _type;

public:
    explicit TypeField(SemanticType* type) noexcept : _type(type) { }

    [[nodiscard]]
    SemanticType* type() const noexcept { return _type; }
};

class TypeMethod {
private:

};

class SemanticType {
private:
    TypeKind _kind;

    std::unordered_map<std::string_view, TypeField> _fieldMap;
    std::unordered_map<std::string_view, TypeMethod> _methodMap;

public:
    explicit SemanticType(TypeKind kind) noexcept : _kind(kind) { }

    [[nodiscard]]
    TypeKind kind() const noexcept { return _kind;}

    [[nodiscard]]
    const TypeField& getField(std::string_view name) const {
        auto it = _fieldMap.find(name);
        assert(it != _fieldMap.end());
        return it->second;
    }

    void setField(std::string_view name, TypeField field) {
        _fieldMap.insert({name, std::move(field)});
    }

    [[nodiscard]]
    bool hasField(std::string_view name) const {
        return _fieldMap.find(name) != _fieldMap.end();
    }

    [[nodiscard]]
    const TypeMethod& getMethod(std::string_view name) const {
        auto it = _methodMap.find(name);
        assert(it != _methodMap.end());
        return it->second;
    }

    void setMethod(std::string_view name, TypeMethod method) {
        _methodMap.insert({name, std::move(method)});
    }

    [[nodiscard]]
    bool hasMethod(std::string_view name) const {
        return _methodMap.find(name) != _methodMap.end();
    }
};

class SemanticTypeTable {
private:
    std::vector<std::unique_ptr<SemanticType>> _types;

public:
    SemanticType* make(SemanticType type) {
        return _types.emplace_back(std::make_unique<SemanticType>(std::move(type))).get();
    }
};

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
