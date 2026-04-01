#pragma once

#include <string>
#include <vector>

#include "lua_ir_visitor.hpp"

namespace Spark::FrontEnd {

struct LuaNode {
    virtual ~LuaNode() = default;

    virtual void accept(LuaNodeVisitor& visitor) = 0;
};

struct LuaExpr : LuaNode { };

struct LuaNumber : LuaExpr {
    std::string value;

    explicit LuaNumber(std::string value) noexcept
        : value(std::move(value)) { }

    void accept(LuaNodeVisitor& visitor) override {
        visitor.visit(this);
    }
};

struct LuaBool : LuaExpr {
    bool value;

    explicit LuaBool(bool value) noexcept
        : value(value) { }

    void accept(LuaNodeVisitor& visitor) override {
        visitor.visit(this);
    }
};

struct LuaString : LuaExpr {
    std::string value;

    explicit LuaString(std::string value) noexcept
        : value(std::move(value)) { }

    void accept(LuaNodeVisitor& visitor) override {
        visitor.visit(this);
    }
};

struct LuaNil : LuaExpr {
    LuaNil() noexcept = default;

    void accept(LuaNodeVisitor& visitor) override {
        visitor.visit(this);
    }
};

struct LuaIdent : LuaExpr {
    std::string name;

    explicit LuaIdent(std::string name) noexcept
        : name(std::move(name)) { }

    void accept(LuaNodeVisitor& visitor) override {
        visitor.visit(this);
    }
};

struct LuaCall : LuaExpr {
    LuaExpr* callee;
    std::vector<LuaExpr*> args;

    LuaCall(LuaExpr* callee, std::vector<LuaExpr*> args) noexcept
        : callee(callee), args(std::move(args)) { }

    void accept(LuaNodeVisitor& visitor) override {
        visitor.visit(this);
    }
};

struct LuaMemberAccess : LuaExpr {
    LuaExpr* base;
    std::string member;

    LuaMemberAccess(LuaExpr* base, std::string member) noexcept
        : base(base), member(std::move(member)) { }

    void accept(LuaNodeVisitor& visitor) override {
        visitor.visit(this);
    }
};

} // Spark::FrontEnd
