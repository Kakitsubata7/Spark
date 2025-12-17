#include <string_view>
#include <tuple>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

#include "frontend/ast.hpp"
#include "frontend/parser.hpp"
#include "utils/error.hpp"
#include "utils/result.hpp"

using namespace Spark;
using namespace Spark::FrontEnd;

Result<AST, Error> parse(std::string_view source) {
    std::istringstream iss{std::string(source)};
    Parser parser;
    return parser.parse(iss);
}

TEST(ParserTest, Block) {
    std::string_view source = "{ }";
    Result<AST, Error> result = parse(source);
    EXPECT_FALSE(result.hasError());
}

TEST(ParserTest, Return) {
    std::string_view source = R"(
return
return true
)";
    Result<AST, Error> result = parse(source);
    EXPECT_FALSE(result.hasError());
}

TEST(ParserTest, While) {
    std::string_view source = R"(
while true { }
)";
    Result<AST, Error> result = parse(source);
    EXPECT_FALSE(result.hasError());
}

TEST(ParserTest, SimpleTest2) {
    std::string_view source = R"(
x = 1 y += 2 z -= 3;
a *= x ; b /= 0 ; c %= 10
)";
    Result<AST, Error> result = parse(source);
    EXPECT_FALSE(result.hasError());

    source = R"(
{
    x = 1
    y = 2
    z = 3
}
)";
    result = parse(source);
    EXPECT_FALSE(result.hasError());
}

TEST(ParserTest, SimpleTest11) {
    std::string_view source = "while true { }";
    Result<AST, Error> result = parse(source);
    EXPECT_FALSE(result.hasError());
}

TEST(ParserTest, SimpleTest12) {
    std::string_view source = "b = if x == 1 then true else false";
    Result<AST, Error> result = parse(source);
    EXPECT_FALSE(result.hasError());
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
    Result<AST, Error> result = parse(source);
    EXPECT_FALSE(result.hasError());
}

