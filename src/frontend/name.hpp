#pragma once

#include <cstdint>
#include <string>
#include <variant>

namespace Spark::FrontEnd {

struct IdentifierName {
    std::string name;

    explicit IdentifierName(std::string name) noexcept : name(std::move(name)) { }

    bool operator==(const IdentifierName& rhs) const noexcept { return name == rhs.name; }
    bool operator!=(const IdentifierName& rhs) const noexcept { return !(*this == rhs); }
};

struct DiscardName {
    DiscardName() noexcept = default;

    bool operator==(const DiscardName) const noexcept { return true; }
    bool operator!=(const DiscardName) const noexcept { return false; }
};

struct ConstructorName {
    ConstructorName() noexcept = default;

    bool operator==(const ConstructorName) const noexcept { return true; }
    bool operator!=(const ConstructorName) const noexcept { return false; }
};

struct DestructorName {
    DestructorName() noexcept = default;

    bool operator==(const DestructorName) const noexcept { return true; }
    bool operator!=(const DestructorName) const noexcept { return false; }
};

struct OperatorName {
    enum class OpKind {
        Add, Sub, Mul, Div, Mod, BitNot, BitAnd, BitOr, BitXor, BitShl, BitShr, LogNot,
            LogAnd, LogOr, Eq, Ne, Lt, Le, Gt, Ge, Range, RangeExcl,
        Pos, Neg,
        Call, Subscript,
        AddAssign, SubAssign, MulAssign, DivAssign, ModAssign, BitAndAssign, BitOrAssign,
            BitXorAssign, BitShlAssign, BitShrAssign
    };

    OpKind op;

    explicit OperatorName(OpKind op) noexcept : op(op) { }

    bool operator==(const OperatorName rhs) const noexcept { return op == rhs.op; }
    bool operator!=(const OperatorName rhs) const noexcept { return !(*this == rhs); }
};

struct SelfName {
    SelfName() noexcept = default;

    bool operator==(const SelfName) const noexcept { return true; }
    bool operator!=(const SelfName) const noexcept { return false; }
};

using Name = std::variant<DiscardName,
                          IdentifierName,
                          ConstructorName,
                          DestructorName,
                          OperatorName,
                          SelfName>;

using NameId = uint64_t;

} // Spark::FrontEnd
