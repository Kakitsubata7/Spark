#include "Type.hpp"

namespace Spark {

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

            case Type::CFunction:
                os << "CFunction";
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

            case Type::Namespace:
                os << "Namespace";
                break;

            case Type::Object:
                os << "Object";
                break;

            default:
                os << "Unimplemented";
                break;
        }
        return os;
    }

} // Spark