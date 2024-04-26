#include "Value.hpp"

#include <cmath>
#include <iomanip>
#include <sstream>

#include "../GC/GC.hpp"
#include "../GC/GCBase.hpp"
#include "Closure.hpp"
#include "Namespace.hpp"

namespace std {
    std::size_t hash<Spark::Value>::operator()(const Spark::Value& value) const  {
        using namespace Spark;

        std::size_t h1 = std::hash<TypeID>()(value.type);
        std::size_t h2;
        switch (value.type) {
            case TypeID::Nil:
                h2 = 0;
                break;

            case TypeID::Integer:
                h2 = std::hash<Int>()(value.intValue);
                break;

            case TypeID::Float:
                h2 = std::hash<Float>()(value.floatValue);
                break;

            case TypeID::Boolean:
                h2 = std::hash<Bool>()(value.boolValue);
                break;

            case TypeID::CFunction:
                h2 = std::hash<CFunction>()(value.cFunc);
                break;

            case TypeID::TypeID:
                h2 = std::hash<TypeID>()(value.typeValue);
                break;

            default:
                h2 = std::hash<GCNode*>()(value.node);
                break;
        }
        return h1 ^ (h2 << 1);
    }
} // std

namespace Spark {

    /* ===== Constructor & Factory Methods ===== */

    Value Value::makeString(GC& gc, const std::string& value) {
        Value self;
        self.type = TypeID::String;
        self.node = gc.allocate<std::string>(value);
        return self;
    }

    Value Value::makeString(GCBase& gc, const std::string& str) {
        Value self;
        self.type == TypeID::String;
        return self;
    }

    Value Value::makeArray(GC& gc, const std::vector<Value>& value) {
        Value self;
        self.type = TypeID::Array;
        self.node = gc.allocate<std::vector<Value>>(value);
        return self;
    }

    Value Value::makeSet(GC& gc, const std::unordered_set<Value>& value) {
        Value self;
        self.type = TypeID::Set;
        self.node = gc.allocate<std::unordered_set<Value>>(value);
        return self;
    }

    Value Value::makeMap(GC& gc, const std::unordered_map<Value, Value>& value) {
        Value self;
        self.type = TypeID::Map;
        self.node = gc.allocate<std::unordered_map<Value, Value>>(value);
        return self;
    }

    Value Value::makeClosure(GC& gc, const Closure& value) {
        Value self;
        self.type = TypeID::Closure;
        self.node = gc.allocate<Closure>(value);
        return self;
    }

    Value Value::makeNamespace(GC& gc, const Namespace& value) {
        Value self;
        self.type = TypeID::Namespace;
        self.node = gc.allocate<Namespace>(value);
        return self;
    }

    Value Value::makeNamespace(GC& gc) {
        return makeNamespace(gc, {});
    }



    /* ===== Operators ===== */

    std::ostream& operator<<(std::ostream& os, const Value& value) {
        switch (value.type) {
            case TypeID::Nil:
                os << "nil";
                break;

            case TypeID::Integer:
                os << value.intValue;
                break;

            case TypeID::Float:
                os << value.floatValue;
                break;

            case TypeID::Boolean:
                os << (value.boolValue ? "true" : "false");
                break;

            case TypeID::CFunction: {
                os << "<CFunction: 0x"
                   << std::hex
                   << std::setw(2)
                   << std::setfill('0')
                   << reinterpret_cast<uintptr_t>(value.cFunc)
                   << ">"
                   << std::dec;
            }
                break;

            case TypeID::TypeID:
                os << value.typeValue;
                break;

            case TypeID::String:
                os << value.node->data<std::string>();
                break;

            case TypeID::Array: {
                const std::vector<Value>& vec = value.node->data<std::vector<Value>>();
                os << "[";
                for (size_t i = 0; i < vec.size(); i++) {
                    os << vec[i];
                    if (i != vec.size() - 1)
                        os << ", ";
                }
                os << "]";
            }
                break;

            case TypeID::Function: {
                os << "<Function: "
                   << std::hex
                   << std::setw(2)
                   << std::setfill('0')
                   << reinterpret_cast<uintptr_t>(value.node)
                   << ">"
                   << std::dec;
            }
                break;

            default: {
                os << "<" << value.type <<": 0x"
                   << std::hex
                   << std::setw(2)
                   << std::setfill('0')
                   << reinterpret_cast<uintptr_t>(value.node)
                   << ">"
                   << std::dec;
            }
                break;
        }
        return os;
    }

    Value Value::operator+(const Value& other) const {
        switch (type) {
            case TypeID::Integer: {
                switch (other.type) {
                    case TypeID::Integer:
                        return makeInt(intValue + other.intValue);

                    case TypeID::Float:
                        return makeFloat(static_cast<Float>(intValue) + other.floatValue);

                    default:
                        break;
                }
            }
                break;

            case TypeID::Float: {
                switch (other.type) {
                    case TypeID::Integer:
                        return makeFloat(floatValue + static_cast<Float>(other.intValue));

                    case TypeID::Float:
                        return makeFloat(floatValue + other.floatValue);

                    default:
                        break;
                }
            }
                break;

            case TypeID::String: {
                std::ostringstream ss;
                ss << node->data<std::string>() << other;
                return makeString(node->getGC(), ss.str());
            }
                break;

            default:
                break;
        }

        std::ostringstream ss;
        ss << "Operator '+' is not supported between '" << type << "' and '" << other.type << "'.";
        throw std::runtime_error(ss.str());
    }

