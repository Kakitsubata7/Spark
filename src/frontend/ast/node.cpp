#include "node.hpp"

#include <typeinfo>

#define EQ_ASSERT_TYPE(v, Type)      \
({                                   \
    if (typeid(v) != typeid(Type)) { \
        return false;                \
    }                                \
    static_cast<const Type&>(v); /* NOLINT(cppcoreguidelines-pro-type-static-cast-downcast) */ \
})

bool ptrEq(const Spark::FrontEnd::Node* lhs, const Spark::FrontEnd::Node* rhs) {
    if (lhs == rhs) {
        return true;
    }
    if (lhs == nullptr || rhs == nullptr) {
        return false;
    }
    return *lhs == *rhs;
}

namespace Spark::FrontEnd {

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



bool IdentifierName::equalsImpl(const Node& rhs) const noexcept {
    const IdentifierName& o = EQ_ASSERT_TYPE(rhs, IdentifierName);
    return name == o.name;
}

bool ConstructorName::equalsImpl(const Node& rhs) const noexcept {
    EQ_ASSERT_TYPE(rhs, ConstructorName);
    return true;
}

bool DestructorName::equalsImpl(const Node& rhs) const noexcept {
    EQ_ASSERT_TYPE(rhs, DestructorName);
    return true;
}

bool OverloadableOpName::equalsImpl(const Node& rhs) const noexcept {
    const OverloadableOpName& o = EQ_ASSERT_TYPE(rhs, OverloadableOpName);
    return op == o.op;
}

bool SelfName::equalsImpl(const Node& rhs) const noexcept {
    EQ_ASSERT_TYPE(rhs, SelfName);
    return true;
}



bool LambdaExpr::equalsImpl(const Node& rhs) const noexcept {
    const LambdaExpr& o = EQ_ASSERT_TYPE(rhs, LambdaExpr);
    if (params.size() != o.params.size() || returns.size() != o.returns.size()) {
        return false;
    }
    for (size_t i = 0; i < params.size(); ++i) {
        if (*params[i] != *o.params[i]) {
            return false;
        }
    }
    for (size_t i = 0; i < returns.size(); ++i) {
        if (*returns[i] != *o.returns[i]) {
            return false;
        }
    }
    return isImmutable == o.isImmutable && ptrEq(captureClause, o.captureClause) && isThrowing == o.isThrowing &&
           ptrEq(throwExpr, o.throwExpr) && *body == *o.body;
}

bool IfThenExpr::equalsImpl(const Node& rhs) const noexcept {
    const IfThenExpr& o = EQ_ASSERT_TYPE(rhs, IfThenExpr);
    return *condition == *o.condition && *thenExpr == *o.thenExpr && *elseExpr == *o.elseExpr;
}

bool TryElseExpr::equalsImpl(const Node& rhs) const noexcept {
    const TryElseExpr& o = EQ_ASSERT_TYPE(rhs, TryElseExpr);
    return *tryExpr == *o.tryExpr && *elseExpr == *o.elseExpr;
}

bool MatchCase::equalsImpl(const Node& rhs) const noexcept {
    const MatchCase& o = EQ_ASSERT_TYPE(rhs, MatchCase);
    return ptrEq(pattern, o.pattern) || ptrEq(guard, o.guard) || *body == *o.body;
}

bool MatchExpr::equalsImpl(const Node& rhs) const noexcept {
    const MatchExpr& o = EQ_ASSERT_TYPE(rhs, MatchExpr);
    if (*scrutinee != *o.scrutinee) {
        return false;
    }
    if (cases.size() != o.cases.size()) {
        return false;
    }
    for (size_t i = 0; i < cases.size(); ++i) {
        if (*cases[i] != *o.cases[i]) {
            return false;
        }
    }
    return true;
}

bool CatchClause::equalsImpl(const Node& rhs) const noexcept {
    const CatchClause& o = EQ_ASSERT_TYPE(rhs, CatchClause);
    return ptrEq(pattern, o.pattern) || ptrEq(guard, o.guard) || *body == *o.body;
}

bool TryCatchExpr::equalsImpl(const Node& rhs) const noexcept {
    const TryCatchExpr& o = EQ_ASSERT_TYPE(rhs, TryCatchExpr);
    if (*expr != *o.expr) {
        return false;
    }
    if (catches.size() != o.catches.size()) {
        return false;
    }
    for (size_t i = 0; i < catches.size(); ++i) {
        if (*catches[i] != *o.catches[i]) {
            return false;
        }
    }
    return true;
}

bool ThrowExpr::equalsImpl(const Node& rhs) const noexcept {
    const ThrowExpr& o = EQ_ASSERT_TYPE(rhs, ThrowExpr);
    return *expr == *o.expr;
}

bool BlockExpr::equalsImpl(const Node& rhs) const noexcept {
    const BlockExpr& o = EQ_ASSERT_TYPE(rhs, BlockExpr);
    if (stmts.size() != o.stmts.size()) {
        return false;
    }
    for (size_t i = 0; i < stmts.size(); ++i) {
        if (*stmts[i] != *o.stmts[i]) {
            return false;
        }
    }
    return true;
}

bool IsExpr::equalsImpl(const Node& rhs) const noexcept {
    const IsExpr& o = EQ_ASSERT_TYPE(rhs, IsExpr);
    return *expr == *o.expr && *type == *o.type;
}

bool AsExpr::equalsImpl(const Node& rhs) const noexcept {
    const AsExpr& o = EQ_ASSERT_TYPE(rhs, AsExpr);
    return *expr == *o.expr && *pattern == *o.pattern && *type == *o.type;
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

bool MemberAccessExpr::equalsImpl(const Node& rhs) const noexcept {
    const MemberAccessExpr& o = EQ_ASSERT_TYPE(rhs, MemberAccessExpr);
    return *base == *o.base && *member == *o.member;
}

bool CallExpr::equalsImpl(const Node& rhs) const noexcept {
    const CallExpr& o = EQ_ASSERT_TYPE(rhs, CallExpr);
    if (*callee != *o.callee) {
        return false;
    }
    if (args.size() != o.args.size()) {
        return false;
    }
    for (size_t i = 0; i < args.size(); ++i) {
        auto [lName, lExpr] = args[i];
        auto [rName, rExpr] = o.args[i];
        if (!ptrEq(lName, rName) || *lExpr != *rExpr) {
            return false;
        }
    }
    return true;
}

bool SubscriptExpr::equalsImpl(const Node& rhs) const noexcept {
    const SubscriptExpr& o = EQ_ASSERT_TYPE(rhs, SubscriptExpr);
    if (*base != *o.base) {
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

bool IntLiteralExpr::equalsImpl(const Node& rhs) const noexcept {
    const IntLiteralExpr& o = EQ_ASSERT_TYPE(rhs, IntLiteralExpr);
    return value == o.value;
}

bool RealLiteralExpr::equalsImpl(const Node& rhs) const noexcept {
    const RealLiteralExpr& o = EQ_ASSERT_TYPE(rhs, RealLiteralExpr);
    return value == o.value;
}

bool BoolLiteralExpr::equalsImpl(const Node& rhs) const noexcept {
    const BoolLiteralExpr& o = EQ_ASSERT_TYPE(rhs, BoolLiteralExpr);
    return value == o.value;
}

bool StringLiteralExpr::equalsImpl(const Node& rhs) const noexcept {
    const StringLiteralExpr& o = EQ_ASSERT_TYPE(rhs, StringLiteralExpr);
    return value == o.value;
}

bool NilLiteralExpr::equalsImpl(const Node& rhs) const noexcept {
    EQ_ASSERT_TYPE(rhs, NilLiteralExpr);
    return true;
}

bool VoidLiteralExpr::equalsImpl(const Node& rhs) const noexcept {
    EQ_ASSERT_TYPE(rhs, VoidLiteralExpr);
    return true;
}

bool IdentifierExpr::equalsImpl(const Node& rhs) const noexcept {
    const IdentifierExpr& o = EQ_ASSERT_TYPE(rhs, IdentifierExpr);
    return name == o.name;
}

bool ConstructorExpr::equalsImpl(const Node& rhs) const noexcept {
    EQ_ASSERT_TYPE(rhs, ConstructorExpr);
    return true;
}

bool DestructorExpr::equalsImpl(const Node& rhs) const noexcept {
    EQ_ASSERT_TYPE(rhs, DestructorExpr);
    return true;
}

bool OverloadableOpExpr::equalsImpl(const Node& rhs) const noexcept {
    const OverloadableOpExpr& o = EQ_ASSERT_TYPE(rhs, OverloadableOpExpr);
    return op == o.op;
}

bool SelfExpr::equalsImpl(const Node& rhs) const noexcept {
    EQ_ASSERT_TYPE(rhs, SelfExpr);
    return true;
}

bool GlobalAccessExpr::equalsImpl(const Node& rhs) const noexcept {
    const GlobalAccessExpr& o = EQ_ASSERT_TYPE(rhs, GlobalAccessExpr);
    return *name == *o.name;
}

bool UpvalueExpr::equalsImpl(const Node& rhs) const noexcept {
    const UpvalueExpr& o = EQ_ASSERT_TYPE(rhs, UpvalueExpr);
    return level == o.level && *name == *o.name;
}

bool TupleExpr::equalsImpl(const Node& rhs) const noexcept {
    const TupleExpr& o = EQ_ASSERT_TYPE(rhs, TupleExpr);
    if (exprs.size() != o.exprs.size()) {
        return false;
    }
    for (size_t i = 0; i < exprs.size(); ++i) {
        if (*exprs[i] != *o.exprs[i]) {
            return false;
        }
    }
    return true;
}

bool CollectionExpr::equalsImpl(const Node& rhs) const noexcept {
    const CollectionExpr& o = EQ_ASSERT_TYPE(rhs, CollectionExpr);
    if (exprs.size() != o.exprs.size()) {
        return false;
    }
    for (size_t i = 0; i < exprs.size(); ++i) {
        if (*exprs[i] != *o.exprs[i]) {
            return false;
        }
    }
    return true;
}

bool TypeofExpr::equalsImpl(const Node& rhs) const noexcept {
    const TypeofExpr& o = EQ_ASSERT_TYPE(rhs, TypeofExpr);
    return *expr == *o.expr;
}



bool VarModifier::equalsImpl(const Node& rhs) const noexcept {
    const VarModifier& o = EQ_ASSERT_TYPE(rhs, VarModifier);
    return kind == o.kind && isImmutable == o.isImmutable && optionality == o.optionality;
}

bool VarDefStmt::equalsImpl(const Node& rhs) const noexcept {
    const VarDefStmt& o = EQ_ASSERT_TYPE(rhs, VarDefStmt);
    return *mod == *o.mod && *pattern == *o.pattern && ptrEq(type, o.type) && ptrEq(this->rhs, o.rhs);
}

bool AssignStmt::equalsImpl(const Node& rhs) const noexcept {
    const AssignStmt& o = EQ_ASSERT_TYPE(rhs, AssignStmt);
    return *lhs == *o.lhs && *this->rhs == *o.rhs && op == o.op;
}

bool FnParam::equalsImpl(const Node& rhs) const noexcept {
    const FnParam& o = EQ_ASSERT_TYPE(rhs, FnParam);
    return ptrEq(mod, o.mod) && *pattern == *o.pattern && ptrEq(type, o.type) && ptrEq(def, o.def);
}

bool FnCapture::equalsImpl(const Node& rhs) const noexcept {
    const FnCapture& o = EQ_ASSERT_TYPE(rhs, FnCapture);
    return ptrEq(mod, o.mod) && *pattern == *o.pattern;
}

bool FnCaptureClause::equalsImpl(const Node& rhs) const noexcept {
    const FnCaptureClause& o = EQ_ASSERT_TYPE(rhs, FnCaptureClause);
    if (captures.size() != o.captures.size()) {
        return false;
    }
    for (size_t i = 0; i < captures.size(); ++i) {
        if (*captures[i] != *o.captures[i]) {
            return false;
        }
    }
    return hasRest == o.hasRest && ptrEq(restMod, o.restMod);
}

bool FnReturn::equalsImpl(const Node& rhs) const noexcept {
    const FnReturn& o = EQ_ASSERT_TYPE(rhs, FnReturn);
    return kind == o.kind && *type == *o.type;
}

bool FnDefStmt::equalsImpl(const Node& rhs) const noexcept {
    const FnDefStmt& o = EQ_ASSERT_TYPE(rhs, FnDefStmt);
    if (generics.size() != o.generics.size() || params.size() != o.params.size() || returns.size() != o.returns.size()) {
        return false;
    }
    for (size_t i = 0; i < generics.size(); ++i) {
        if (*generics[i] != *o.generics[i]) {
            return false;
        }
    }
    for (size_t i = 0; i < params.size(); ++i) {
        if (*params[i] != *o.params[i]) {
            return false;
        }
    }
    for (size_t i = 0; i < returns.size(); ++i) {
        if (*returns[i] != *o.returns[i]) {
            return false;
        }
    }
    return isImmutable == o.isImmutable && *name == *o.name && ptrEq(captureClause, o.captureClause) &&
           isThrowing == o.isThrowing && ptrEq(throwExpr, o.throwExpr) && *body == *o.body;
}

bool TypeDefStmt::equalsImpl(const Node& rhs) const noexcept {
    const TypeDefStmt& o = EQ_ASSERT_TYPE(rhs, TypeDefStmt);
    if (kind != o.kind || isImmutable != o.isImmutable || *name != *o.name || *body != *o.body) {
        return false;
    }
    if (generics.size() != o.generics.size()) {
        return false;
    }
    for (size_t i = 0; i < generics.size(); ++i) {
        if (*generics[i] != *o.generics[i]) {
            return false;
        }
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

bool IfStmt::equalsImpl(const Node& rhs) const noexcept {
    const IfStmt& o = EQ_ASSERT_TYPE(rhs, IfStmt);
    return *condition == *o.condition && *thenBody == *o.thenBody && *elseBody == *o.elseBody;
}

bool WhileStmt::equalsImpl(const Node& rhs) const noexcept {
    const WhileStmt& o = EQ_ASSERT_TYPE(rhs, WhileStmt);
    return *condition == *o.condition && *body == *o.body;
}

bool ForStmt::equalsImpl(const Node& rhs) const noexcept {
    const ForStmt& o = EQ_ASSERT_TYPE(rhs, ForStmt);
    return *iterator == *o.iterator && *range == *o.range && *body == *o.body;
}

bool BreakStmt::equalsImpl(const Node& rhs) const noexcept {
    EQ_ASSERT_TYPE(rhs, BreakStmt);
    return true;
}

bool ContinueStmt::equalsImpl(const Node& rhs) const noexcept {
    EQ_ASSERT_TYPE(rhs, ContinueStmt);
    return true;
}

bool ReturnStmt::equalsImpl(const Node& rhs) const noexcept {
    const ReturnStmt& o = EQ_ASSERT_TYPE(rhs, ReturnStmt);
    return *expr == *o.expr;
}

bool ModuleStmt::equalsImpl(const Node& rhs) const noexcept {
    const ModuleStmt& o = EQ_ASSERT_TYPE(rhs, ModuleStmt);
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

bool ExprStmt::equalsImpl(const Node& rhs) const noexcept {
    const ExprStmt& o = EQ_ASSERT_TYPE(rhs, ExprStmt);
    return *expr == *o.expr;
}



bool IntLiteralPattern::equalsImpl(const Node& rhs) const noexcept {
    const IntLiteralPattern& o = EQ_ASSERT_TYPE(rhs, IntLiteralPattern);
    return value == o.value;
}

bool RealLiteralPattern::equalsImpl(const Node& rhs) const noexcept {
    const RealLiteralPattern& o = EQ_ASSERT_TYPE(rhs, RealLiteralPattern);
    return value == o.value;
}

bool BoolLiteralPattern::equalsImpl(const Node& rhs) const noexcept {
    const BoolLiteralPattern& o = EQ_ASSERT_TYPE(rhs, BoolLiteralPattern);
    return value == o.value;
}

bool StringLiteralPattern::equalsImpl(const Node& rhs) const noexcept {
    const StringLiteralPattern& o = EQ_ASSERT_TYPE(rhs, StringLiteralPattern);
    return value == o.value;
}

bool NilLiteralPattern::equalsImpl(const Node& rhs) const noexcept {
    EQ_ASSERT_TYPE(rhs, NilLiteralPattern);
    return true;
}

bool VoidLiteralPattern::equalsImpl(const Node& rhs) const noexcept {
    EQ_ASSERT_TYPE(rhs, VoidLiteralPattern);
    return true;
}
    
bool BindingPattern::equalsImpl(const Node& rhs) const noexcept {
    const BindingPattern& o = EQ_ASSERT_TYPE(rhs, BindingPattern);
    return name == o.name;
}

bool WildcardPattern::equalsImpl(const Node& rhs) const noexcept {
    EQ_ASSERT_TYPE(rhs, WildcardPattern);
    return true;
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
    if (isExact != o.isExact) {
        return false;
    }
    if (prefix.size() != o.prefix.size()) {
        return false;
    }
    for (size_t i = 0; i < prefix.size(); ++i) {
        if (*prefix[i] != *o.prefix[i]) {
            return false;
        }
    }
    if (suffix.size() != o.suffix.size()) {
        return false;
    }
    for (size_t i = 0; i < suffix.size(); ++i) {
        if (*suffix[i] != *o.suffix[i]) {
            return false;
        }
    }
    return true;
}

} // Spark::FrontEnd
