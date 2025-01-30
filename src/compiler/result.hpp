#pragma once

#include <optional>
#include <string>
#include <vector>

namespace spark {

template <typename T>
struct Result {
    std::optional<T> value;
    std::vector<std::string> warnings;
    std::vector<std::string> errors;

    [[nodiscard]] constexpr bool hasError() const { return !errors.empty(); }
};

} // spark
