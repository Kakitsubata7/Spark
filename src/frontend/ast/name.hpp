#pragma once

#include <memory>
#include <ostream>
#include <string>
#include <unordered_set>
#include <variant>

namespace Spark::FrontEnd {

enum class OverOpKind {
    Add, Sub, Mul, Div, Mod, BitNot, BitAnd, BitOr, BitXor, BitShl, BitShr, LogNot,
        LogAnd, LogOr, Eq, Ne, Lt, Le, Gt, Ge, Range, RangeExcl,
    Pos, Neg,
    Call, Subscript,
    AddAssign, SubAssign, MulAssign, DivAssign, ModAssign, BitAndAssign, BitOrAssign,
        BitXorAssign, BitShlAssign, BitShrAssign
};

class NameValue {
private:
    struct Identifier {
        std::string name;
        friend bool operator==(const Identifier& lhs, const Identifier& rhs) noexcept { return lhs.name == rhs.name; };
    };
    struct Discard { friend bool operator==(const Discard&, const Discard&) noexcept { return true; } };
    struct Self { friend bool operator==(const Self&, const Self&) noexcept { return true; } };
    struct Constructor { friend bool operator==(const Constructor&, const Constructor&) noexcept { return true; } };
    struct Destructor { friend bool operator==(const Destructor&, const Destructor&) noexcept { return true; } };
    struct Operator {
        OverOpKind op;
        friend bool operator==(const Operator& lhs, const Operator& rhs) noexcept { return lhs.op == rhs.op; }
    };

    using ValueT = std::variant<Identifier, Discard, Self, Constructor, Destructor, Operator>;

    ValueT _value;

    explicit NameValue(ValueT value) noexcept : _value(std::move(value)) { }

public:
    [[nodiscard]]
    static NameValue identifier(std::string name) noexcept { return NameValue(Identifier{.name = std::move(name)}); }
    [[nodiscard]]
    static NameValue discard() noexcept { return NameValue(Discard{}); }
    [[nodiscard]]
    static NameValue self() noexcept { return NameValue(Self{}); }
    [[nodiscard]]
    static NameValue constructor() noexcept { return NameValue(Constructor{}); }
    [[nodiscard]]
    static NameValue destructor() noexcept { return NameValue(Destructor{}); }
    [[nodiscard]]
    static NameValue op(OverOpKind op) noexcept { return NameValue(Operator{.op = op}); }

    [[nodiscard]]
    bool isIdentifier() const noexcept { return std::holds_alternative<Identifier>(_value); }
    [[nodiscard]]
    bool isDiscard() const noexcept { return std::holds_alternative<Discard>(_value); }
    [[nodiscard]]
    bool isSelf() const noexcept { return std::holds_alternative<Self>(_value); }
    [[nodiscard]]
    bool isConstructor() const noexcept { return std::holds_alternative<Constructor>(_value); }
    [[nodiscard]]
    bool isDestructor() const noexcept { return std::holds_alternative<Destructor>(_value); }
    [[nodiscard]]
    bool isOp() const noexcept { return std::holds_alternative<Operator>(_value); }

    [[nodiscard]]
    std::string_view str() const noexcept;

    bool operator==(const NameValue& rhs) const noexcept {
        return _value == rhs._value;
    }

    bool operator!=(const NameValue& rhs) const noexcept {
        return !(*this == rhs);
    }

    friend std::hash<NameValue>;
};

} // Spark::FrontEnd

template<>
struct std::hash<Spark::FrontEnd::NameValue> {
    size_t operator()(const Spark::FrontEnd::NameValue& n) const noexcept {
        return std::visit([&](auto const& v) -> size_t {
            using T = std::decay_t<decltype(v)>;

            size_t tag = n._value.index();
            if constexpr (std::is_same_v<T, Spark::FrontEnd::NameValue::Identifier>) {
                return tag ^ (std::hash<std::string>{}(v.name) << 1);
            } else if constexpr (std::is_same_v<T, Spark::FrontEnd::NameValue::Operator>) {
                return tag ^ (std::hash<Spark::FrontEnd::OverOpKind>{}(v.op) << 1);
            } else {
                return tag;
            }
        }, n._value);
    }
};

namespace Spark::FrontEnd {

class InternedNameValue {
private:
    const NameValue* _ptr;

public:
    explicit InternedNameValue(const NameValue* ptr) noexcept : _ptr(ptr) { }

    [[nodiscard]]
    const NameValue& value() const noexcept { return *_ptr; }

    [[nodiscard]]
    std::string_view str() const noexcept { return _ptr->str(); }

    friend bool operator==(InternedNameValue lhs, InternedNameValue rhs) noexcept {
        return lhs._ptr == rhs._ptr;
    }

    friend bool operator!=(InternedNameValue lhs, InternedNameValue rhs) noexcept {
        return !(lhs == rhs);
    }

    friend std::ostream& operator<<(std::ostream& os, const InternedNameValue& n) {
        return os << n.str();
    }

    friend std::hash<InternedNameValue>;
};

class NameValueInterner {
private:
    struct PtrHash {
        size_t operator()(const std::unique_ptr<NameValue>& p) const noexcept {
            return std::hash<NameValue>{}(*p);
        }
    };

    struct PtrEq {
        bool operator()(const std::unique_ptr<NameValue>& lhs, const std::unique_ptr<NameValue>& rhs) const noexcept {
            return *lhs == *rhs;
        }
    };

    std::unordered_set<std::unique_ptr<NameValue>, PtrHash, PtrEq> _set;

public:
    InternedNameValue intern(NameValue value);
};

} // Spark::FrontEnd

template <>
struct std::hash<Spark::FrontEnd::InternedNameValue> {
    size_t operator()(const Spark::FrontEnd::InternedNameValue& n) const noexcept {
        return std::hash<const Spark::FrontEnd::NameValue*>{}(n._ptr);
    }
};
