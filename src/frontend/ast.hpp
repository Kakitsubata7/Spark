#pragma once

#include <memory>
#include <utility>
#include <vector>

#include "ast/equal_visitor.hpp"
#include "ast/node.hpp"
#include "ast/node_visitor.hpp"

namespace Spark::FrontEnd {

class AST {
private:
    std::vector<std::unique_ptr<Node>> _nodes;

public:
    Node* root;

    AST() noexcept : root(nullptr) { }

    AST(const AST& other) = delete;
    AST& operator=(const AST& other) = delete;
    
    AST(AST&& other) noexcept;
    AST& operator=(AST&& other) noexcept;

    /**
     * Allocates a `Node` subtype instance and returns its pointer. The lifecycle of the instance is handled by
     * this class, deallocated in its destructor.
     * @tparam T A concrete subtype of `Node`.
     * @tparam Args Type of the arguments to forward to the constructor of the `Node` subtype.
     * @param args Arguments to forward to the constructor of the `Node` subtype.
     * @return Pointer to the allocated `Node` subtype instance.
     */
    template <typename T, typename... Args>
    T* make(Args&&... args) {
        auto ptr = std::make_unique<T>(std::forward<Args>(args)...);
        T* raw = ptr.get();
        _nodes.emplace_back(std::move(ptr));
        return raw;
    }
};

} // Spark::FrontEnd
