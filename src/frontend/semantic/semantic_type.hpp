#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include "frontend/ast/node.hpp"

namespace Spark::FrontEnd {

class SemanticType {

};

class SemanticTypeTable {
private:
    std::vector<std::unique_ptr<SemanticType>> _types;

public:
    SemanticType* make(SemanticType type) {
        return _types.emplace_back(std::make_unique<SemanticType>(type)).get();
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
