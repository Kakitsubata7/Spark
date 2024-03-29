#include "Value.hpp"

#include <cmath>
#include <sstream>

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

} // Spark
