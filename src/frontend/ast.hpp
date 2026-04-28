#pragma once

#include <memory>
#include <utility>
#include <vector>

#include "ast/child_visitor.hpp"
#include "ast/literal.hpp"
#include "ast/name.hpp"
#include "ast/node.hpp"
#include "ast/node_visitor.hpp"

namespace Spark::FrontEnd {

class AST {
private:
    std::vector<std::unique_ptr<Node>> _nodes;

    NameValueInterner _interner;

public:
    Node* root;

    AST() noexcept : root(nullptr) { }

    AST(const AST& other) = delete;
    AST& operator=(const AST& other) = delete;
    
    AST(AST&& other) noexcept = default;
    AST& operator=(AST&& other) = default;

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

    /**
     * Interns a `NameValue` instance and returns an `InternedNameValue`. The lifecycle of the `NameValue` is handled
     * by this class, deallocated in its destructor.
     * @param v `NameValue` instance to intern.
     * @return `InternedNameValue` instance.
     */
    InternedNameValue intern(NameValue v) {
        return _interner.intern(std::move(v));
    }
};

} // Spark::FrontEnd
