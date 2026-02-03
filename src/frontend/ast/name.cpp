#include "name.hpp"

namespace Spark::FrontEnd {

std::string_view NameValue::str() const noexcept {
    return std::visit([](auto const& v) -> std::string_view {
        using T = std::decay_t<decltype(v)>;

        if constexpr (std::is_same_v<T, Identifier>) {
            return v.name;
        } else if constexpr (std::is_same_v<T, Discard>) {
            return "_";
        } else if constexpr (std::is_same_v<T, Self>) {
            return "self";
        } else if constexpr (std::is_same_v<T, Constructor>) {
            return "constructor";
        } else if constexpr (std::is_same_v<T, Destructor>) {
            return "destructor";
        } else if constexpr (std::is_same_v<T, Operator>) {
            switch (v.op) {
                case OverOpKind::Add:          return "operator+";
                case OverOpKind::Sub:          return "operator-";
                case OverOpKind::Mul:          return "operator*";
                case OverOpKind::Div:          return "operator/";
                case OverOpKind::Mod:          return "operator%";
                case OverOpKind::BitNot:       return "operator~";
                case OverOpKind::BitAnd:       return "operator&";
                case OverOpKind::BitOr:        return "operator|";
                case OverOpKind::BitXor:       return "operator^";
                case OverOpKind::BitShl:       return "operator<<";
                case OverOpKind::BitShr:       return "operator>>";
                case OverOpKind::LogNot:       return "operator!";
                case OverOpKind::LogAnd:       return "operator&&";
                case OverOpKind::LogOr:        return "operator||";
                case OverOpKind::Eq:           return "operator==";
                case OverOpKind::Ne:           return "operator!=";
                case OverOpKind::Lt:           return "operator<";
                case OverOpKind::Le:           return "operator<=";
                case OverOpKind::Gt:           return "operator>";
                case OverOpKind::Ge:           return "operator>=";
                case OverOpKind::Range:        return "operator...";
                case OverOpKind::RangeExcl:    return "operator..<";
                case OverOpKind::Pos:          return "operator(+)";
                case OverOpKind::Call:         return "operator()";
                case OverOpKind::Subscript:    return "operator[]";
                case OverOpKind::AddAssign:    return "operator+=";
                case OverOpKind::SubAssign:    return "operator-=";
                case OverOpKind::MulAssign:    return "operator*=";
                case OverOpKind::DivAssign:    return "operator/=";
                case OverOpKind::ModAssign:    return "operator%=";
                case OverOpKind::BitAndAssign: return "operator&=";
                case OverOpKind::BitOrAssign:  return "operator|=";
                case OverOpKind::BitXorAssign: return "operator^=";
                case OverOpKind::BitShlAssign: return "operator<<=";
                case OverOpKind::BitShrAssign: return "operator>>=";
                default:                       return ""; // Unreachable
            }
        }
        return ""; // Unreachable
    }, _value);
}

InternedNameValue NameValueInterner::intern(NameValue value) {
    std::unique_ptr<NameValue> key = std::make_unique<NameValue>(std::move(value));

    if (auto it = _set.find(key); it != _set.end()) {
        return InternedNameValue{it->get()};
    }
    auto [it, _] = _set.emplace(std::move(key));
    return InternedNameValue{it->get()};
}

} // Spark::FrontEnd
