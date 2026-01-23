#include "node.hpp"

#include <typeinfo>

/**
 * Checks if `node` is type `type`. If so, return the down-casted reference, otherwise,
 * return false in the scope of the macro usage.
 * @param node Reference to `Node`.
 * @param type `Node` subtype to check.
 */
#define ASSERT_NODE(node, type)         \
({                                      \
    if (typeid(node) != typeid(type)) { \
        return false;                   \
    }                                   \
    static_cast<const type&>(node); /* NOLINT(cppcoreguidelines-pro-type-static-cast-downcast) */ \
})

/**
 * Checks if two `Node*` pointers stores instances that are equal.
 * @param lhs Left-hand-side `Node` pointer.
 * @param rhs Right-hand-side `Node` pointer.
 * @return True if two pointers have `Node` instances that are equal, false otherwise.
 */
bool ptrEq(const Spark::FrontEnd::Node* lhs, const Spark::FrontEnd::Node* rhs) {
    if (lhs == rhs) {
        return true;
    }
    if (lhs == nullptr || rhs == nullptr) {
        return false;
    }
    return lhs->equalsStructurally(*rhs);
}

/**
 * Check if two `std::vector<Node*>` stores `Node` instances that are equal.
 * @tparam T `Node` subtype.
 * @param lhs Left-hand-side vector.
 * @param rhs Right-hand-side vector.
 * @return True if the two vectors have pointers to `Node` instances that are equal, false
 *         otherwise.
 */
template <typename T>
bool ptrVecEq(const std::vector<T*>& lhs, const std::vector<T*>& rhs) {
    if (lhs.size() != rhs.size()) {
        return false;
    }
    for (size_t i = 0; i < lhs.size(); ++i) {
        if (!lhs[i]->equalsStructurally(*rhs[i])) {
            return false;
        }
    }
    return true;
}

