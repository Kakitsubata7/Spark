#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "../GC/GCNode.hpp"
#include "../GC/GCPtr.hpp"
#include "Bool.hpp"
#include "Float.hpp"
#include "Integer.hpp"
#include "Type.hpp"

namespace Spark {

class GC;

class Value {

    /* ===== Constructor, Factory Methods & Destructor ===== */

public:
    Value() {
        type = Type::Nil;
        intValue = {};
        ptrValue = {};
    }

    [[nodiscard]]
    static Value makeNil() {
        Value self;
        self.type = Type::Nil;
        return self;
    }

    [[nodiscard]]
    static Value makeInt(Int value = 0) {
        Value self;
        self.type = Type::Integer;
        self.intValue = value;
        return self;
    }

    [[nodiscard]]
    static Value makeFloat(Float value = 0.0) {
        Value self;
        self.type = Type::Float;
        self.floatValue = value;
        return self;
    }

    [[nodiscard]]
    static Value makeBool(Bool value = false) {
        Value self;
        self.type = Type::Boolean;
        self.boolValue = value;
        return self;
    }

    [[nodiscard]]
    static Value makeType(Type value) {
        Value self;
        self.type = Type::Type;
        self.typeValue = value;
        return self;
    }

    [[nodiscard]]
    static Value makeString(GC& gc, const std::string& value = "");

    [[nodiscard]]
    static Value makeArray(GC& gc, const std::vector<Value>& value = {});

    ~Value() = default;



    /* ===== Data ===== */

public:
    Type type;

    union {
        Int intValue{};
        Float floatValue;
        Bool boolValue;
        void* ptrValue;
        Type typeValue;
        GCPtr<std::string> stringPtr;
        GCPtr<std::vector<Value>> arrayPtr;
        GCPtr<std::unordered_set<Value>> setPtr;
        GCPtr<std::unordered_map<Value, Value>> mapPtr;
        GCPtr<std::unordered_map<std::string, Value>> objectPtr;
        GCPtr<void*> threadPtr;
    };

    [[nodiscard]]
    bool isReferenceType() const {
        return static_cast<int>(type) > 0x04;
    }

    [[nodiscard]]
    bool tryGetGCNode(GCNode*& out) const {
        switch (type) {
            case Type::String:
                out = stringPtr.node();
                break;

            case Type::Array:
                out = arrayPtr.node();
                break;

            case Type::Set:
                out = setPtr.node();
                break;

            case Type::Map:
                out = mapPtr.node();
                break;

            case Type::Object:
                out = objectPtr.node();
                break;

            case Type::Thread:
                out = threadPtr.node();
                break;

            default:
                return false;
        }

        return true;
    }

};

} // Spark
