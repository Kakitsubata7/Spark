#include "lua_ir_visitor.hpp"

#include "lua_ir.hpp"

namespace Spark::FrontEnd {

void LuaNodeVisitor::visit(LuaNode* node) { }

void LuaNodeVisitor::visit(LuaExpr* expr) { visit(static_cast<LuaNode*>(expr)); }

void LuaNodeVisitor::visit(LuaNumber* n) { visit(static_cast<LuaExpr*>(n)); }

void LuaNodeVisitor::visit(LuaBool* b) { visit(static_cast<LuaExpr*>(b)); }

void LuaNodeVisitor::visit(LuaString* s) { visit(static_cast<LuaExpr*>(s)); }

void LuaNodeVisitor::visit(LuaNil* nil) { visit(static_cast<LuaExpr*>(nil)); }

void LuaNodeVisitor::visit(LuaIdent* ident) { visit(static_cast<LuaExpr*>(ident)); }

void LuaNodeVisitor::visit(LuaCall* call) { visit(static_cast<LuaExpr*>(call)); }

void LuaNodeVisitor::visit(LuaMemberAccess* maccess) { visit(static_cast<LuaExpr*>(maccess)); }

} // Spark::FrontEnd
