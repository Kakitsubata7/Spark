#pragma once

namespace Spark::FrontEnd {

struct LuaNode;
struct LuaExpr;
struct LuaNumber;
struct LuaBool;
struct LuaString;
struct LuaNil;
struct LuaIdent;
struct LuaCall;
struct LuaMemberAccess;

class LuaNodeVisitor {
public:
    virtual ~LuaNodeVisitor() = default;

    virtual void visit(LuaNode* node);
    virtual void visit(LuaExpr* expr);
    virtual void visit(LuaNumber* n);
    virtual void visit(LuaBool* b);
    virtual void visit(LuaString* s);
    virtual void visit(LuaNil* nil);
    virtual void visit(LuaIdent* ident);
    virtual void visit(LuaCall* call);
    virtual void visit(LuaMemberAccess* maccess);
};

} // Spark::FrontEnd
