#pragma once

#include <optional>
#include <utility>
#include <vector>

#include "ast.hpp"
#include "lexer.hpp"
#include "parser/parser_context.hpp"
#include "source_buffer.hpp"
#include "utils/error.hpp"

namespace Spark::FrontEnd {

class Parser {
public:
    Parser() = default;

    std::pair<AST, std::vector<Error>> parse(std::istream& stream,
                                             std::optional<std::string_view> filename = std::nullopt,
                                             SourceBuffer& srcbuf = NullSourceBuffer::instance());
};

} // Spark::FrontEnd
