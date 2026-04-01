#pragma once

namespace Spark::FrontEnd {

struct LuaNode;
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

    virtual void visit(LuaNode* node) = 0;
    virtual void visit(LuaNumber* n) = 0;
    virtual void visit(LuaBool* b) = 0;
    virtual void visit(LuaString* s) = 0;
    virtual void visit(LuaNil* nil) = 0;
    virtual void visit(LuaIdent* ident) = 0;
    virtual void visit(LuaCall* call) = 0;
    virtual void visit(LuaMemberAccess* maccess) = 0;
};

} // Spark::FrontEnd
