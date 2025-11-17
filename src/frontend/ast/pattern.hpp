#pragma once

#include <vector>

#include "identifier.hpp"
#include "type.hpp"
#include "node.hpp"

namespace Spark::FrontEnd::AST {

struct Pattern : Node {
    Type type;
};

struct VarPattern final : Pattern {
    Name varName;
    Path typePath;

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct TuplePattern final : Pattern {
    struct Component {
        Name varName;
        Path typePath;
    };

    std::vector<Component> components;

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct CollectionPattern final : Pattern {

};

struct FieldPattern final : Pattern {
    struct Field {
        Path typePath;
    };

    Path typePath;
};

} // Spark::FrontEnd::AST
