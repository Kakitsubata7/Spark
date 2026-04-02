#pragma once

#include <string>

#include "lua_ir_visitor.hpp"

namespace Spark::FrontEnd {

class LuaExprEmitter : public LuaNodeVisitor {
private:
    std::string _result;

public:
    [[nodiscard]]
    const std::string& result() const noexcept { return _result; }

    [[nodiscard]]
    std::string& result() noexcept { return _result; }

    void visit(LuaNode* node) override;
    void visit(LuaExpr* expr) override;
    void visit(LuaNumber* n) override;
    void visit(LuaBool* b) override;
    void visit(LuaString* s) override;
    void visit(LuaNil* nil) override;

    [[nodiscard]]
    const std::string& emit(LuaNode* node);
};

} // Spark::FrontEnd
