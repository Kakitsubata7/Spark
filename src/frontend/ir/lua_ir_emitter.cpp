#include "lua_ir_emitter.hpp"

#include <cassert>

#include "lua_ir.hpp"

namespace Spark::FrontEnd {

void LuaExprEmitter::visit(LuaNode* node) {
    assert(node != nullptr);

    if (LuaExpr* expr = node->as<LuaExpr>()) {
        visit(expr);
    }
    assert(false && "`LuaNode*` must point to a `LuaExpr` subclass object for `LuaExprEmitter`");
}

void LuaExprEmitter::visit(LuaExpr* expr) {
    assert(expr != nullptr);

    expr->accept(*this);
}

void LuaExprEmitter::visit(LuaNumber* n) {
    assert(n != nullptr);

    _result = n->value;
}

void LuaExprEmitter::visit(LuaBool* b) {
    assert(b != nullptr);

    _result = b->value ? "true" : "false";
}

void LuaExprEmitter::visit(LuaString* s) {
    assert(s != nullptr);

    std::string result;
}

void LuaExprEmitter::visit(LuaNil* nil) {
    assert(nil != nullptr);

    _result = "nil";
}

} // Spark::FrontEnd
