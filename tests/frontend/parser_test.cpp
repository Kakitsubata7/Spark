#include <gtest/gtest.h>

#include "frontend/ast/ast.hpp"
#include "frontend/parser.hpp"
#include "frontend/parser_error.hpp"
#include "result.hpp"

using namespace Spark;
using namespace Spark::FrontEnd;

Result<AST::AST, ParserError> parse(std::string_view source) {
    std::istringstream iss{std::string(source)};
    Parser parser;
    return parser.parse(iss);
}

TEST(ParserTest, GeneralTest1) {
    std::string_view source = R"(while true do end)";
    auto result = parse(source);
    std::cout << result.error().message << std::endl;
    EXPECT_TRUE(result.hasValue());
}
