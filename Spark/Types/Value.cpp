#include "Value.hpp"

#include <cmath>
#include <iomanip>
#include <sstream>

#include "../GC/GC.hpp"
#include "Closure.hpp"

namespace std {
    std::size_t hash<Spark::Value>::operator()(const Spark::Value& value) const  {
        using namespace Spark;

        std::size_t h1 = std::hash<Type>()(value.type);
        std::size_t h2;
        switch (value.type) {
            case Type::Nil:
                h2 = 0;
                break;

            case Type::Integer:
                h2 = std::hash<Int>()(value.intValue);
                break;

            case Type::Float:
                h2 = std::hash<Float>()(value.floatValue);
                break;

            case Type::Boolean:
                h2 = std::hash<Bool>()(value.boolValue);
                break;

            case Type::CFunction:
                h2 = std::hash<CFunction>()(value.cFuncPtr);
                break;

            case Type::Type:
                h2 = std::hash<Type>()(value.typeValue);
                break;

            default:
                h2 = std::hash<GCNode*>()(value.nodePtr);
                break;
        }
        return h1 ^ (h2 << 1);
    }
} // std

namespace Spark {

    /* ===== Factory Methods ===== */

    Value Value::makeString(GC& gc, const std::string& value) {
        Value self;
        self.type = Type::String;
        self.nodePtr = gc.allocate<std::string>(value);
        return self;
    }

    Value Value::makeArray(GC& gc, const std::vector<Value>& value) {
        Value self;
        self.type = Type::Array;
        self.nodePtr = gc.allocate<std::vector<Value>>(value);
        return self;
    }

    Value Value::makeSet(GC& gc, const std::unordered_set<Value>& value) {
        Value self;
        self.type = Type::Set;
        self.nodePtr = gc.allocate<std::unordered_set<Value>>(value);
        return self;
    }

    Value Value::makeMap(GC& gc, const std::unordered_map<Value, Value>& value) {
        Value self;
        self.type = Type::Map;
        self.nodePtr = gc.allocate<std::unordered_map<Value, Value>>(value);
        return self;
    }

    Value Value::makeClosure(GC& gc, const Closure& value) {
        Value self;
        self.type = Type::Closure;
        self.nodePtr = gc.allocate<Closure>(value);
        return self;
    }



    /* ===== Operators ===== */

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

            case Type::CFunction: {
                os << "<CFunction: 0x"
                   << std::hex
                   << std::setw(2)
                   << std::setfill('0')
                   << reinterpret_cast<uintptr_t>(value.cFuncPtr)
                   << ">"
                   << std::dec;
            }
                break;

            case Type::Type:
                os << value.typeValue;
                break;

            case Type::String:
                os << value.nodePtr->data<std::string>();
                break;

            case Type::Array: {
                const std::vector<Value>& vec = value.nodePtr->data<std::vector<Value>>();
                os << "[";
                for (size_t i = 0; i < vec.size(); i++) {
                    os << vec[i];
                    if (i != vec.size() - 1)
                        os << ", ";
                }
                os << "]";
            }
                break;

            case Type::Function: {
                os << "<Function: "
                   << std::hex
                   << std::setw(2)
                   << std::setfill('0')
                   << reinterpret_cast<uintptr_t>(value.nodePtr)
                   << ">"
                   << std::dec;
            }
                break;

