#include <gtest/gtest.h>

#include "compiler/ast.hpp"
#include "compiler/parser.hpp"
#include "compiler/parser_error.hpp"
#include "result.hpp"

using namespace Spark;
using namespace Spark::Compiler;

Result<AST::AST, ParserError> parse(std::string_view source) {
    std::istringstream iss{std::string(source)};
    Parser parser;
    return parser.parse(iss);
}

TEST(ParserTest, GeneralTest1) {
    std::string_view source = R"(while true do end)";
    auto result = parse(source);
    EXPECT_TRUE(result.hasValue()) << result.error().message;
}
