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
    return parser.parse(iss);
}

TEST(ParserTest, SimpleTest1) {
    std::string_view source = R"(while true do end)";
    auto [ast, errors] = parse(source);
    EXPECT_TRUE(errors.empty());
}

TEST(ParserTest, SimpleTest2) {
    std::string_view source = R"(b = if x == 1 then true else false)";
    auto [ast, errors] = parse(source);
    EXPECT_TRUE(errors.empty());
}

TEST(ParserTest, SimpleTest3) {
    std::string_view source = R"(foo.bar.baz())";
    auto [ast, errors] = parse(source);
    EXPECT_TRUE(errors.empty());
}