            default: {
                os << "<" << value.type <<": 0x"
                   << std::hex
                   << std::setw(2)
                   << std::setfill('0')
                   << reinterpret_cast<uintptr_t>(value.nodePtr)
                   << ">"
                   << std::dec;
            }
                break;
        }
        return os;
    }

    Value Value::operator+(const Value& other) const {
        switch (type) {
            case Type::Integer: {
                switch (other.type) {
                    case Type::Integer:
                        return makeInt(intValue + other.intValue);

                    case Type::Float:
                        return makeFloat(static_cast<Float>(intValue) + other.floatValue);

                    default:
                        break;
                }
            }
                break;

            case Type::Float: {
                switch (other.type) {
                    case Type::Integer:
                        return makeFloat(floatValue + static_cast<Float>(other.intValue));

                    case Type::Float:
                        return makeFloat(floatValue + other.floatValue);

                    default:
                        break;
                }
            }
                break;

            case Type::String: {
                std::ostringstream ss;
                ss << nodePtr->data<std::string>() << other;
                return makeString(nodePtr->getGC(), ss.str());
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
            case Type::Integer: {
                switch (other.type) {
                    case Type::Integer:
                        return makeInt(intValue - other.intValue);

                    case Type::Float:
                        return makeFloat(static_cast<Float>(intValue) - other.floatValue);

                    default:
                        break;
                }
            }
                break;

            case Type::Float: {
                switch (other.type) {
                    case Type::Integer:
                        return makeFloat(floatValue - static_cast<Float>(other.intValue));

                    case Type::Float:
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
            case Type::Integer: {
                switch (other.type) {
                    case Type::Integer:
                        return makeInt(intValue * other.intValue);

                    case Type::Float:
                        return makeFloat(static_cast<Float>(intValue) * other.floatValue);

                    default:
                        break;
                }
            }
                break;

            case Type::Float: {
                switch (other.type) {
                    case Type::Integer:
                        return makeFloat(floatValue * static_cast<Float>(other.intValue));

                    case Type::Float:
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
            case Type::Integer: {
                switch (other.type) {
                    case Type::Integer:
                        return makeInt(intValue / other.intValue);

                    case Type::Float:
                        return makeFloat(static_cast<Float>(intValue) / other.floatValue);

                    default:
                        break;
                }
            }
                break;

            case Type::Float: {
                switch (other.type) {
                    case Type::Integer:
                        return makeFloat(floatValue / static_cast<Float>(other.intValue));

                    case Type::Float:
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
            case Type::Integer: {
                switch (other.type) {
                    case Type::Integer:
                        return makeInt(intValue % other.intValue);

                    case Type::Float:
                        return makeFloat(std::fmod(static_cast<Float>(intValue), other.floatValue));

                    default:
                        break;
                }
            }
                break;

            case Type::Float: {
                switch (other.type) {
                    case Type::Integer:
                        return makeFloat(std::fmod(floatValue, static_cast<Float>(other.intValue)));

                    case Type::Float:
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
            case Type::Integer: {
                switch (other.type) {
                    case Type::Integer:
                        return intValue == other.intValue;

                    case Type::Float:
                        return static_cast<Float>(intValue) == other.floatValue;

                    default:
                        break;
                }
            }
                break;

            case Type::Float: {
                switch (other.type) {
                    case Type::Integer:
                        return floatValue == static_cast<Float>(other.intValue);

                    case Type::Float:
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
            case Type::Nil:
                return true;

            case Type::Boolean:
                return boolValue == other.boolValue;

            case Type::CFunction:
                return cFuncPtr == other.cFuncPtr;

            case Type::Type:
                return typeValue == other.typeValue;

            case Type::String:
                return nodePtr->data<std::string>() == other.nodePtr->data<std::string>();

            case Type::Array:
                return nodePtr->data<std::vector<Value>>() == other.nodePtr->data<std::vector<Value>>();

            case Type::Set:
                return nodePtr->data<std::unordered_set<Value>>() == other.nodePtr->data<std::unordered_set<Value>>();

            case Type::Map:
                return nodePtr->data<std::unordered_map<Value, Value>>() ==
                       other.nodePtr->data<std::unordered_map<Value, Value>>();

            default:
                return nodePtr == other.nodePtr;
        }
    }

    bool Value::operator!=(const Value& other) const {
        return !(*this == other);
    }

    bool Value::operator<(const Value& other) const {
        switch (type) {
            case Type::Integer: {
                switch (other.type) {
                    case Type::Integer:
                        return intValue < other.intValue;

                    case Type::Float:
                        return static_cast<Float>(intValue) < other.floatValue;

                    default:
                        break;
                }
            }
                break;

            case Type::Float: {
                switch (other.type) {
                    case Type::Integer:
                        return floatValue < static_cast<Float>(other.intValue);

                    case Type::Float:
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
            case Type::Integer: {
                switch (other.type) {
                    case Type::Integer:
                        return intValue <= other.intValue;

                    case Type::Float:
                        return static_cast<Float>(intValue) <= other.floatValue;

                    default:
                        break;
                }
            }
                break;

            case Type::Float: {
                switch (other.type) {
                    case Type::Integer:
                        return floatValue <= static_cast<Float>(other.intValue);

                    case Type::Float:
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
            case Type::Integer: {
                switch (other.type) {
                    case Type::Integer:
                        return intValue > other.intValue;

                    case Type::Float:
                        return static_cast<Float>(intValue) > other.floatValue;

                    default:
                        break;
                }
            }
                break;

            case Type::Float: {
                switch (other.type) {
                    case Type::Integer:
                        return floatValue > static_cast<Float>(other.intValue);

                    case Type::Float:
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
            case Type::Integer: {
                switch (other.type) {
                    case Type::Integer:
                        return intValue >= other.intValue;

                    case Type::Float:
                        return static_cast<Float>(intValue) >= other.floatValue;

                    default:
                        break;
                }
            }
                break;

            case Type::Float: {
                switch (other.type) {
                    case Type::Integer:
                        return floatValue >= static_cast<Float>(other.intValue);

                    case Type::Float:
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
        if (type == Type::Boolean && other.type == Type::Boolean)
            return boolValue && other.boolValue;

        std::ostringstream ss;
        ss << "Operator '&&' is not supported between '" << type << "' and '" << other.type << "'.";
        throw std::runtime_error(ss.str());
    }

    bool Value::operator||(const Value& other) const {
        if (type == Type::Boolean && other.type == Type::Boolean)
            return boolValue || other.boolValue;

        std::ostringstream ss;
        ss << "Operator '||' is not supported between '" << type << "' and '" << other.type << "'.";
        throw std::runtime_error(ss.str());
    }

    bool Value::exclusiveOr(const Value& lhs, const Value& rhs) {
        if (lhs.type == Type::Boolean && rhs.type == Type::Boolean)
            return lhs.boolValue != rhs.boolValue;

        std::ostringstream ss;
        ss << "Operator '^^' is not supported between '" << lhs.type << "' and '" << rhs.type << "'.";
        throw std::runtime_error(ss.str());
    }

    bool Value::operator!() const {
        if (type == Type::Boolean)
            return !boolValue;

        std::ostringstream ss;
        ss << "Operator '!' is not supported for '" << type << "'.";
        throw std::runtime_error(ss.str());
    }

} // Spark
