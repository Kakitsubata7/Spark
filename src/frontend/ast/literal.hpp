#pragma once

#include <string>
#include <variant>

#include "utils/bigint.hpp"
#include "utils/bigreal.hpp"

namespace Spark::FrontEnd {

struct IntLiteral {
    BigInt value;

    explicit IntLiteral(BigInt value) noexcept : value(std::move(value)) { }

    bool operator==(const IntLiteral& rhs) const noexcept { return value == rhs.value; }
    bool operator!=(const IntLiteral& rhs) const noexcept { return !(*this == rhs); }
};

struct RealLiteral {
    BigReal value;

    explicit RealLiteral(BigReal value) noexcept : value(std::move(value)) { }

    bool operator==(const RealLiteral& rhs) const noexcept { return value == rhs.value; }
    bool operator!=(const RealLiteral& rhs) const noexcept { return !(*this == rhs); }
};

struct BoolLiteral {
    bool value;

    explicit BoolLiteral(bool value) noexcept : value(value) { }

    bool operator==(const BoolLiteral rhs) const noexcept { return value == rhs.value; }
    bool operator!=(const BoolLiteral rhs) const noexcept { return !(*this == rhs); }
};

struct StringLiteral {
    std::string value;

    explicit StringLiteral(std::string value) noexcept : value(std::move(value)) { }

    bool operator==(const StringLiteral& rhs) const noexcept { return value == rhs.value; }
    bool operator!=(const StringLiteral& rhs) const noexcept { return !(*this == rhs); }
};

struct NilLiteral {
    NilLiteral() noexcept = default;

    bool operator==(const NilLiteral) const noexcept { return true; }
    bool operator!=(const NilLiteral) const noexcept { return false; }
};

struct VoidLiteral {
    VoidLiteral() noexcept = default;

    bool operator==(const VoidLiteral) const noexcept { return true; }
    bool operator!=(const VoidLiteral) const noexcept { return false; }
};

using Literal = std::variant<VoidLiteral,
                             IntLiteral,
                             RealLiteral,
                             BoolLiteral,
                             StringLiteral,
                             NilLiteral>;

} // Spark::FrontEnd
