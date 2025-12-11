#pragma once

#include <memory>
#include <utility>
#include <unordered_set>

#include "ast/equal_visitor.hpp"
#include "ast/node.hpp"
#include "ast/node_visitor.hpp"

namespace Spark::FrontEnd {

class AST {
private:
    std::unique_ptr<Block> _root;
    std::unordered_set<Node*> _nodes;

public:
    [[nodiscard]] constexpr Block* root() const noexcept { return _root.get(); }

    AST();
    ~AST();

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
        T* ptr = new T(std::forward<Args>(args)...);
        _nodes.insert(ptr);
        return ptr;
    }

    /**
     * Deallocates a `Node` subtype instance created with `make`.
     * @param p Pointer to `Node` instance to deallocate.
     */
    void free(Node* p) noexcept;

private:
    void destruct() noexcept;
};

} // Spark::FrontEnd
