#pragma once

#include <ostream>
#include <string_view>

namespace Spark {

class InternedString {
private:
    std::string_view _view;

public:
    [[nodiscard]]
    constexpr std::string_view view() const noexcept { return _view; }

    constexpr explicit InternedString(std::string_view view) noexcept : _view{view} { }
    constexpr InternedString() noexcept : InternedString{""} { }

    constexpr bool operator==(InternedString other) const {
        return _view.data() == other._view.data();
    }

    constexpr bool operator!=(InternedString other) const {
        return !(*this == other);
    }

    static const InternedString empty;
};

inline constexpr InternedString InternedString::empty{};

inline std::ostream& operator<<(std::ostream& os, InternedString is) {
    os << is.view();
    return os;
}

} // Spark
