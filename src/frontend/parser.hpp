#pragma once

#include <optional>
#include <string_view>

#include "ast.hpp"
#include "lexer.hpp"
#include "utils/error.hpp"
#include "utils/result.hpp"

namespace Spark::FrontEnd {

class Parser {
public:
    Parser() = default;

    static Result<AST, Error> parse(std::istream& stream, std::optional<std::string_view> filename = std::nullopt);
};

} // Spark::FrontEnd
