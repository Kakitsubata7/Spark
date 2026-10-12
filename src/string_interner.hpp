#pragma once

#include <string>
#include <unordered_set>

#include "interned_string.hpp"

namespace Spark {

class StringInterner {
private:
    struct StringHash {
        using is_transparent = void;

        std::size_t operator()(std::string_view sv) const noexcept {
            return std::hash<std::string_view>{}(sv);
        }
    };

    std::unordered_set<std::string, StringHash, std::equal_to<>> _internedSet;

public:
    InternedString intern(std::string_view sv) {
        // Empty string
        if (sv.empty()) {
            return InternedString::empty;
        }

        // Get existing interned string
        if (const auto it = _internedSet.find(sv); it != _internedSet.end()) {
            return InternedString{*it};
        }

        // Intern the new string
        auto [s, _] = _internedSet.emplace(std::string{sv});
        return InternedString{*s};
    }
};

} // Spark
