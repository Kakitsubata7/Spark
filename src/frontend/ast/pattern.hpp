#pragma once

#include <vector>

#include "identifier.hpp"
#include "type.hpp"
#include "node.hpp"

namespace Spark::FrontEnd::AST {

struct Pattern : Node {
    Type* type = nullptr;
};

/**
 * `x: T` : T
 * `x` : Any
 */
struct VarPattern final : Pattern {
    Name varName;
    Path typePath;

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

/**
 * `(a: A, b: B, c: C)` : (A, B, C)
 * `(a, b, c)` : (Any, Any, Any)
 */
struct TuplePattern final : Pattern {
    struct Component {
        Name varName;
        std::optional<Path> typePath = std::nullopt;
    };

    std::vector<Component> components;

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct CollectionPattern final : Pattern {
    
};

/**
 * `T(a: A as x, b: B as y)` : T
 * `T(a: A, b)` : T
 * `Any(a, b)` : Any
 */
struct FieldPattern final : Pattern {
    struct Field {
        Name fieldName;
        std::optional<Name> asName = std::nullopt;
        std::optional<Path> typePath = std::nullopt;
    };

    Path typePath;
    std::vector<Field> fields;

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct OrPattern final : Pattern {

};

} // Spark::FrontEnd::AST