    Value Value::operator-(const Value& other) const {
        switch (type) {
            case TypeID::Integer: {
                switch (other.type) {
                    case TypeID::Integer:
                        return makeInt(intValue - other.intValue);

                    case TypeID::Float:
                        return makeFloat(static_cast<Float>(intValue) - other.floatValue);

                    default:
                        break;
                }
            }
                break;

            case TypeID::Float: {
                switch (other.type) {
                    case TypeID::Integer:
                        return makeFloat(floatValue - static_cast<Float>(other.intValue));

                    case TypeID::Float:
                        return makeFloat(floatValue - other.floatValue);

                    default:
                        break;
                }
            }
                break;

            default:
                break;
        }

        std::ostringstream ss;
        ss << "Operator '-' is not supported between '" << type << "' and '" << other.type << "'.";
        throw std::runtime_error(ss.str());
    }

    Value Value::operator*(const Value& other) const {
        switch (type) {
            case TypeID::Integer: {
                switch (other.type) {
                    case TypeID::Integer:
                        return makeInt(intValue * other.intValue);

                    case TypeID::Float:
                        return makeFloat(static_cast<Float>(intValue) * other.floatValue);

                    default:
                        break;
                }
            }
                break;

            case TypeID::Float: {
                switch (other.type) {
                    case TypeID::Integer:
                        return makeFloat(floatValue * static_cast<Float>(other.intValue));

                    case TypeID::Float:
                        return makeFloat(floatValue * other.floatValue);

                    default:
                        break;
                }
            }
                break;

            default:
                break;
        }

        std::ostringstream ss;
        ss << "Operator '*' is not supported between '" << type << "' and '" << other.type << "'.";
        throw std::runtime_error(ss.str());
    }

    Value Value::operator/(const Value& other) const {
        switch (type) {
            case TypeID::Integer: {
                switch (other.type) {
                    case TypeID::Integer:
                        return makeInt(intValue / other.intValue);

                    case TypeID::Float:
                        return makeFloat(static_cast<Float>(intValue) / other.floatValue);

                    default:
                        break;
                }
            }
                break;

            case TypeID::Float: {
                switch (other.type) {
                    case TypeID::Integer:
                        return makeFloat(floatValue / static_cast<Float>(other.intValue));

                    case TypeID::Float:
                        return makeFloat(floatValue / other.floatValue);

                    default:
                        break;
                }
            }
                break;

            default:
                break;
        }

        std::ostringstream ss;
        ss << "Operator '/' is not supported between '" << type << "' and '" << other.type << "'.";
        throw std::runtime_error(ss.str());
    }

    Value Value::operator%(const Value& other) const {
        switch (type) {
            case TypeID::Integer: {
                switch (other.type) {
                    case TypeID::Integer:
                        return makeInt(intValue % other.intValue);

                    case TypeID::Float:
                        return makeFloat(std::fmod(static_cast<Float>(intValue), other.floatValue));

                    default:
                        break;
                }
            }
                break;

            case TypeID::Float: {
                switch (other.type) {
                    case TypeID::Integer:
                        return makeFloat(std::fmod(floatValue, static_cast<Float>(other.intValue)));

                    case TypeID::Float:
                        return makeFloat(std::fmod(floatValue, other.floatValue));

                    default:
                        break;
                }
            }
                break;

            default:
                break;
        }

        std::ostringstream ss;
        ss << "Operator '%' is not supported between '" << type << "' and '" << other.type << "'.";
        throw std::runtime_error(ss.str());
    }

    bool Value::operator==(const Value& other) const {
        // Check equality for numerical types
        switch (type) {
            case TypeID::Integer: {
                switch (other.type) {
                    case TypeID::Integer:
                        return intValue == other.intValue;

                    case TypeID::Float:
                        return static_cast<Float>(intValue) == other.floatValue;

                    default:
                        break;
                }
            }
                break;

            case TypeID::Float: {
                switch (other.type) {
                    case TypeID::Integer:
                        return floatValue == static_cast<Float>(other.intValue);

                    case TypeID::Float:
                        return floatValue == other.floatValue;

                    default:
                        break;
                }
            }
                break;

            default:
                break;
        }

        // Check equality for other types
        if (type != other.type)
            return false;
        switch (type) {
            case TypeID::Nil:
                return true;

            case TypeID::Boolean:
                return boolValue == other.boolValue;

            case TypeID::CFunction:
                return cFunc == other.cFunc;

            case TypeID::TypeID:
                return typeValue == other.typeValue;

            case TypeID::String:
                return node->data<std::string>() == other.node->data<std::string>();

            case TypeID::Array:
                return node->data<std::vector<Value>>() == other.node->data<std::vector<Value>>();

            case TypeID::Set:
                return node->data<std::unordered_set<Value>>() == other.node->data<std::unordered_set<Value>>();

            case TypeID::Map:
                return node->data<std::unordered_map<Value, Value>>() ==
                       other.node->data<std::unordered_map<Value, Value>>();

            default:
                return node == other.node;
        }
    }

