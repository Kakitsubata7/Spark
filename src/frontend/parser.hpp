#pragma once

#include <optional>
#include <string_view>
#include <vector>

#include "ast.hpp"
#include "utils/error.hpp"

namespace Spark::FrontEnd {

class Parser {
public:
    Parser() = default;

    static std::pair<AST, std::vector<Error>> parse(std::istream& stream,
                                                    std::optional<std::string_view> filename = std::nullopt);
};

} // Spark::FrontEnd