namespace Spark::FrontEnd {

bool Node::operator==(const Node& rhs) const noexcept {
    return equalsStructurally(rhs) && start == rhs.start && end == rhs.end;
}

bool Node::operator!=(const Node& rhs) const noexcept {
    return !(*this == rhs);
}

bool Node::equalsStructurally(const Node& rhs) const noexcept {
    if (this == &rhs) {
        return true;
    }
    return equalsImpl(rhs);
}



bool VarModifier::equalsImpl(const Node& rhs) const noexcept {
    const VarModifier& o = ASSERT_NODE(rhs, VarModifier);
    return kind == o.kind && isImmutable == o.isImmutable && optionality == o.optionality;
}

bool FnParam::equalsImpl(const Node& rhs) const noexcept {
    const FnParam& o = ASSERT_NODE(rhs, FnParam);
    return ptrEq(mod, o.mod) && ptrEq(pattern, o.pattern) &&
           ptrEq(type, o.type) && ptrEq(def, o.def);
}

bool FnCapture::equalsImpl(const Node& rhs) const noexcept {
    const FnCapture& o = ASSERT_NODE(rhs, FnCapture);
    return ptrEq(mod, o.mod) && ptrEq(pattern, o.pattern);
}

bool FnCaptureClause::equalsImpl(const Node& rhs) const noexcept {
    const FnCaptureClause& o = ASSERT_NODE(rhs, FnCaptureClause);
    return ptrVecEq(captures, o.captures) && hasRest == o.hasRest &&
           ptrEq(restMod, o.restMod);
}

bool FnReturn::equalsImpl(const Node& rhs) const noexcept {
    const FnReturn& o = ASSERT_NODE(rhs, FnReturn);
    return kind == o.kind && ptrEq(type, o.type);
}

bool PathSeg::equalsImpl(const Node& rhs) const noexcept {
    const PathSeg& o = ASSERT_NODE(rhs, PathSeg);
    return name == o.name && ptrVecEq(generics, o.generics);
}

bool Path::equalsImpl(const Node& rhs) const noexcept {
    const Path& o = ASSERT_NODE(rhs, Path);
    return ptrVecEq(segs, o.segs);
}



bool LambdaExpr::equalsImpl(const Node& rhs) const noexcept {
    const LambdaExpr& o = ASSERT_NODE(rhs, LambdaExpr);
    return isImmutable == o.isImmutable && ptrVecEq(params, o.params) &&
           ptrEq(captureClause, o.captureClause) &&
           ptrVecEq(returns, o.returns) && isThrowing == o.isThrowing &&
           ptrEq(throwExpr, o.throwExpr) && ptrEq(body, o.body);
}

bool IfThenExpr::equalsImpl(const Node& rhs) const noexcept {
    const IfThenExpr& o = ASSERT_NODE(rhs, IfThenExpr);
    return ptrEq(condition, o.condition) && ptrEq(thenExpr, o.thenExpr) &&
           ptrEq(elseExpr, o.elseExpr);
}

bool TryElseExpr::equalsImpl(const Node& rhs) const noexcept {
    const TryElseExpr& o = ASSERT_NODE(rhs, TryElseExpr);
    return ptrEq(tryExpr, o.tryExpr) && ptrEq(elseExpr, o.elseExpr);
}

bool MatchCase::equalsImpl(const Node& rhs) const noexcept {
    const MatchCase& o = ASSERT_NODE(rhs, MatchCase);
    return ptrEq(pattern, o.pattern) && ptrEq(guard, o.guard) &&
           ptrEq(body, o.body);
}

bool MatchExpr::equalsImpl(const Node& rhs) const noexcept {
    const MatchExpr& o = ASSERT_NODE(rhs, MatchExpr);
    return ptrEq(scrutinee, o.scrutinee) && ptrVecEq(cases, o.cases);
}

bool CatchClause::equalsImpl(const Node& rhs) const noexcept {
    const CatchClause& o = ASSERT_NODE(rhs, CatchClause);
    return ptrEq(pattern, o.pattern) && ptrEq(guard, o.guard) &&
           ptrEq(body, o.body);
}

bool TryCatchExpr::equalsImpl(const Node& rhs) const noexcept {
    const TryCatchExpr& o = ASSERT_NODE(rhs, TryCatchExpr);
    return ptrEq(expr, o.expr) && ptrVecEq(catches, o.catches);
}

bool ThrowExpr::equalsImpl(const Node& rhs) const noexcept {
    const ThrowExpr& o = ASSERT_NODE(rhs, ThrowExpr);
    return ptrEq(expr, o.expr);
}

bool BlockExpr::equalsImpl(const Node& rhs) const noexcept {
    const BlockExpr& o = ASSERT_NODE(rhs, BlockExpr);
    return ptrVecEq(nodes, o.nodes);
}

bool IsExpr::equalsImpl(const Node& rhs) const noexcept {
    const IsExpr& o = ASSERT_NODE(rhs, IsExpr);
    return ptrEq(expr, o.expr) && ptrEq(type, o.type);
}

bool AsExpr::equalsImpl(const Node& rhs) const noexcept {
    const AsExpr& o = ASSERT_NODE(rhs, AsExpr);
    return ptrEq(expr, o.expr) && ptrEq(pattern, o.pattern) &&
           ptrEq(type, o.type);
}

bool BinaryExpr::equalsImpl(const Node& rhs) const noexcept {
    const BinaryExpr& o = ASSERT_NODE(rhs, BinaryExpr);
    return op == o.op && ptrEq(lhs, o.lhs) && ptrEq(this->rhs, o.rhs);
}

bool PrefixExpr::equalsImpl(const Node& rhs) const noexcept {
    const PrefixExpr& o = ASSERT_NODE(rhs, PrefixExpr);
    return op == o.op && ptrEq(expr, o.expr);
}

bool PostfixExpr::equalsImpl(const Node& rhs) const noexcept {
    const PostfixExpr& o = ASSERT_NODE(rhs, PostfixExpr);
    return op == o.op && ptrEq(expr, o.expr);
}

bool MemberAccessExpr::equalsImpl(const Node& rhs) const noexcept {
    const MemberAccessExpr& o = ASSERT_NODE(rhs, MemberAccessExpr);
    return ptrEq(base, o.base) && member == o.member;
}

bool CallArg::equalsImpl(const Node& rhs) const noexcept {
    const CallArg& o = ASSERT_NODE(rhs, CallArg);
    return name == o.name && ptrEq(expr, o.expr);
}

bool CallExpr::equalsImpl(const Node& rhs) const noexcept {
    const CallExpr& o = ASSERT_NODE(rhs, CallExpr);
    return ptrEq(callee, o.callee) && ptrVecEq(args, o.args);
}

bool SubscriptExpr::equalsImpl(const Node& rhs) const noexcept {
    const SubscriptExpr& o = ASSERT_NODE(rhs, SubscriptExpr);
    return ptrEq(base, o.base) && ptrVecEq(indices, o.indices);
}

bool LiteralExpr::equalsImpl(const Node& rhs) const noexcept {
    const LiteralExpr& o = ASSERT_NODE(rhs, LiteralExpr);
    return literal == o.literal;
}

bool NameExpr::equalsImpl(const Node& rhs) const noexcept {
    const NameExpr& o = ASSERT_NODE(rhs, NameExpr);
    return name == o.name;
}

bool GlobalAccessExpr::equalsImpl(const Node& rhs) const noexcept {
    const GlobalAccessExpr& o = ASSERT_NODE(rhs, GlobalAccessExpr);
    return name == o.name;
}

bool UpvalueExpr::equalsImpl(const Node& rhs) const noexcept {
    const UpvalueExpr& o = ASSERT_NODE(rhs, UpvalueExpr);
    return level == o.level && name == o.name;
}

bool TupleExpr::equalsImpl(const Node& rhs) const noexcept {
    const TupleExpr& o = ASSERT_NODE(rhs, TupleExpr);
    return ptrVecEq(exprs, o.exprs);
}

bool CollectionExpr::equalsImpl(const Node& rhs) const noexcept {
    const CollectionExpr& o = ASSERT_NODE(rhs, CollectionExpr);
    return ptrVecEq(exprs, o.exprs);
}

bool TypeofExpr::equalsImpl(const Node& rhs) const noexcept {
    const TypeofExpr& o = ASSERT_NODE(rhs, TypeofExpr);
    return ptrEq(expr, o.expr);
}



bool VarDefStmt::equalsImpl(const Node& rhs) const noexcept {
    const VarDefStmt& o = ASSERT_NODE(rhs, VarDefStmt);
    return ptrEq(mod, o.mod) && ptrEq(pattern, o.pattern) &&
           ptrEq(type, o.type) && ptrEq(this->rhs, o.rhs);
}

bool FnDefStmt::equalsImpl(const Node& rhs) const noexcept {
    const FnDefStmt& o = ASSERT_NODE(rhs, FnDefStmt);
    return isImmutable == o.isImmutable && name == o.name &&
           generics == o.generics && ptrVecEq(params, o.params) &&
           ptrEq(captureClause, o.captureClause) &&
           ptrVecEq(returns, o.returns) && isThrowing == o.isThrowing &&
           ptrEq(throwExpr, o.throwExpr) && ptrEq(body, o.body);
}

bool TypeDefStmt::equalsImpl(const Node& rhs) const noexcept {
    const TypeDefStmt& o = ASSERT_NODE(rhs, TypeDefStmt);
    return kind == o.kind && isImmutable == o.isImmutable && name == o.name &&
           generics == o.generics && ptrVecEq(bases, o.bases) &&
           ptrEq(body, o.body);
}

bool CaseDefStmt::equalsImpl(const Node& rhs) const noexcept {
    const CaseDefStmt& o = ASSERT_NODE(rhs, CaseDefStmt);
    return name == o.name && ptrEq(val, o.val);
}

bool AssignStmt::equalsImpl(const Node& rhs) const noexcept {
    const AssignStmt& o = ASSERT_NODE(rhs, AssignStmt);
    return op == o.op && ptrEq(lhs, o.lhs) && ptrEq(this->rhs, o.rhs);
}

bool IfStmt::equalsImpl(const Node& rhs) const noexcept {
    const IfStmt& o = ASSERT_NODE(rhs, IfStmt);
    return ptrEq(condition, o.condition) && ptrEq(thenBody, o.thenBody) &&
           ptrEq(elseBody, o.elseBody);
}

bool WhileStmt::equalsImpl(const Node& rhs) const noexcept {
    const WhileStmt& o = ASSERT_NODE(rhs, WhileStmt);
    return ptrEq(condition, o.condition) && ptrEq(body, o.body);
}

bool DoWhileStmt::equalsImpl(const Node& rhs) const noexcept {
    const DoWhileStmt& o = ASSERT_NODE(rhs, DoWhileStmt);
    return ptrEq(body, o.body) && ptrEq(condition, o.condition);
}

bool ForStmt::equalsImpl(const Node& rhs) const noexcept {
    const ForStmt& o = ASSERT_NODE(rhs, ForStmt);
    return ptrEq(iterator, o.iterator) && ptrEq(range, o.range) &&
           ptrEq(body, o.body);
}

bool BreakStmt::equalsImpl(const Node& rhs) const noexcept {
    ASSERT_NODE(rhs, BreakStmt);
    return true;
}

bool ContinueStmt::equalsImpl(const Node& rhs) const noexcept {
    ASSERT_NODE(rhs, ContinueStmt);
    return true;
}

bool ReturnStmt::equalsImpl(const Node& rhs) const noexcept {
    const ReturnStmt& o = ASSERT_NODE(rhs, ReturnStmt);
    return ptrEq(expr, o.expr);
}

bool ModuleStmt::equalsImpl(const Node& rhs) const noexcept {
    const ModuleStmt& o = ASSERT_NODE(rhs, ModuleStmt);
    return ptrEq(path, o.path) && ptrEq(body, o.body);
}

bool ExportStmt::equalsImpl(const Node& rhs) const noexcept {
    const ExportStmt& o = ASSERT_NODE(rhs, ExportStmt);
    return ptrEq(stmt, o.stmt);
}

bool ImportItem::equalsImpl(const Node& rhs) const noexcept {
    const ImportItem& o = ASSERT_NODE(rhs, ImportItem);
    return ptrEq(path, o.path) && as == o.as;
}

bool ImportStmt::equalsImpl(const Node& rhs) const noexcept {
    const ImportStmt& o = ASSERT_NODE(rhs, ImportStmt);
    return ptrEq(from, o.from) && ptrVecEq(imports, o.imports);
}

bool ImportAllStmt::equalsImpl(const Node& rhs) const noexcept {
    const ImportAllStmt& o = ASSERT_NODE(rhs, ImportAllStmt);
    return ptrEq(from, o.from);
}

bool UndefineStmt::equalsImpl(const Node& rhs) const noexcept {
    const UndefineStmt& o = ASSERT_NODE(rhs, UndefineStmt);
    return ptrEq(path, o.path);
}



bool LiteralPattern::equalsImpl(const Node& rhs) const noexcept {
    const LiteralPattern& o = ASSERT_NODE(rhs, LiteralPattern);
    return literal == o.literal;
}

bool BindingPattern::equalsImpl(const Node& rhs) const noexcept {
    const BindingPattern& o = ASSERT_NODE(rhs, BindingPattern);
    return name == o.name;
}

bool TuplePattern::equalsImpl(const Node& rhs) const noexcept {
    const TuplePattern& o = ASSERT_NODE(rhs, TuplePattern);
    return ptrVecEq(patterns, o.patterns);
}

bool CollectionPattern::equalsImpl(const Node& rhs) const noexcept {
    const CollectionPattern& o = ASSERT_NODE(rhs, CollectionPattern);
    return ptrVecEq(prefix, o.prefix) && ptrVecEq(suffix, o.suffix) &&
           isExact == o.isExact;
}

bool RecordPatternField::equalsImpl(const Node& rhs) const noexcept {
    const RecordPatternField& o = ASSERT_NODE(rhs, RecordPatternField);
    return label == o.label && ptrEq(pattern, o.pattern);
}

bool RecordPattern::equalsImpl(const Node& rhs) const noexcept {
    const RecordPattern& o = ASSERT_NODE(rhs, RecordPattern);
    return ptrVecEq(fields, o.fields);
}

} // Spark::FrontEnd