    bool Value::operator!=(const Value& other) const {
        return !(*this == other);
    }

    bool Value::operator<(const Value& other) const {
        switch (type) {
            case TypeID::Integer: {
                switch (other.type) {
                    case TypeID::Integer:
                        return intValue < other.intValue;

                    case TypeID::Float:
                        return static_cast<Float>(intValue) < other.floatValue;

                    default:
                        break;
                }
            }
                break;

            case TypeID::Float: {
                switch (other.type) {
                    case TypeID::Integer:
                        return floatValue < static_cast<Float>(other.intValue);

                    case TypeID::Float:
                        return floatValue < other.floatValue;

                    default:
                        break;
                }
            }
                break;

            default:
                break;
        }

        std::ostringstream ss;
        ss << "Operator '<' is not supported between '" << type << "' and '" << other.type << "'.";
        throw std::runtime_error(ss.str());
    }

    bool Value::operator<=(const Value& other) const {
        switch (type) {
            case TypeID::Integer: {
                switch (other.type) {
                    case TypeID::Integer:
                        return intValue <= other.intValue;

                    case TypeID::Float:
                        return static_cast<Float>(intValue) <= other.floatValue;

                    default:
                        break;
                }
            }
                break;

            case TypeID::Float: {
                switch (other.type) {
                    case TypeID::Integer:
                        return floatValue <= static_cast<Float>(other.intValue);

                    case TypeID::Float:
                        return floatValue <= other.floatValue;

                    default:
                        break;
                }
            }
                break;

            default:
                break;
        }

        std::ostringstream ss;
        ss << "Operator '<=' is not supported between '" << type << "' and '" << other.type << "'.";
        throw std::runtime_error(ss.str());
    }

    bool Value::operator>(const Value& other) const {
        switch (type) {
            case TypeID::Integer: {
                switch (other.type) {
                    case TypeID::Integer:
                        return intValue > other.intValue;

                    case TypeID::Float:
                        return static_cast<Float>(intValue) > other.floatValue;

                    default:
                        break;
                }
            }
                break;

            case TypeID::Float: {
                switch (other.type) {
                    case TypeID::Integer:
                        return floatValue > static_cast<Float>(other.intValue);

                    case TypeID::Float:
                        return floatValue > other.floatValue;

                    default:
                        break;
                }
            }
                break;

            default:
                break;
        }

        std::ostringstream ss;
        ss << "Operator '>' is not supported between '" << type << "' and '" << other.type << "'.";
        throw std::runtime_error(ss.str());
    }

    bool Value::operator>=(const Value& other) const {
        switch (type) {
            case TypeID::Integer: {
                switch (other.type) {
                    case TypeID::Integer:
                        return intValue >= other.intValue;

                    case TypeID::Float:
                        return static_cast<Float>(intValue) >= other.floatValue;

                    default:
                        break;
                }
            }
                break;

            case TypeID::Float: {
                switch (other.type) {
                    case TypeID::Integer:
                        return floatValue >= static_cast<Float>(other.intValue);

                    case TypeID::Float:
                        return floatValue >= other.floatValue;

                    default:
                        break;
                }
            }
                break;

            default:
                break;
        }

        std::ostringstream ss;
        ss << "Operator '>=' is not supported between '" << type << "' and '" << other.type << "'.";
        throw std::runtime_error(ss.str());
    }

    bool Value::operator&&(const Value& other) const {
        if (type == TypeID::Boolean && other.type == TypeID::Boolean)
            return boolValue && other.boolValue;

        std::ostringstream ss;
        ss << "Operator '&&' is not supported between '" << type << "' and '" << other.type << "'.";
        throw std::runtime_error(ss.str());
    }

    bool Value::operator||(const Value& other) const {
        if (type == TypeID::Boolean && other.type == TypeID::Boolean)
            return boolValue || other.boolValue;

        std::ostringstream ss;
        ss << "Operator '||' is not supported between '" << type << "' and '" << other.type << "'.";
        throw std::runtime_error(ss.str());
    }

    bool Value::exclusiveOr(const Value& lhs, const Value& rhs) {
        if (lhs.type == TypeID::Boolean && rhs.type == TypeID::Boolean)
            return lhs.boolValue != rhs.boolValue;

        std::ostringstream ss;
        ss << "Operator '^^' is not supported between '" << lhs.type << "' and '" << rhs.type << "'.";
        throw std::runtime_error(ss.str());
    }

    bool Value::operator!() const {
        if (type == TypeID::Boolean)
            return !boolValue;

        std::ostringstream ss;
        ss << "Operator '!' is not supported for '" << type << "'.";
        throw std::runtime_error(ss.str());
    }

} // Spark
