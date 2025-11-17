#pragma once

#include <memory>
#include <utility>
#include <vector>

#include "expr.hpp"
#include "node.hpp"
#include "stmt.hpp"

namespace Spark::FrontEnd::AST {

class AST {
private:
    BlockStmt _root{1, 1, {}};
    std::vector<std::unique_ptr<Node>> _nodes;

public:
    AST() = default;

    AST(const AST& other) = default;
    AST& operator=(const AST& other) = default;
    AST(AST&& other) noexcept = default;
    AST& operator=(AST&& other) noexcept = default;

    /**
     * Allocates a `NodeBase` subtype instance and returns its pointer. The lifecycle of the instance is handled by
     * this class, deallocated in its destructor.
     * @tparam T A concrete subtype of `NodeBase`.
     * @tparam Args Type of the arguments to forward to the constructor of the `NodeBase` subtype.
     * @param args Arguments to forward to the constructor of the `NodeBase` subtype.
     * @return Pointer to the allocated `NodeBase` subtype instance.
     */
    template <typename T, typename... Args>
    T* make(Args&&... args) {
        return _nodes.emplace_back<T>(std::forward<Args>(args)...);
    }
};

} // Spark::FrontEnd::AST
