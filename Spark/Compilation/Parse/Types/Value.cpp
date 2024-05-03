#include <cmath>
#include <sstream>
#include <stdexcept>

#include "Value.hpp"

namespace Spark {

    /* ===== Constructors, Factory Methods & Destructor ===== */

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

    Value Value::makeNil() {
        Value value;
        value.type = Type::Nil;
        return value;
    }

    Value Value::makeInt(Int i) {
        Value value;
        value.type = Type::Int;
        value.intValue = i;
        return value;
    }

    Value Value::makeFloat(Float f) {
        Value value;
        value.type = Type::Float;
        value.floatValue = f;
        return value;
    }

    Value Value::makeBool(Bool b) {
        Value value;
        value.type = Type::Bool;
        value.boolValue = b;
        return value;
    }

    Value Value::makeString(const std::string& str) {
        Value value;
        value.type = Type::String;
        value.strValue = str;
        return value;
    }



    /* ===== Operators ===== */

    Value Value::operator+(const Value& other) const {
        switch (type) {
            case Type::Int: {
                switch (other.type) {
                    case Type::Int:
                        return Value::makeInt(intValue + other.intValue);

                    case Type::Float:
                        return Value::makeFloat(static_cast<Float>(intValue) + other.floatValue);

                    default:
                        goto unsupported;
                }
            }

            case Type::Float: {
                switch (other.type) {
                    case Type::Int:
                        return Value::makeFloat(floatValue + static_cast<Float>(other.intValue));

                    case Type::Float:
                        return Value::makeFloat(floatValue + other.floatValue);

                    default:
                        goto unsupported;
                }
            }

            case Type::String: {
                std::stringstream ss;
                ss << strValue << other;
                return Value::makeString(ss.str());
            }

            default: {
            unsupported:
                std::stringstream ss;
                ss << "'+' operator is not supported between " << type << " and " << other.type << ".";
                throw std::runtime_error(ss.str());
            }
        }
    }

    Value Value::operator-(const Value& other) const {
        switch (type) {
            case Type::Int: {
                switch (other.type) {
                    case Type::Int:
                        return Value::makeInt(intValue - other.intValue);

                    case Type::Float:
                        return Value::makeFloat(static_cast<Float>(intValue) - other.floatValue);

                    default:
                        goto unsupported;
                }
            }

            case Type::Float: {
                switch (other.type) {
                    case Type::Int:
                        return Value::makeFloat(floatValue - static_cast<Float>(other.intValue));

                    case Type::Float:
                        return Value::makeFloat(floatValue - other.floatValue);

                    default:
                        goto unsupported;
                }
            }

            default: {
                unsupported:
                std::stringstream ss;
                ss << "'-' operator is not supported between " << type << " and " << other.type << ".";
                throw std::runtime_error(ss.str());
            }
        }
    }

    Value Value::operator*(const Value& other) const {
        switch (type) {
            case Type::Int: {
                switch (other.type) {
                    case Type::Int:
                        return Value::makeInt(intValue * other.intValue);

                    case Type::Float:
                        return Value::makeFloat(static_cast<Float>(intValue) * other.floatValue);

                    default:
                        goto unsupported;
                }
            }

            case Type::Float: {
                switch (other.type) {
                    case Type::Int:
                        return Value::makeFloat(floatValue * static_cast<Float>(other.intValue));

                    case Type::Float:
                        return Value::makeFloat(floatValue * other.floatValue);

                    default:
                        goto unsupported;
                }
            }

            default: {
                unsupported:
                std::stringstream ss;
                ss << "'*' operator is not supported between " << type << " and " << other.type << ".";
                throw std::runtime_error(ss.str());
            }
        }
    }

    Value Value::operator/(const Value& other) const {
        switch (type) {
            case Type::Int: {
                switch (other.type) {
                    case Type::Int:
                        return Value::makeInt(intValue / other.intValue);

                    case Type::Float:
                        return Value::makeFloat(static_cast<Float>(intValue) / other.floatValue);

                    default:
                        goto unsupported;
                }
            }

            case Type::Float: {
                switch (other.type) {
                    case Type::Int:
                        return Value::makeFloat(floatValue / static_cast<Float>(other.intValue));

                    case Type::Float:
                        return Value::makeFloat(floatValue / other.floatValue);

                    default:
                        goto unsupported;
                }
            }

            default: {
                unsupported:
                std::stringstream ss;
                ss << "'/' operator is not supported between " << type << " and " << other.type << ".";
                throw std::runtime_error(ss.str());
            }
        }
    }

    Value Value::operator%(const Value& other) const {
        switch (type) {
            case Type::Int: {
                switch (other.type) {
                    case Type::Int:
                        return Value::makeInt(intValue % other.intValue);

                    case Type::Float:
                        return Value::makeFloat(std::fmod(static_cast<Float>(intValue), other.floatValue));

                    default:
                        goto unsupported;
                }
            }

            case Type::Float: {
                switch (other.type) {
                    case Type::Int:
                        return Value::makeFloat(std::fmod(floatValue, static_cast<Float>(other.intValue)));

                    case Type::Float:
                        return Value::makeFloat(std::fmod(floatValue, other.floatValue));

                    default:
                        goto unsupported;
                }
            }

            default: {
                unsupported:
                std::stringstream ss;
                ss << "'%' operator is not supported between " << type << " and " << other.type << ".";
                throw std::runtime_error(ss.str());
            }
        }
    }

    std::ostream& operator<<(std::ostream& os, const Value& value) {
        switch (value.type) {
            case Type::Nil:
                os << "nil";
                break;

            case Type::Int:
                os << value.intValue;
                break;

            case Type::Float:
                os << value.floatValue;
                break;

            case Type::Bool:
                os << value.boolValue;
                break;

            case Type::String:
                os << value.strValue;
                break;

            default:
                os << std::ios_base::failbit;
        }
        return os;
    }

} // Spark
