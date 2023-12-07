#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "Bool8.hpp"
#include "Float64.hpp"
#include "GC/GCPtr.hpp"
#include "Int64.hpp"
#include "Type.hpp"

namespace Spark {

class Value {

    /* ===== Data ===== */

private:
    Type _type;
    Bool8 _isConstant;

    union {
        Int64 integerValue;
        Float64 floatValue;
        Bool8 booleanValue;
        void* pointerValue;
        Type typeValue;
        GCPtr<std::string> stringPtr;
        GCPtr<std::vector<Value>> arrayPtr;
        GCPtr<std::unordered_set<Value>> setPtr;
        GCPtr<std::unordered_map<Value, Value>> mapPtr;
        GCPtr<std::unordered_map<std::string, Value>> objectPtr;
        GCPtr<void*> threadPtr;
        GCPtr<void*> promisePtr;
    };

public:
    [[nodiscard]]
    constexpr Type type() const { return _type; }

    [[nodiscard]]
    constexpr Bool8 isConstant() const { return _isConstant; }



    /* ===== Constructor ===== */

private:
    constexpr Value() : _type(Type::Nil), _isConstant(false), pointerValue(nullptr) { }



    /* ===== Factory Methods ===== */

};

} // Spark
