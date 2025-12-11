#include "ast.hpp"

namespace Spark::FrontEnd {

AST::AST() {
    _root = std::make_unique<Block>(Location(1, 1), Location(1, 1));
}

AST::~AST() {
    destruct();
}

AST::AST(AST&& other) noexcept : _root(std::move(other._root)),
                                _nodes(std::move(other._nodes)) {
    other._root = nullptr;
}

AST& AST::operator=(AST&& other) noexcept {
    if (this != &other) {
        destruct();

        _root = std::move(other._root);
        _nodes = std::move(other._nodes);

        other._root = nullptr;
    }
    return *this;
}

void AST::free(Node* p) noexcept {
    if (_nodes.find(p) != _nodes.end()) {
        _nodes.erase(p);
        delete p;
    }
}

void AST::destruct() noexcept {
    for (Node* p : _nodes) {
        delete p;
    }
    _nodes.clear();
}

} // Spark::FrontEnd
