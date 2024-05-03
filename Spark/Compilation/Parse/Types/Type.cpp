#include "Type.hpp"

namespace Spark {

    std::ostream& operator<<(std::ostream& os, Type type) {
        switch (type) {
            case Type::Nil:
                os << "Nil";
                break;

            case Type::Int:
                os << "Int";
                break;

            case Type::Float:
                os << "Float";
                break;

            case Type::String:
                os << "String";
                break;

            default:
                os.setstate(std::ios_base::failbit);
                break;
        }
        return os;
    }

} // Spark
