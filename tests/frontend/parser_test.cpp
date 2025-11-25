#include <optional>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

#include "frontend/ast.hpp"
#include "frontend/parser.hpp"

using namespace Spark;
using namespace Spark::FrontEnd;

std::pair<AST, std::vector<Error>> parse(std::string_view source) {
    std::istringstream iss{std::string(source)};
    Parser parser;
    SourceBuffer srcbuf;
    return parser.parse(iss, std::nullopt, srcbuf);
}

TEST(ParserTest, GeneralTest1) {
    std::string_view source = R"(while true do end)";
    auto [ast, errors] = parse(source);
    EXPECT_TRUE(errors.empty());
}
