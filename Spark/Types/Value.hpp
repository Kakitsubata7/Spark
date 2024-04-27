#pragma once

#include <functional>
#include <ostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "../GC/GCNode.hpp"
#include "Bool.hpp"
#include "CFunction.hpp"
#include "Float.hpp"
#include "Int.hpp"
#include "TypeID.hpp"

namespace Spark {
    class Value;
} // Spark

namespace std {
    template<>
    struct hash<Spark::Value> {
        std::size_t operator()(const Spark::Value& value) const;
    };
} // std

namespace Spark {

class Closure;
class GC;
class GCBase;
class Thread;
class Namespace;

/**
 * The size of this type should be (8 bytes + pointer size + alignment); at least 16 bytes.
 */
class Value {

    /* ===== Constructor, Factory Methods & Destructor ===== */

public:
    constexpr Value() : type(TypeID::Nil) { }

    [[nodiscard]]
    constexpr static Value makeNil() {
        return Value();
    }

    [[nodiscard]]
    constexpr static Value makeInt(Int i = 0) {
        Value self;
        self.type = TypeID::Integer;
        self.intValue = i;
        return self;
    }

    [[nodiscard]]
    constexpr static Value makeFloat(Float f = 0.0) {
        Value self;
        self.type = TypeID::Float;
        self.floatValue = f;
        return self;
    }

    [[nodiscard]]
    constexpr static Value makeBool(Bool b = false) {
        Value self;
        self.type = TypeID::Boolean;
        self.boolValue = b;
        return self;
    }

    [[nodiscard]]
    constexpr static Value makeCFunction(CFunction cFunc) {
        Value self;
        self.type = TypeID::CFunction;
        self.cFunc = cFunc;
        return self;
    }

    [[nodiscard]]
    constexpr static Value makeType(TypeID typeID) {
        Value self;
        self.type = TypeID::TypeID;
        self.typeValue = typeID;
        return self;
    }

    [[nodiscard]]
    static Value makeString(GC& gc, const std::string& value = "");

    [[nodiscard]]
    static Value makeString(GCBase& gc, const std::string& str = "");

    [[nodiscard]]
    static Value makeArray(GC& gc, const std::vector<Value>& value = {});

    [[nodiscard]]
    static Value makeSet(GC& gc, const std::unordered_set<Value>& value = {});

    [[nodiscard]]
    static Value makeMap(GC& gc, const std::unordered_map<Value, Value>& value = {});

    [[nodiscard]]
    static Value makeClosure(GC& gc, const Closure& value);

    [[nodiscard]]
    static Value makeNamespace(GC& gc, const Namespace& value);

    [[nodiscard]]
    static Value makeNamespace(GC& gc);

    ~Value() = default;



    /* ===== Data ===== */

public:
    TypeID type;

    union {
        Int intValue;
        Float floatValue;
        Bool boolValue;
        void* ptrValue{};
        CFunction cFunc;
        TypeID typeValue;
        GCNode* nodePtr;
    };

    [[nodiscard]]
    bool isReferenceType() const {
        return static_cast<int>(type) > static_cast<int>(TypeID::TypeID);
    }

    [[nodiscard]]
    bool isCallable() const {
        return (type == TypeID::CFunction) || (type == TypeID::Function) || (type == TypeID::Closure);
    }



    /* ===== Operators ===== */

public:
    friend std::ostream& operator<<(std::ostream& os, const Value& value);

    Value operator+(const Value& other) const;
    Value operator-(const Value& other) const;
    Value operator*(const Value& other) const;
    Value operator/(const Value& other) const;
    Value operator%(const Value& other) const;

    bool operator==(const Value& other) const;
    bool operator!=(const Value& other) const;

    bool operator<(const Value& other) const;
    bool operator<=(const Value& other) const;
    bool operator>(const Value& other) const;
    bool operator>=(const Value& other) const;

    bool operator&&(const Value& other) const;
    bool operator||(const Value& other) const;
    [[nodiscard]] static bool exclusiveOr(const Value& lhs, const Value& rhs);
    bool operator!() const;

};

} // Spark
