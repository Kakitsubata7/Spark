#pragma once

#include <memory>
#include <utility>
#include <vector>

#include "ast/expr.hpp"
#include "ast/node.hpp"
#include "ast/node_visitor.hpp"
#include "ast/pattern.hpp"
#include "ast/stmt.hpp"
#include "ast/type_nodes.hpp"
#include "ast/utils.hpp"

namespace Spark::FrontEnd {

class AST {
private:
    BlockStmt _root;
    std::vector<std::unique_ptr<Node>> _nodes;

public:
    [[nodiscard]] constexpr BlockStmt* root() noexcept { return &_root; }

    AST() : _root({1, 1}, {1, 1}, {}) { }

    AST(const AST& other) = default;
    AST& operator=(const AST& other) = default;
    AST(AST&& other) noexcept = default;
    AST& operator=(AST&& other) noexcept = default;

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
        std::unique_ptr<T> ptr = std::make_unique<T>(std::forward<Args>(args)...);
        T* raw = ptr.get();
        _nodes.emplace_back(std::move(ptr));
        return raw;
    }
};

} // Spark::FrontEnd
