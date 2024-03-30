#include "Value.hpp"

#include <cmath>
#include <sstream>

#include "../GC/GC.hpp"
#include "Closure.hpp"

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

            case Type::Type:
                os << value.typeValue;
                break;

            case Type::String:
                os << value.nodePtr->getData<std::string>();
                break;

            case Type::Array: {
                const std::vector<Value>& vec = value.nodePtr->getData<std::vector<Value>>();
                os << "[";
                for (size_t i = 0; i < vec.size(); i++) {
                    os << vec[i];
                    if (i != vec.size() - 1)
                        os << ", ";
                }
                os << "]";
            }
                break;

            default:
                os << "unimplemented";
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
                ss << nodePtr->getData<std::string>() << other;
                return makeString(nodePtr->getGC(), ss.str());
            }
                break;

            default:
                break;
        }

        std::ostringstream ss;
        ss << "Operator '+' is not supported between " << type << " and " << other.type << ".";
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
        ss << "Operator '-' is not supported between " << type << " and " << other.type << ".";
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
        ss << "Operator '*' is not supported between " << type << " and " << other.type << ".";
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
        ss << "Operator '/' is not supported between " << type << " and " << other.type << ".";
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
        ss << "Operator '%' is not supported between " << type << " and " << other.type << ".";
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

            case Type::Type:
                return typeValue == other.typeValue;

            case Type::String:
                return nodePtr->getData<std::string>() == other.nodePtr->getData<std::string>();

            case Type::Array:
                return nodePtr->getData<std::vector<Value>>() == other.nodePtr->getData<std::vector<Value>>();

            case Type::Set:
                return nodePtr->getData<std::unordered_set<Value>>() == other.nodePtr->getData<std::unordered_set<Value>>();

            case Type::Map:
                return nodePtr->getData<std::unordered_map<Value, Value>>() == other.nodePtr->getData<std::unordered_map<Value, Value>>();

            default:
                return nodePtr == other.nodePtr;
        }
    }

} // Spark
