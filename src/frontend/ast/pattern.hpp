#pragma once

#include <optional>
#include <vector>

#include "type.hpp"
#include "node.hpp"
#include "utils.hpp"
#include "utils/bigint.hpp"
#include "utils/bigreal.hpp"

namespace Spark::FrontEnd::AST {

struct Pattern : Node {
    Type* type = nullptr;
};

struct IntLiteralPattern final : Pattern {
    BigInt value;

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct RealLiteralPattern final : Pattern {
    BigReal value;

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct BoolLiteralPattern final : Pattern {
    bool value = false;

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct StringLiteralPattern final : Pattern {
    std::string value;

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct NilLiteralPattern final : Pattern {
    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct WildcardPattern final : Pattern {
    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct VarPattern final : Pattern {
    Name varName;
    std::optional<Path> typePath = std::nullopt;

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct TuplePattern final : Pattern {
    std::vector<Pattern*> components;

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct CollectionPattern final : Pattern {
    std::vector<Pattern*> prefix;
    std::vector<Pattern*> suffix;
    bool hasGap = false;

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct FieldPattern final : Pattern {
    struct Field {
        Name fieldName;
        Pattern* pattern = nullptr;
    };

    Path typePath;
    std::vector<Field> fields;

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

struct OrPattern final : Pattern {
    Pattern* left = nullptr;
    Pattern* right = nullptr;

    void accept(NodeVisitor& v) override { v.visit(*this); }
};

} // Spark::FrontEnd::AST
