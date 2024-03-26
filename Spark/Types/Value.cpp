#include "Value.hpp"

#include "../GC/GC.hpp"

namespace Spark {

    /* ===== Factory Methods ===== */

    Value Value::makeString(Spark::GC& gc, const std::string& value) {
        Value self;
        self.type = Type::String;
        self.nodePtr = gc.allocate<std::string>(value);
        return self;
    }

    Value Value::makeArray(Spark::GC& gc, const std::vector<Value>& value) {
        Value self;
        self.type = Type::Array;
        self.nodePtr = gc.allocate<std::vector<Value>>(value);
        return self;
    }



    /* ===== Operator ===== */

    std::ostream& operator<<(std::ostream& os, const Value& value) {
        switch (value.type) {
            case Type::Nil:
                os << "nil";
                break;

            case Type::Integer:
                os << value.intValue;
                break;

            case Type::Float:
                os << value.floatValue;
                break;

            case Type::Boolean:
                os << (value.boolValue ? "true" : "false");
                break;

            case Type::Type:
                os << value.typeValue;
                break;

            case Type::String:
                os << value.nodePtr->getData<std::string>();
                break;

            default:
                os << "unimplemented";
                break;
        }
        return os;
    }

} // Spark
