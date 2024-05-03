#pragma once

#include "Value.hpp"

namespace Spark {

    /* ===== Constructor & Destructor ===== */

    Value::Value(const Value& other) {
        type = other.type;
        switch (other.type) {
            case Type::Int:
                intValue = other.intValue;
                break;

            case Type::Float:
                floatValue = other.floatValue;
                break;

            case Type::String:
                strValue = other.strValue;
                break;

            default:
                break;
        }
    }

    Value::Value(Value&& other) noexcept {
        type = other.type;
        switch (other.type) {
            case Type::Int:
                intValue = other.intValue;
                break;

            case Type::Float:
                floatValue = other.floatValue;
                break;

            case Type::String:
                strValue = std::move(other.strValue);
                break;

            default:
                break;
        }
    }

    Value::~Value() {
        switch (type) {
            case Type::String:
                strValue.~basic_string();
                break;

            default:
                break;
        }
    }

} // Spark
