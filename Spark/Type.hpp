#pragma once

#include <cstdint>

namespace Spark {

    enum class Type : uint8_t {
        Nil,
        Integer,
        Float,
        Boolean,
        Pointer,
        Type,
        String,
        Array,
        Set,
        Map,
        Object,
        Function,
        Thread,
        Promise
    };

    std::ostream& operator<<(std::ostream& os, const Type type) {
        switch (type) {
            case Type::Nil:
                os << "Nil";
                break;

            case Type::Integer:
                os << "Integer";
                break;

            case Type::Float:
                os << "Float";
                break;

            case Type::Boolean:
                os << "Boolean";
                break;

            case Type::Pointer:
                os << "Pointer";
                break;

            case Type::Type:
                os << "Type";
                break;

            case Type::String:
                os << "String";
                break;

            case Type::Array:
                os << "Array";
                break;

            case Type::Set:
                os << "Set";
                break;

            case Type::Map:
                os << "Map";
                break;

            case Type::Object:
                os << "Object";
                break;

            case Type::Function:
                os << "Function";
                break;

            case Type::Thread:
                os << "Thread";
                break;

            case Type::Promise:
                os << "Promise";
                break;
        }
        return os;
    }

} // Spark
