#pragma once

#include <vector>

#include "frontend/ast.hpp"
#include "utils/error.hpp"

namespace Spark::FrontEnd {

class NameResolver {
private:
    class Visitor : public NodeVisitor {

    };

public:
    static std::vector<Error> resolve(AST& ast);
};

} // Spark::FrontEnd
