#include <string_view>
#include <tuple>
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

TEST(ParserTest, Block) {
    std::string_view source = "{ }";
    auto [ast, errors] = parse(source);
    EXPECT_TRUE(errors.empty());
}

TEST(ParserTest, Return) {
    std::string_view source = R"(
return
return true
)";
    auto [ast, errors] = parse(source);
    EXPECT_TRUE(errors.empty());
}

TEST(ParserTest, While) {
    std::string_view source = R"(
while true { }
)";
    auto [ast, errors] = parse(source);
    EXPECT_TRUE(errors.empty());
}

TEST(ParserTest, SimpleTest2) {
    std::string_view source = R"(
x = 1 y += 2 z -= 3;
a *= x ; b /= 0 ; c %= 10
)";
    auto [ast, errors] = parse(source);
    EXPECT_TRUE(errors.empty());

    source = R"(
{
    x = 1
    y = 2
    z = 3
}
)";
    std::tie(ast, errors) = parse(source);
    EXPECT_TRUE(errors.empty());
}

TEST(ParserTest, SimpleTest11) {
    std::string_view source = "while true { }";
    auto [ast, errors] = parse(source);
    EXPECT_TRUE(errors.empty());
}

TEST(ParserTest, SimpleTest12) {
    std::string_view source = "b = if x == 1 then true else false";
    auto [ast, errors] = parse(source);
    EXPECT_TRUE(errors.empty());
}

TEST(ParserTest, SimpleTest13) {
    std::string_view source = R"(
foo()
foo.bar()
foo.bar.baz()
obj.fn1().fn2().fn3()
a?()
a?.b()
a?.b().c?
)";
    auto [ast, errors] = parse(source);
    EXPECT_TRUE(errors.empty());
}

