#include "node.hpp"

#include <typeinfo>

#define EQ_ASSERT_TYPE(v, Type)      \
({                                   \
    if (typeid(v) != typeid(Type)) { \
        return false;                \
    }                                \
    static_cast<const Type&>(v);     \
})

namespace Spark::FrontEnd {

bool ptrEq(const Node* lhs, const Node* rhs) {
    if (lhs == rhs) {
        return true;
    }
    if (lhs == nullptr || rhs == nullptr) {
        return false;
    }
    return *lhs == *rhs;
}

bool Node::operator==(const Node& rhs) const noexcept {
    if (this == &rhs) {
        return true;
    }
    if (kind() != rhs.kind() /* TODO: || start != rhs.start || end != rhs.end */) {
        return false;
    }
    return equalsImpl(rhs);
}

bool Node::operator!=(const Node& rhs) const noexcept {
    return !(*this == rhs);
}

bool Body::equalsImpl(const Node& rhs) const noexcept {
    const Body& o = EQ_ASSERT_TYPE(rhs, Body);
    if (nodes.size() != o.nodes.size()) {
        return false;
    }
    for (size_t i = 0; i < nodes.size(); ++i) {
        if (*nodes[i] != *o.nodes[i]) {
            return false;
        }
    }
    return true;
}

bool BlockStmt::equalsImpl(const Node& rhs) const noexcept {
    const BlockStmt& o = EQ_ASSERT_TYPE(rhs, BlockStmt);
    return *body == *o.body;
}

bool IfElseStmt::equalsImpl(const Node& rhs) const noexcept {
    const IfElseStmt& o = EQ_ASSERT_TYPE(rhs, IfElseStmt);

    if (ifBranches.size() != o.ifBranches.size()) {
        return false;
    }
    for (size_t i = 0; i < ifBranches.size(); ++i) {
        const Branch& b = ifBranches[i];
        const Branch& ob = o.ifBranches[i];
        if (*b.condition != *ob.condition || *b.block != *ob.block) {
            return false;
        }
    }

    if (!ptrEq(elseBlock, o.elseBlock)) {
        return false;
    }

    return true;
}

bool WhileStmt::equalsImpl(const Node& rhs) const noexcept {
    const WhileStmt& o = EQ_ASSERT_TYPE(rhs, WhileStmt);
    return *condition == *o.condition && *block == *o.block;
}

bool ForStmt::equalsImpl(const Node& rhs) const noexcept {
    const ForStmt& o = EQ_ASSERT_TYPE(rhs, ForStmt);
    return *iterator == *o.iterator && *range == *o.range && *block == *o.block;
}

bool BreakStmt::equalsImpl(const Node& rhs) const noexcept {
    EQ_ASSERT_TYPE(rhs, BreakStmt);
    return true;
}

bool ContinueStmt::equalsImpl(const Node& rhs) const noexcept {
    EQ_ASSERT_TYPE(rhs, ContinueStmt);
    return true;
}

bool IfThenExpr::equalsImpl(const Node& rhs) const noexcept {
    const IfThenExpr& o = EQ_ASSERT_TYPE(rhs, IfThenExpr);
    return *condition == *o.condition && *trueNode == *o.trueNode && *falseNode == *o.falseNode;
}

bool BindingModifier::operator==(const BindingModifier& rhs) const noexcept {
    return kind == rhs.kind &&
           isImmutable == rhs.isImmutable &&
           isNullable == rhs.isNullable &&
           isNonNullable == rhs.isNonNullable;
}

bool BindingModifier::operator!=(const BindingModifier& rhs) const noexcept {
    return !(*this == rhs);
}

bool BindingPattern::equalsImpl(const Node& rhs) const noexcept {
    const BindingPattern& o = EQ_ASSERT_TYPE(rhs, BindingPattern);
    return mod == o.mod && *bind == *o.bind && ptrEq(typeAnnot, o.typeAnnot);
}

bool TuplePattern::equalsImpl(const Node& rhs) const noexcept {
    const TuplePattern& o = EQ_ASSERT_TYPE(rhs, TuplePattern);
    if (patterns.size() != o.patterns.size()) {
        return false;
    }
    for (size_t i = 0; i < patterns.size(); ++i) {
        if (*patterns[i] != *o.patterns[i]) {
            return false;
        }
    }
    return true;
}

bool CollectionPattern::equalsImpl(const Node& rhs) const noexcept {
    const CollectionPattern& o = EQ_ASSERT_TYPE(rhs, CollectionPattern);
    if (hasRest != o.hasRest || prefix.size() != o.prefix.size() || suffix.size() != o.suffix.size()) {
        return false;
    }
    for (size_t i = 0; i < prefix.size(); ++i) {
        if (*prefix[i] != *o.prefix[i]) {
            return false;
        }
    }
    for (size_t i = 0; i < suffix.size(); ++i) {
        if (*suffix[i] != *o.suffix[i]) {
            return false;
        }
    }
    return true;
}

bool Identifier::equalsImpl(const Node& rhs) const noexcept {
    const Identifier& o = EQ_ASSERT_TYPE(rhs, Identifier);
    return name == o.name;
}

bool Discard::equalsImpl(const Node& rhs) const noexcept {
    EQ_ASSERT_TYPE(rhs, Discard);
    return true;
}

bool IntLiteral::equalsImpl(const Node& rhs) const noexcept {
    const IntLiteral& o = EQ_ASSERT_TYPE(rhs, IntLiteral);
    return value == o.value;
}

bool RealLiteral::equalsImpl(const Node& rhs) const noexcept {
    const RealLiteral& o = EQ_ASSERT_TYPE(rhs, RealLiteral);
    return value == o.value;
}

bool BoolLiteral::equalsImpl(const Node& rhs) const noexcept {
    const BoolLiteral& o = EQ_ASSERT_TYPE(rhs, BoolLiteral);
    return value == o.value;
}

bool StringLiteral::equalsImpl(const Node& rhs) const noexcept {
    const StringLiteral& o = EQ_ASSERT_TYPE(rhs, StringLiteral);
    return value == o.value;
}

bool NilLiteral::equalsImpl(const Node& rhs) const noexcept {
    EQ_ASSERT_TYPE(rhs, NilLiteral);
    return true;
}

bool VoidLiteral::equalsImpl(const Node& rhs) const noexcept {
    EQ_ASSERT_TYPE(rhs, VoidLiteral);
    return true;
}

bool Undefined::equalsImpl(const Node& rhs) const noexcept {
    EQ_ASSERT_TYPE(rhs, Undefined);
    return true;
}

} // Spark::FrontEnd
