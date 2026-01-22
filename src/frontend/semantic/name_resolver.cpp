#include "name_resolver.hpp"

#include <stack>

namespace Spark::FrontEnd {

std::vector<Error> NameResolver::resolve(AST& ast) {
    Visitor visitor;

    std::stack<Node*> stack;
    stack.push(ast.root);

    while (!stack.empty()) {
        Node* node = stack.top();
        if (node != nullptr) {
            visitor.visit(*node);
        }
    }

    return {};
}

} // Spark::FrontEnd
