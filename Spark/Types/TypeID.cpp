#include "TypeID.hpp"

namespace Spark {

    std::ostream& operator<<(std::ostream& os, const TypeID typeID) {
        switch (typeID) {
            case TypeID::Nil:
                os << "Nil";
                break;

            case TypeID::Integer:
                os << "Integer";
                break;

            case TypeID::Float:
                os << "Float";
                break;

            case TypeID::Boolean:
                os << "Boolean";
                break;

            case TypeID::CFunction:
                os << "CFunction";
                break;

            case TypeID::TypeID:
                os << "Type";
                break;

            case TypeID::String:
                os << "String";
                break;

            case TypeID::Array:
                os << "Array";
                break;

            case TypeID::Set:
                os << "Set";
                break;

            case TypeID::Map:
                os << "Map";
                break;

            case TypeID::Namespace:
                os << "Namespace";
                break;

            case TypeID::Object:
                os << "Object";
                break;

            default:
                os << "Unimplemented";
                break;
        }
        return os;
    }

} // Spark