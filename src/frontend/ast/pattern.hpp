#pragma once

#include <optional>
#include <vector>

#include "identifier.hpp"
#include "type.hpp"
#include "node.hpp"

namespace Spark::FrontEnd::AST {

struct Pattern : Node {
    Type* type = nullptr;
};

struct VarPattern final : Pattern {
    Name varName;
    Path typePath;

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct TuplePattern final : Pattern {
    struct Component {
        Name varName;
        std::optional<Path> typePath = std::nullopt;
    };

    std::vector<Component> components;

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct CollectionPattern final : Pattern {
    std::vector<Expr*> prefix;
    std::vector<Expr*> suffix;
    bool hasGap = false;

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

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
    Pattern* left;
    Pattern* right;

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

} // Spark::FrontEnd::AST
