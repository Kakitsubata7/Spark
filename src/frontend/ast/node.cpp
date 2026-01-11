#include "node.hpp"

#include <typeinfo>

#define EQ_ASSERT_TYPE(v, Type)      \
({                                   \
    if (typeid(v) != typeid(Type)) { \
        return false;                \
    }                                \
    static_cast<const Type&>(v); /* NOLINT(cppcoreguidelines-pro-type-static-cast-downcast) */ \
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
    if (false /* TODO: || start != rhs.start || end != rhs.end */) {
        return false;
    }
    return equalsImpl(rhs);
}

bool Node::operator!=(const Node& rhs) const noexcept {
    return !(*this == rhs);
}

bool Assign::equalsImpl(const Node& rhs) const noexcept {
    const Assign& o = EQ_ASSERT_TYPE(rhs, Assign);
    return *lhs == *o.lhs && *this->rhs == *o.rhs && op == o.op;
}

bool TypeDef::equalsImpl(const Node& rhs) const noexcept {
    const TypeDef& o = EQ_ASSERT_TYPE(rhs, TypeDef);
    if (kind != o.kind || isImmutable != o.isImmutable || *decl != *o.decl || *body != *o.body) {
        return false;
    }
    if (bases.size() != o.bases.size()) {
        return false;
    }
    for (size_t i = 0; i < bases.size(); ++i) {
        if (*bases[i] != *o.bases[i]) {
            return false;
        }
    }
    return true;
}

bool IfElse::equalsImpl(const Node& rhs) const noexcept {
    const IfElse& o = EQ_ASSERT_TYPE(rhs, IfElse);
    return *condition == *o.condition && *thenBody == *o.thenBody && *elseBody == *o.elseBody;
}

bool While::equalsImpl(const Node& rhs) const noexcept {
    const While& o = EQ_ASSERT_TYPE(rhs, While);
    return *condition == *o.condition && *body == *o.body;
}

bool For::equalsImpl(const Node& rhs) const noexcept {
    const For& o = EQ_ASSERT_TYPE(rhs, For);
    return *iterator == *o.iterator && *range == *o.range && *body == *o.body;
}

bool Break::equalsImpl(const Node& rhs) const noexcept {
    EQ_ASSERT_TYPE(rhs, Break);
    return true;
}

bool Continue::equalsImpl(const Node& rhs) const noexcept {
    EQ_ASSERT_TYPE(rhs, Continue);
    return true;
}

bool Return::equalsImpl(const Node& rhs) const noexcept {
    const Return& o = EQ_ASSERT_TYPE(rhs, Return);
    return *expr == *o.expr;
}

bool Module::equalsImpl(const Node& rhs) const noexcept {
    const Module& o = EQ_ASSERT_TYPE(rhs, Module);
    if (*body != *o.body) {
        return false;
    }
    if (names.size() != o.names.size()) {
        return false;
    }
    for (size_t i = 0; i < names.size(); ++i) {
        if (*names[i] != *o.names[i]) {
            return false;
        }
    }
    return false;
}

bool Export::equalsImpl(const Node& rhs) const noexcept {
    const Export& o = EQ_ASSERT_TYPE(rhs, Export);
    return *stmt == *o.stmt;
}

bool Undefine::equalsImpl(const Node& rhs) const noexcept {
    const Undefine& o = EQ_ASSERT_TYPE(rhs, Undefine);
    return *var == *o.var;
}

bool IfThen::equalsImpl(const Node& rhs) const noexcept {
    const IfThen& o = EQ_ASSERT_TYPE(rhs, IfThen);
    return *condition == *o.condition && *thenExpr == *o.thenExpr && *elseExpr == *o.elseExpr;
}

bool TryElse::equalsImpl(const Node& rhs) const noexcept {
    const TryElse& o = EQ_ASSERT_TYPE(rhs, TryElse);
    return *tryExpr == *o.tryExpr && *elseExpr == *o.elseExpr;
}

bool Match::equalsImpl(const Node& rhs) const noexcept {
    const Match& o = EQ_ASSERT_TYPE(rhs, Match);
    if (*binding != *o.binding || *scrutinee != *o.scrutinee) {
        return false;
    }
    if (cases.size() != o.cases.size()) {
        return false;
    }
    for (size_t i = 0; i < cases.size(); ++i) {
        const Case& lc = cases[i];
        const Case& rc = o.cases[i];
        if (*lc.pattern != *rc.pattern || *lc.guard != *rc.guard || *lc.body != *rc.body) {
            return false;
        }
    }
    return true;
}

bool TryCatch::equalsImpl(const Node& rhs) const noexcept {
    const TryCatch& o = EQ_ASSERT_TYPE(rhs, TryCatch);
    if (*expr != *o.expr) {
        return false;
    }
    if (catches.size() != o.catches.size()) {
        return false;
    }
    for (size_t i = 0; i < catches.size(); ++i) {
        const Catch& lc = catches[i];
        const Catch& rc = o.catches[i];
        if (*lc.pattern != *rc.pattern || *lc.guard != *rc.guard || *lc.body != *rc.body) {
            return false;
        }
    }
    return true;
}

bool Throw::equalsImpl(const Node& rhs) const noexcept {
    const Throw& o = EQ_ASSERT_TYPE(rhs, Throw);
    return *expr == *o.expr;
}

bool Block::equalsImpl(const Node& rhs) const noexcept {
    const Block& o = EQ_ASSERT_TYPE(rhs, Block);
    if (elements.size() != o.elements.size()) {
        return false;
    }
    for (size_t i = 0; i < elements.size(); ++i) {
        if (*elements[i] != *o.elements[i]) {
            return false;
        }
    }
    return true;
}

bool BinaryExpr::equalsImpl(const Node& rhs) const noexcept {
    const BinaryExpr& o = EQ_ASSERT_TYPE(rhs, BinaryExpr);
    return op == o.op && *lhs == *o.lhs && *this->rhs == *o.rhs;
}

bool PrefixExpr::equalsImpl(const Node& rhs) const noexcept {
    const PrefixExpr& o = EQ_ASSERT_TYPE(rhs, PrefixExpr);
    return op == o.op && *expr == *o.expr;
}

bool PostfixExpr::equalsImpl(const Node& rhs) const noexcept {
    const PostfixExpr& o = EQ_ASSERT_TYPE(rhs, PostfixExpr);
    return op == o.op && *expr == *o.expr;
}

bool MemberAccess::equalsImpl(const Node& rhs) const noexcept {
    const MemberAccess& o = EQ_ASSERT_TYPE(rhs, MemberAccess);
    return *member == *o.member;
}

bool Subscript::equalsImpl(const Node& rhs) const noexcept {
    const Subscript& o = EQ_ASSERT_TYPE(rhs, Subscript);
    if (*callee != *o.callee) {
        return false;
    }
    if (indices.size() != o.indices.size()) {
        return false;
    }
    for (size_t i = 0; i < indices.size(); ++i) {
        if (*indices[i] != *o.indices[i]) {
            return false;
        }
    }
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

bool Identifier::equalsImpl(const Node& rhs) const noexcept {
    const Identifier& o = EQ_ASSERT_TYPE(rhs, Identifier);
    return name == o.name;
}

bool Wildcard::equalsImpl(const Node& rhs) const noexcept {
    EQ_ASSERT_TYPE(rhs, Wildcard);
    return true;
}

bool Upvalue::equalsImpl(const Node& rhs) const noexcept {
    const Upvalue& o = EQ_ASSERT_TYPE(rhs, Upvalue);
    return level == o.level && *var == *o.var;
}

bool Global::equalsImpl(const Node& rhs) const noexcept {
    EQ_ASSERT_TYPE(rhs, Global);
    return true;
}

bool Self::equalsImpl(const Node& rhs) const noexcept {
    EQ_ASSERT_TYPE(rhs, Self);
    return true;
}

bool OverloadableOp::equalsImpl(const Node& rhs) const noexcept {
    const OverloadableOp& o = EQ_ASSERT_TYPE(rhs, OverloadableOp);
    return op == o.op;
}

bool BindingModifier::equalsImpl(const Node& rhs) const noexcept {
    const BindingModifier& o = EQ_ASSERT_TYPE(rhs, BindingModifier);
    return kind == o.kind && isImmutable == o.isImmutable && opt == o.opt;
}

} // Spark::FrontEnd
